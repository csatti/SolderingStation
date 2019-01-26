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

#include "timing.h"
#include "config.h"
#include "analog.h"

#define MAX_DELAY			pdMS_TO_TICKS(40)

static volatile uint16_t _u16HalfWaveCount = 0;
static volatile uint32_t _u32StartPoint, _u32EndPoint, _u32CenterPoint, _u32PrevCenterPoint, _u32PrevLength, _u32TotalLength = 0, _u32Count = 0;
static volatile uint8_t _u8ValidFreqTiming = 0, _u8EnableIT = 0, _u8Heating, _u8ReCal = 0;
volatile uint32_t u32MarkerMosfetStart = 0;
volatile uint32_t u32MarkerLowSwitchOff = 0;
volatile uint32_t u32MarkerPeak = 0;
volatile uint32_t u32MarkerZeroCross = 0;


static TaskHandle_t _thControlTask = NULL;

static uint32_t _posCalc()
{
	uint32_t pos = LL_DMA_GetDataLength(AN_ADC_DMA, AN_ADC2_DMA_Stream);

	if (pos > u32AnalogCompensatedBufferLength) pos -= u32AnalogCompensatedBufferLength;
	pos = u32AnalogCompensatedBufferLength - pos;

	return pos;
}

ISR(TIMER_CTRL_IRQHandler)
{
	BaseType_t higherPriorityTaskWoken;
	uint32_t notifyVal = 0;

	if (LL_TIM_IsActiveFlag_CC1(TIMER_CTRL)) {
		notifyVal |= NOTIFY_MOSFETSTART;
		u32MarkerMosfetStart = _posCalc();
		//MONITOR_1_SET();
		LL_TIM_ClearFlag_CC1(TIMER_CTRL);
	}
	if (LL_TIM_IsActiveFlag_CC2(TIMER_CTRL)) {
		notifyVal |= NOTIFY_LOWSWITCHOFF;
		u32MarkerLowSwitchOff = _posCalc();
		//MONITOR_1_CLR();
		LL_TIM_ClearFlag_CC2(TIMER_CTRL);
	}
	if (LL_TIM_IsActiveFlag_CC3(TIMER_CTRL)) {
		notifyVal |= NOTIFY_PEAK;
		u32MarkerPeak = _posCalc();
		//MONITOR_1_SET();
		LL_TIM_ClearFlag_CC3(TIMER_CTRL);
	}
	if (LL_TIM_IsActiveFlag_CC4(TIMER_CTRL)) {
		notifyVal |= NOTIFY_ZEROCROSS;
		u32MarkerZeroCross = _posCalc();
		//MONITOR_1_CLR();
		LL_TIM_ClearFlag_CC4(TIMER_CTRL);
		//Analog_UpdateIronTempISR();

	}
	if ((_thControlTask != NULL) && (notifyVal)) {
		xTaskNotifyFromISR( _thControlTask, notifyVal, eSetBits, &higherPriorityTaskWoken);
		portYIELD_FROM_ISR( higherPriorityTaskWoken );
	}
	__DSB();
}

ISR(DETECT_EXTI_IRQHandler)
{
	if (LL_EXTI_IsActiveFlag_0_31(DETECT_ZeroCross_EXTI_LINE)) {
	    LL_EXTI_ClearFlag_0_31(DETECT_ZeroCross_EXTI_LINE);
	    if (DETECT_GPIO->IDR & DETECT_ZeroCross_Pin) {
	    	if (_u8ValidFreqTiming) {
	    		TIMER_CTRL->CNT = 0;
	    	} else {
		    	_u32StartPoint = TIMER_CTRL->CNT;
		    	_u8Heating = (HEATING_GPIO->ODR & HEATING_Heating_Pin);
	    	}
	    } else if (!_u8ValidFreqTiming) {

			_u32EndPoint = TIMER_CTRL->CNT;
			_u8Heating ^= (HEATING_GPIO->ODR & HEATING_Heating_Pin);

			if (_u32EndPoint < _u32StartPoint) {
				_u32EndPoint += TIMER_CTRL->ARR;
			}
			_u32CenterPoint = _u32StartPoint + ((_u32EndPoint - _u32StartPoint) / 2);

			if (_u32PrevCenterPoint) {
				if (_u32CenterPoint > _u32PrevCenterPoint) {
					//uint16_t length = (uint16_t)(_u32PrevCenterPoint - cPos);

					_u32Count++;
					// Throw away first 100
					if (_u32Count > 100)
						_u32TotalLength += _u32CenterPoint - _u32PrevCenterPoint;

					if (_u32Count >= 200) {
						uint16_t length = (uint16_t)(_u32TotalLength / (_u32Count - 100));

						if (((length > 8283) && (length < 8383)) || ((length > 9950) && (length < 10050))) {
							if ((_u16HalfWaveCount - 3 < length) && (_u16HalfWaveCount + 3 > length)) {
								_u16HalfWaveCount = 10000; // 50 Hz
								if ((length > 8283) && (length < 8383)) _u16HalfWaveCount = 8333; // 60 Hz
								//_u16HalfWaveCount = length;
								LL_TIM_SetAutoReload(TIMER_CTRL, _u16HalfWaveCount + 2000);
								uint64_t dataRate = AN_ADC2_3_SAMPLERATE;
								dataRate *= (uint64_t)_u16HalfWaveCount;
								dataRate /= 500000;
								// Adjust buffer size for full wave
								u32AnalogCompensatedBufferLength = (uint16_t)dataRate;
								if (u32AnalogCompensatedBufferLength > (AN_ADC2_3_BUFFSIZE / 2)) u32AnalogCompensatedBufferLength = AN_ADC2_3_BUFFSIZE / 2;

								LL_DMA_SetDataLength(AN_ADC_DMA, AN_ADC2_DMA_Stream, u32AnalogCompensatedBufferLength * 2);
								LL_DMA_SetDataLength(AN_ADC_DMA, AN_ADC3_DMA_Stream, u32AnalogCompensatedBufferLength * 2);
								analogStartSampling();
								_u8ValidFreqTiming = 1;
							}
							_u16HalfWaveCount = length;
						} else {
							_u32Count = _u32TotalLength = 0;
							_u32Count = 0;
						}
					}
				}
			}
			_u32PrevCenterPoint = _u32CenterPoint;
		}
	}


	if (LL_EXTI_IsActiveFlag_0_31(DETECT_ToolPresence_EXTI_LINE))
	{
		LL_EXTI_ClearFlag_0_31(DETECT_ToolPresence_EXTI_LINE);
	}
	__DSB();
}


void timingSetHalfWaveCountISR(uint16_t cnt)
{
	_u16HalfWaveCount = cnt;
	LL_TIM_SetAutoReload(TIMER_CTRL, cnt);
}

uint16_t timingGetHalfWaveCount(void)
{
	return _u16HalfWaveCount;
}

uint8_t timingIsFrequencyDetermined(void)
{
	return _u8ValidFreqTiming;
}

void timingUpdateTimings(void)
{
	CalibOffline_t* calib;

	if (!_u16HalfWaveCount) return;

	calib = configGetCalibOffline();
	timingSetTiming(NOTIFY_MOSFETSTART, calib->timings.offsetMosfetSwitchStart);
	//timingSetTiming(NOTIFY_LOWSWITCHOFF, calib->timings.offsetLowVoltageSwitchOff);
	timingSetTiming(NOTIFY_LOWSWITCHOFF, (_u16HalfWaveCount * 2) / 7);
	timingSetTiming(NOTIFY_ZEROCROSS, calib->timings.offsetZeroCross);
	timingSetTiming(NOTIFY_PEAK, _u16HalfWaveCount / 2);
	LL_TIM_EnableIT_CC1(TIMER_CTRL);
	LL_TIM_EnableIT_CC2(TIMER_CTRL);
	LL_TIM_EnableIT_CC3(TIMER_CTRL);
	LL_TIM_EnableIT_CC4(TIMER_CTRL);
	configReturnCalibOffline();
}

void timingSetTiming(uint32_t notification, uint16_t timing)
{
	switch (notification)
	{
		case NOTIFY_MOSFETSTART: 	LL_TIM_OC_SetCompareCH1(TIMER_CTRL, timing); break;
		case NOTIFY_LOWSWITCHOFF: 	LL_TIM_OC_SetCompareCH2(TIMER_CTRL, timing); break;
		case NOTIFY_PEAK: 			LL_TIM_OC_SetCompareCH3(TIMER_CTRL, timing); break;
		case NOTIFY_ZEROCROSS: 		LL_TIM_OC_SetCompareCH4(TIMER_CTRL, timing); break;
		default: break;
	}
}

void timingRestartTimer(void)
{
	LL_TIM_SetCounter(TIMER_CTRL, 0);
}

uint16_t timingGetCounter(void)
{
	return LL_TIM_GetCounter(TIMER_CTRL);
}

void timingInit(void)
{
	boardCTRLTimerConfig();
	boardHeatingConfig();
	boardDetectConfig();
}

void timingSetControlTask(TaskHandle_t task)
{
	_thControlTask = task;
}

