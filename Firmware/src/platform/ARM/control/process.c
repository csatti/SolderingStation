/* Copyright (C) 2018, Attila Kovács
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation  and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of any
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "process.h"
#include "analog.h"
#include "message.h"
#include "timing.h"
#include "arm_math.h"

#define MAX_DELAY				pdMS_TO_TICKS(40)
#define TUNING_READY			50
#define RETUNE_DIFF				30

#define PROCESS_TASK_PRIORITY       ( configMAX_PRIORITIES - 2 )

static TaskHandle_t _thProcessTask = NULL;
static uint8_t _u8Tuning = 1;
static uint8_t _u8TuningReady = 0;
static uint32_t _u32SampleNumForTuning = 0;
static uint32_t _u32TunedMidVoltage, _u32TunedMidCurrent;

volatile uint8_t* pu8VoltageScope;
volatile uint8_t* pu8CurrentScope;
volatile uint8_t _u8ScopeUsed = 1;
volatile uint16_t u16Marker[MARKER_NUM];

volatile uint32_t u32ProcessRMSPower, u32ProcessRMSVoltage, u32ProcessRMSCurrent, u32ProcessMaxVoltage, u32ProcessMaxCurrent;
volatile uint32_t u32FilteredPower, u32FilteredVoltage, u32FilteredCurrent;

volatile uint32_t *_pu32VoltageSamples, *_pu32CurrentSamples, *_pu32PowerSamples;

static volatile SemaphoreHandle_t _shMutexMeasurements  = NULL;
static volatile SemaphoreHandle_t _shMutexScope  = NULL;

static TaskHandle_t _thScopeTask = NULL;

// Wait for event notification from interrupt handlers
static uint32_t _waitForEvent(uint32_t notify)
{
	BaseType_t result;
	uint32_t notifyVal = 0, prevVal = 0;

	// Wait until notification
	do {
		// Store accumulated events
		prevVal = notifyVal;
		// Wait for notification
		result = xTaskNotifyWait( pdFALSE, notify | NOTIFY_ADCFAULT, &notifyVal, MAX_DELAY );
		// Combine new event with previous events
		notifyVal |= prevVal;
		// If no event in allocated time, generate fault
		if ((result != pdPASS) || (notifyVal & NOTIFY_ADCFAULT)) {
			return 0;
		}
		// Wait until all required event is collected
	} while ((notifyVal & notify) != notify);

	return notifyVal;
}

static void _processCollectedData(void)
{
	static uint32_t meanvtgSum = 0, meancurSum = 0;

	uint16_t meanvtg, meancur, rmsvtg, rmscur, maxvtg, maxcur;
	uint32_t dummy;

	if (!timingIsFrequencyDetermined()) {
		u32ProcessRMSPower = 0;
		u32ProcessRMSVoltage = 0;
		u32ProcessRMSCurrent = 0;
		return;
	}

	if (_u8Tuning) {
		arm_mean_q15((q15_t*)pu16AnalogBufferVoltage, u32AnalogCompensatedBufferLength, (q15_t*)&meanvtg);
		arm_mean_q15((q15_t*)pu16AnalogBufferCurrent, u32AnalogCompensatedBufferLength, (q15_t*)&meancur);
		meanvtgSum += (uint32_t)meanvtg;
		meancurSum += (uint32_t)meancur;

		_u32SampleNumForTuning++;
		if (_u32SampleNumForTuning >= TUNING_READY) {
			_u8Tuning = 0;
			_u8TuningReady = 1;
			_u32TunedMidVoltage = meanvtgSum / _u32SampleNumForTuning;
			_u32TunedMidCurrent = meancurSum / _u32SampleNumForTuning;
		}
	} else if (!_u8Tuning) {

		uint32_t m[MARKER_NUM] = { u32MarkerMosfetStart, u32MarkerLowSwitchOff, u32MarkerPeak, u32MarkerZeroCross };


		// Offset buffers to mid level
		q15_t qmid = -((q15_t)_u32TunedMidVoltage);
		arm_offset_q15((q15_t*)pu16AnalogBufferVoltage, qmid, (q15_t*)pu16AnalogBufferVoltage, u32AnalogCompensatedBufferLength);
		qmid = -((q15_t)_u32TunedMidCurrent);
		arm_offset_q15((q15_t*)pu16AnalogBufferCurrent, qmid, (q15_t*)pu16AnalogBufferCurrent, u32AnalogCompensatedBufferLength);

		// Calculate RMS values
		arm_rms_q15((q15_t*)pu16AnalogBufferVoltage, u32AnalogCompensatedBufferLength, (q15_t*)&rmsvtg);
		arm_rms_q15((q15_t*)pu16AnalogBufferCurrent, u32AnalogCompensatedBufferLength, (q15_t*)&rmscur);

		// Calculate absolute maximum values
		arm_abs_q15((q15_t*)pu16AnalogBufferVoltage, (q15_t*)pu16AnalogBufferVoltage, u32AnalogCompensatedBufferLength);
		arm_abs_q15((q15_t*)pu16AnalogBufferCurrent, (q15_t*)pu16AnalogBufferCurrent, u32AnalogCompensatedBufferLength);
		arm_max_q15((q15_t*)pu16AnalogBufferVoltage, u32AnalogCompensatedBufferLength, (q15_t*)&maxvtg, &dummy);
		arm_max_q15((q15_t*)pu16AnalogBufferCurrent, u32AnalogCompensatedBufferLength, (q15_t*)&maxcur, &dummy);

		uint32_t rmsVoltage, rmsCurrent, rmsPower;
		uint32_t *voltageBuf = (uint32_t*)_pu32VoltageSamples;
		uint32_t *currentBuf = (uint32_t*)_pu32CurrentSamples;
		uint32_t *powerBuf = (uint32_t*)_pu32PowerSamples;
		uint32_t sumVoltage = 0;
		uint32_t sumCurrent = 0;
		uint32_t sumPower = 0;

		// U/21 at 5V range
		// 5V to 3.3V conversion by resistor divider
		rmsVoltage = ((uint32_t)rmsvtg * (1000 * 21 * 5)) / 4095; // mV
		// Current sensor: 100mV / A at 5V range
		// 5V to 3.3V conversion by resistor divider
		rmsCurrent = ((uint32_t)rmscur * (1000 * 10 * 5)) / 4095; // mA
		rmsPower = (rmsVoltage * rmsCurrent) / 1000; // mW
		for (uint16_t i = 0; i < AVERAGING_SAMPLENUM - 1; i++)
		{
			*voltageBuf = *(voltageBuf + 1);
			sumVoltage += *voltageBuf++;
			*currentBuf = *(currentBuf + 1);
			sumCurrent += *currentBuf++;
			*powerBuf = *(powerBuf + 1);
			sumPower += *powerBuf++;
		}
		*voltageBuf = rmsVoltage;
		sumVoltage += *voltageBuf;
		*currentBuf = rmsCurrent;
		sumCurrent += *currentBuf;
		*powerBuf = rmsPower;
		sumPower += *powerBuf;


		if (xSemaphoreTake(_shMutexMeasurements, 0) == pdTRUE) {
			// U/21 at 5V range
			// 5V to 3.3V conversion by resistor divider
			u32ProcessRMSVoltage = rmsVoltage; // mV
			u32ProcessMaxVoltage = ((uint32_t)maxvtg * (1000 * 21 * 5)) / 4095; // mV

			// Current sensor: 100mV / A at 5V range
			// 5V to 3.3V conversion by resistor divider
			if (rmscur < 1500)	 {
				// Removing glitches
				u32ProcessRMSCurrent = rmsCurrent; // mA
				u32ProcessMaxCurrent = ((uint32_t)maxcur * (1000 * 10 * 5)) / 4095; // mA

				u32ProcessRMSPower = rmsPower; // mW

			}

			u32FilteredCurrent = sumCurrent / AVERAGING_SAMPLENUM;
			u32FilteredVoltage = sumVoltage / AVERAGING_SAMPLENUM;
			u32FilteredPower = sumPower / AVERAGING_SAMPLENUM;
			xSemaphoreGive(_shMutexMeasurements);
		}

		if (_u8ScopeUsed) {
			if (xSemaphoreTake(_shMutexScope, 0) == pdTRUE) {
				uint16_t* voltage = (uint16_t*)pu16AnalogBufferVoltage;
				uint16_t* current = (uint16_t*)pu16AnalogBufferCurrent;
				uint8_t* scopeVoltage = (uint8_t*)pu8VoltageScope;
				uint8_t* scopeCurrent = (uint8_t*)pu8CurrentScope;
				uint32_t notifyVal = NOTIFY_NEWDATATRIGGER;

				if (rmsCurrent > HEATCURRENT_TRIGGER) notifyVal |= NOTIFY_HEATTRIGGER;

				for (int i = 0; i < MARKER_NUM; i++)
				{
					u16Marker[i] = m[i] / SCOPE_STEP;
				}

				for (int i = 0; i < u32AnalogCompensatedBufferLength / SCOPE_STEP; i++)
				{
					*scopeVoltage++ = (uint8_t)(*voltage >> 4);
					voltage += SCOPE_STEP;
					*scopeCurrent++ = (uint8_t)(*current >> 4);
					current += SCOPE_STEP;
				}
				_u8ScopeUsed = 0;
				xSemaphoreGive(_shMutexScope);
				if (_thScopeTask != NULL) {
					xTaskNotify( _thScopeTask, notifyVal, eSetBits);
				}

			}
		}


	}

}

static void _processTask(void *pvParameters)
{
	analogSetProcessTask(_thProcessTask);
	while (1)
	{
		uint32_t val = 0;

		do
		{
			val |= _waitForEvent(NOTIFY_ADCVOLTAGE | NOTIFY_ADCCURRENT);
		} while (val != (NOTIFY_ADCVOLTAGE | NOTIFY_ADCCURRENT));

		_processCollectedData();
	}
}

void processInit(void)
{
#define SAMPLEBUF_SIZE (AVERAGING_SAMPLENUM * sizeof(uint32_t))

	xTaskCreate(_processTask, TXT("Process"), 1000, NULL, PROCESS_TASK_PRIORITY, &_thProcessTask );

	_shMutexMeasurements = xSemaphoreCreateMutex();
	_shMutexScope = xSemaphoreCreateMutex();

	vQueueAddToRegistry(_shMutexMeasurements, TXT("ProcMx"));
	vQueueAddToRegistry(_shMutexScope, TXT("ScopMx"));

	pu8VoltageScope = (uint8_t*)pvPortMalloc(SCOPE_LEN);
	pu8CurrentScope = (uint8_t*)pvPortMalloc(SCOPE_LEN);
	_pu32VoltageSamples = (uint32_t*)pvPortMalloc(SAMPLEBUF_SIZE);
	_pu32CurrentSamples = (uint32_t*)pvPortMalloc(SAMPLEBUF_SIZE);
	_pu32PowerSamples = (uint32_t*)pvPortMalloc(SAMPLEBUF_SIZE);

	memset((void*)pu8VoltageScope, 0, SCOPE_LEN);
	memset((void*)pu8CurrentScope, 0, SCOPE_LEN);
	memset((void*)_pu32VoltageSamples, 0, SAMPLEBUF_SIZE);
	memset((void*)_pu32CurrentSamples, 0, SAMPLEBUF_SIZE);
	memset((void*)_pu32PowerSamples, 0, SAMPLEBUF_SIZE);
#undef SAMPLEBUF_SIZE
}

uint8_t processIsTuned(void)
{
	return !_u8Tuning;
}

BaseType_t processLockDataMaxWait(uint32_t maxwait)
{
	return xSemaphoreTake(_shMutexMeasurements, maxwait);
}

void processUnlockData(void)
{
	xSemaphoreGive(_shMutexMeasurements);
}

BaseType_t processLockScopeMaxWait(uint32_t maxwait)
{
	return xSemaphoreTake(_shMutexScope, maxwait);
}

void processUnlockScope(void)
{
	xSemaphoreGive(_shMutexScope);
}

void processSetScopeTask(TaskHandle_t task)
{
	processLockScopeMaxWait(portMAX_DELAY);
	_thScopeTask = task;
	processUnlockScope();
}

void processSetScopeDataIsUsed(void)
{
	_u8ScopeUsed = 1;
}


