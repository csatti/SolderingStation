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

#ifndef PLATFORM_WIN32_CONTROL_CONTROL_H_
#define PLATFORM_WIN32_CONTROL_CONTROL_H_

#include "main.h"
#include "tool.h"

#define SCOPE_LEN	275
#define MARKER_NUM	4

extern uint16_t u16Marker[MARKER_NUM];
extern uint8_t* pu8VoltageScope;
extern uint8_t* pu8CurrentScope;

void controlInit(void);

#define controlAddHeatPulses(pls)  	((void) 0)

#define controlGetDataExchangeNow() 	controlGetDataExchangeMaxWait(0)
#define controlGetDataExchange() 		controlGetDataExchangeMaxWait(portMAX_DELAY)
DataExchange_t* controlGetDataExchangeMaxWait(uint32_t maxwait);
void controlReturnDataExchange(void);

#define processLockScopeNow() 			((void) 0)
void processLockScope(void);
#define processLockScopeMaxWait(x) 		((void) 0)
#define processUnlockScope()			((void) 0)

void processSetScopeTask(TaskHandle_t task);
void processSetScopeDataIsUsed(void);

#endif /* PLATFORM_WIN32_CONTROL_CONTROL_H_ */
