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

#ifndef PLATFORM_ARM_CONTROL_PROCESS_H_
#define PLATFORM_ARM_CONTROL_PROCESS_H_

#include "main.h"

#define SCOPE_STEP				10
#define SCOPE_LEN				(AN_ADC2_3_BUFFSIZE / (2 * SCOPE_STEP))
#define MARKER_NUM				4
#define HEATCURRENT_TRIGGER		500	// mA
#define AVERAGING_SAMPLENUM		40

extern volatile uint8_t* pu8VoltageScope;
extern volatile uint8_t* pu8CurrentScope;
extern volatile uint16_t u16Marker[MARKER_NUM];

extern volatile uint32_t u32ProcessRMSPower, u32ProcessRMSVoltage, u32ProcessRMSCurrent, u32ProcessMaxVoltage, u32ProcessMaxCurrent;
extern volatile uint32_t u32FilteredPower, u32FilteredVoltage, u32FilteredCurrent;

void processInit(void);
uint8_t processIsTuned(void);

#define processLockDataNow() 			processLockDataMaxWait(0)
#define processLockData() 				processLockDataMaxWait(portMAX_DELAY)
BaseType_t processLockDataMaxWait(uint32_t maxwait);
void processUnlockData(void);

#define processLockScopeNow() 			processLockScopeMaxWait(0)
#define processLockScope() 				processLockScopeMaxWait(portMAX_DELAY)
BaseType_t processLockScopeMaxWait(uint32_t maxwait);
void processUnlockScope(void);

void processSetScopeTask(TaskHandle_t task);
void processSetScopeDataIsUsed(void);

#endif /* PLATFORM_ARM_CONTROL_PROCESS_H_ */
