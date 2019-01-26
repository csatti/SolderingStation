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

#ifndef PLATFORM_ARM_BOARD_ANALOG_H_
#define PLATFORM_ARM_BOARD_ANALOG_H_

#include "main.h"

extern volatile uint16_t* pu16AnalogBufferVoltage;
extern volatile uint16_t* pu16AnalogBufferCurrent;
extern volatile uint32_t u32AnalogFreqTicks;
volatile uint32_t u32AnalogCompensatedBufferLength;

void analogInit(void);

uint16_t analogGetMuxChannel(MuxChannels_t channel);
uint16_t analogGetChannel(AnalogChannels_t channel);

uint16_t analogGetSupplyVoltage(void);
uint16_t analogGetStoredSupplyVoltage(void);
uint16_t analogGetMCUTemperature(void);
uint16_t analogGetStoredMCUTemperature(void);
uint16_t analogGetAmbientTemp(void);			// in 0.1 °F
void analogActivateADCWatchdog(uint16_t target);
void analogStartSampling(void);

void analogSetProcessTask(TaskHandle_t task);

uint8_t analogIsFrequencyDetermined(void);

#endif /* PLATFORM_ARM_BOARD_ANALOG_H_ */
