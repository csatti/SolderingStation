/* Copyright (C) 2018 - 2019, Attila Kovács
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

#include "analog.h"
#include "timing.h"
#include "utility.h"

#define WATCHDOG_HYSTERESIS		5

#define MUTEX_ENTER()	xSemaphoreTake(mutexADC1, portMAX_DELAY)
#define MUTEX_EXIT()	xSemaphoreGive(mutexADC1)

static volatile uint16_t adc1Buffer[AN_ADC1_BUFFSIZE];
static volatile uint16_t adc2Buffer[AN_ADC2_3_BUFFSIZE];
static volatile uint16_t adc3Buffer[AN_ADC2_3_BUFFSIZE];

volatile uint32_t u32AnalogCompensatedBufferLength = AN_ADC2_3_BUFFSIZE / 2;
volatile uint16_t* pu16AnalogBufferVoltage;
volatile uint16_t* pu16AnalogBufferCurrent;

static volatile TaskHandle_t _adcTask = NULL;
static volatile TaskHandle_t processTask = NULL;

static volatile uint8_t watchdogToggle = 0;
static volatile uint16_t watchdogTarget = 0;
volatile uint32_t u32AnalogFreqTicks = 0;

static volatile uint8_t validFreqTiming = 0;


static volatile SemaphoreHandle_t mutexADC1  = NULL;

static volatile uint16_t halfWaves[10];
static volatile uint16_t halfWavePos = 0;

static volatile uint16_t sineLength = 0;
static volatile uint16_t sineSample = 0;
static volatile uint16_t prevSineLength = 0;

static volatile uint16_t _supplyVoltage = 0;
static volatile uint16_t _mcuTemp = 0;

ISR(AN_ADC_IRQHandler)
{
	uint16_t cnt;


	if (LL_ADC_IsActiveFlag_AWD1(AN_ADC2)) {
		if (watchdogToggle) {
			halfWaves[halfWavePos] = cnt = timingGetCounter();
			halfWavePos++;
			halfWavePos %= 10;
			timingRestartTimer();
			// No valid frequency timing yet (autocalibrate 50Hz / 60Hz with crystal imperfections)
			if (!validFreqTiming) {
				// Drop first sample, probably partial wave
				if (sineSample) sineLength = cnt;
				// If we have already two results and sineLength is valid
				if ((prevSineLength) && (sineLength > 15000)) {
					// Hysteresis check - sanity
					if ((prevSineLength - 5 < sineLength) && (prevSineLength + 5 > sineLength)) {
						validFreqTiming = 1;
						timingSetHalfWaveCountISR(sineLength / 2);

						uint64_t dataRate = AN_ADC2_3_SAMPLERATE;
						dataRate *= (uint64_t)sineLength;
						dataRate /= 1000000;
						// Adjust buffer size for full wave
						u32AnalogCompensatedBufferLength = (uint16_t)dataRate;
						if (u32AnalogCompensatedBufferLength > (AN_ADC2_3_BUFFSIZE / 2)) u32AnalogCompensatedBufferLength = AN_ADC2_3_BUFFSIZE / 2;

						LL_DMA_SetDataLength(AN_ADC_DMA, AN_ADC2_DMA_Stream, u32AnalogCompensatedBufferLength * 2);
						LL_DMA_SetDataLength(AN_ADC_DMA, AN_ADC3_DMA_Stream, u32AnalogCompensatedBufferLength * 2);
					} else {
						// Try again
						prevSineLength = 0;
						sineLength = 0;
					}
				} else {
					prevSineLength = sineLength;
					sineLength = 0;
				}
				sineSample++;
			}
		}

		if (watchdogToggle) {
			LL_ADC_SetAnalogWDThresholds(AN_ADC2, LL_ADC_AWD_THRESHOLD_HIGH, watchdogTarget + WATCHDOG_HYSTERESIS);
			LL_ADC_SetAnalogWDThresholds(AN_ADC2, LL_ADC_AWD_THRESHOLD_LOW, 0);
			//MONITOR_1_SET();
		} else {
			LL_ADC_SetAnalogWDThresholds(AN_ADC2, LL_ADC_AWD_THRESHOLD_HIGH, 0x0FFF);
			LL_ADC_SetAnalogWDThresholds(AN_ADC2, LL_ADC_AWD_THRESHOLD_LOW, watchdogTarget - WATCHDOG_HYSTERESIS);
			//MONITOR_1_CLR();
		}
		watchdogToggle = !watchdogToggle;
		u32AnalogFreqTicks++;
		LL_ADC_ClearFlag_AWD1(AN_ADC2);

	}
}

void analogActivateADCWatchdog(uint16_t target)
{
	validFreqTiming = 1;
	return; // Disable Analog watchdog based calibration
	watchdogTarget = target;
	LL_ADC_SetAnalogWDMonitChannels(AN_ADC2, LL_ADC_AWD_ALL_CHANNELS_REG);
	LL_ADC_SetAnalogWDThresholds(AN_ADC2, LL_ADC_AWD_THRESHOLD_HIGH, target + WATCHDOG_HYSTERESIS);
	LL_ADC_SetAnalogWDThresholds(AN_ADC2, LL_ADC_AWD_THRESHOLD_LOW, 0);
	LL_ADC_EnableIT_AWD1(AN_ADC2);
}

void analogInit(void)
{
	boardADCConfig();
	boardMUXConfig();

#ifdef MONITORING
	Monitoring_Config();
#endif

	mutexADC1 = xSemaphoreCreateMutex();

	vQueueAddToRegistry(mutexADC1, TXT("ADCMx"));

	// Setup DMA transactions
	AN_ADC2_DMA_ClearTCFlag();
	AN_ADC2_DMA_ClearHTFlag();
	LL_DMA_DisableStream(AN_ADC_DMA, AN_ADC2_DMA_Stream);
	LL_DMA_SetDataLength(AN_ADC_DMA, AN_ADC2_DMA_Stream, AN_ADC2_3_BUFFSIZE);
	LL_DMA_SetMemoryAddress(AN_ADC_DMA, AN_ADC2_DMA_Stream, (uint32_t)adc2Buffer);
	LL_DMA_EnableIT_HT(AN_ADC_DMA, AN_ADC2_DMA_Stream);
	LL_DMA_EnableIT_TC(AN_ADC_DMA, AN_ADC2_DMA_Stream);
	AN_ADC3_DMA_ClearTCFlag();
	AN_ADC3_DMA_ClearHTFlag();
	LL_DMA_DisableStream(AN_ADC_DMA, AN_ADC3_DMA_Stream);
	LL_DMA_SetDataLength(AN_ADC_DMA, AN_ADC3_DMA_Stream, AN_ADC2_3_BUFFSIZE);
	LL_DMA_SetMemoryAddress(AN_ADC_DMA, AN_ADC3_DMA_Stream, (uint32_t)adc3Buffer);
	LL_DMA_EnableIT_HT(AN_ADC_DMA, AN_ADC3_DMA_Stream);
	LL_DMA_EnableIT_TC(AN_ADC_DMA, AN_ADC3_DMA_Stream);
}

void analogStartSampling(void)
{
	// SW trigger
	LL_DMA_EnableStream(AN_ADC_DMA, AN_ADC2_DMA_Stream);
	LL_DMA_EnableStream(AN_ADC_DMA, AN_ADC3_DMA_Stream);
	LL_ADC_REG_StartConversionSWStart(AN_ADC2);
	LL_ADC_REG_StartConversionSWStart(AN_ADC3);
}

void analogSetProcessTask(TaskHandle_t task)
{
	processTask = task;
}

ISR(AN_ADC2_DMA_IRQHandler)
{
	BaseType_t higherPriorityTaskWoken;

	if (AN_ADC2_DMA_IsTCActive()) {
		AN_ADC2_DMA_ClearTCFlag();
		pu16AnalogBufferVoltage = adc2Buffer + u32AnalogCompensatedBufferLength;
	} else if (AN_ADC2_DMA_IsHTActive()) {
		AN_ADC2_DMA_ClearHTFlag();
		pu16AnalogBufferVoltage = adc2Buffer;
	}

	// Send DMA complete notification to task
	if (processTask != NULL) {
		xTaskNotifyFromISR( processTask, NOTIFY_ADCVOLTAGE, eSetBits, &higherPriorityTaskWoken);
		portYIELD_FROM_ISR( higherPriorityTaskWoken );
	}
	__DSB();
}

ISR(AN_ADC3_DMA_IRQHandler)
{
	BaseType_t higherPriorityTaskWoken;

	if (AN_ADC3_DMA_IsTCActive()) {
		AN_ADC3_DMA_ClearTCFlag();
		pu16AnalogBufferCurrent = adc3Buffer + u32AnalogCompensatedBufferLength;
	} else if (AN_ADC3_DMA_IsHTActive()) {
		AN_ADC3_DMA_ClearHTFlag();
		pu16AnalogBufferCurrent = adc3Buffer;
	}

	// Send DMA complete notification to task
	if (processTask != NULL) {
		xTaskNotifyFromISR( processTask, NOTIFY_ADCCURRENT, eSetBits, &higherPriorityTaskWoken);
		portYIELD_FROM_ISR( higherPriorityTaskWoken );
	}
	__DSB();
}

ISR(AN_ADC1_DMA_IRQHandler)
{
	BaseType_t higherPriorityTaskWoken;

	LL_DMA_DisableIT_TC(AN_ADC_DMA, AN_ADC1_DMA_Stream);
	LL_DMA_DisableStream(AN_ADC_DMA, AN_ADC1_DMA_Stream);
	AN_ADC1_DMA_ClearTCFlag();
	if (_adcTask) {
		vTaskNotifyGiveFromISR(_adcTask, &higherPriorityTaskWoken);
		portYIELD_FROM_ISR( higherPriorityTaskWoken );
	} 
	__DSB();
}

static void setMuxChannel(MuxChannels_t channel)
{
	if (channel & 0x01) MUX_1_SET();
	else MUX_1_CLR();

	if (channel & 0x02) MUX_2_SET();
	else MUX_2_CLR();

	if (channel & 0x04) MUX_3_SET();
	else MUX_3_CLR();
}

static uint16_t getChannel(uint32_t ch)
{
	__IO uint16_t val = 0;

	// Setup sequence
	AN_ADC1->SQR3 = ch | (ch << 5U) | (ch << 10U) | (ch << 15U) | (ch << 20U)  | (ch << 25U);
	AN_ADC1->SQR2 = ch | (ch << 5U);

	// Setup DMA transaction
	AN_ADC1_DMA_ClearTCFlag();
	LL_DMA_DisableStream(AN_ADC_DMA, AN_ADC1_DMA_Stream);
	LL_DMA_SetDataLength(AN_ADC_DMA, AN_ADC1_DMA_Stream, AN_ADC1_BUFFSIZE);
	LL_DMA_SetMemoryAddress(AN_ADC_DMA, AN_ADC1_DMA_Stream, (uint32_t)adc1Buffer);
	LL_DMA_DisableIT_HT(AN_ADC_DMA, AN_ADC1_DMA_Stream);
	LL_DMA_EnableIT_TC(AN_ADC_DMA, AN_ADC1_DMA_Stream);
	LL_DMA_EnableStream(AN_ADC_DMA, AN_ADC1_DMA_Stream);

	_adcTask = xTaskGetCurrentTaskHandle();

	xTaskNotifyStateClear(NULL);
	// SW trigger
	LL_ADC_REG_StartConversionSWStart(AN_ADC1);

	// Wait until done
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);


	// Drop first value
	for (int i = 1; i < AN_ADC1_BUFFSIZE; i++)
	{
		val += adc1Buffer[i] & 0x0FFF;
	}

	val /= (AN_ADC1_BUFFSIZE - 1);

	return val;
}

uint16_t analogGetMuxChannel(MuxChannels_t channel)
{
	uint32_t ch = MULTIPLEXER & ADC_CHANNEL_ID_NUMBER_MASK;
	uint16_t val;

	MUTEX_ENTER();
	setMuxChannel(channel);
	val = getChannel(ch);
	MUTEX_EXIT();

	return val;
}

uint16_t analogGetChannel(AnalogChannels_t channel)
{
	uint32_t ch = channel & ADC_CHANNEL_ID_NUMBER_MASK;
	uint16_t val;

	MUTEX_ENTER();
	val = getChannel(ch);
	MUTEX_EXIT();

	return val;
}

// Return value in mV
uint16_t analogGetSupplyVoltage(void)
{
	uint16_t factoryCalibVref = *ADC_VREFINTCAL;
	uint16_t factoryCalibTemp30 = *ADC_TEMPINTCAL30;
	uint16_t factoryCalibTemp110 = *ADC_TEMPINTCAL110;

	uint32_t intRefRaw = (uint32_t)analogGetChannel(INTERNAL_VREF);
	uint32_t intTempRaw = (uint32_t)analogGetChannel(INTERNAL_TEMP);
	uint32_t res;
	int32_t temp;
	int32_t tempscale;

	static uint32_t filteredRef = 0, filteredTemp = 0;

	res = (3300L * (uint32_t)factoryCalibVref) / intRefRaw;

	// Limiting
	if ((res > 4000) || (res < 2000)) res = 3300;

	filterSignal(&filteredRef, res);
	res = filteredSignalValue(filteredRef);

	temp = (res * (int32_t)intTempRaw * 10) / 33L; // Temperature in uV
	filterSignal(&filteredTemp, (uint32_t)temp);
	temp = (int32_t)filteredSignalValue(filteredTemp);

	tempscale = ((factoryCalibTemp110 - factoryCalibTemp30) * (330000 / 80)) / 0x0FFF; // Get tempscale in 10uV / °C

	temp -= (int32_t)factoryCalibTemp30 * 1000; // Remove 30°C offset

	temp /= tempscale; // in 0.1°C above 30°C

	_mcuTemp = (uint16_t)(860 + (temp * 18) / 10);		// in 0.1°F

	temp *= 3;  // in ppm

	int32_t comp = ((int32_t)res * temp) / 1000000L;

	res -= comp;

	// Limiting
	if ((res > 4000) || (res < 2000)) res = 3300;

	_supplyVoltage = (uint16_t)res;

	return (uint16_t)res;
}

uint16_t analogGetStoredSupplyVoltage(void)
{
	return _supplyVoltage;
}

uint16_t analogGetMCUTemperature(void)
{
	analogGetSupplyVoltage();
	return _mcuTemp;
}

uint16_t analogGetStoredMCUTemperature(void)
{
	return _mcuTemp;
}

uint16_t analogGetAmbientTemp(void)
{
	uint32_t ambiRaw = (uint32_t)analogGetChannel(AMBIENT_TEMP);
	uint32_t supplyVtg = 3300;  // Just force this
	int32_t res;

	analogGetSupplyVoltage();
	res = (ambiRaw * supplyVtg) / 0x0FFF; // 12 bit conversion

	// MCP9701
	//   400mV at 0°C
	//   19.5mV  / °C increase
	//   10.83mV / °F increase

	res -= 400;  // in mV (offset to 0°C)
	res*= 1000;  // in uV (offset to 0°C)

	res /= 1083; // in 0.1 °F (offset -32 °F)
	res += 320;  // in 0.1 °F

	if (res < 0) res = 0; // We don't handle negative temperature for now

	return (uint16_t)res;
}

uint8_t analogIsFrequencyDetermined(void)
{
	validFreqTiming = 1;
	return validFreqTiming;
}
