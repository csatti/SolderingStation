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

#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include "board.h"
#include "lcd.h"

#define msgPrintC(str, color) DBG_PrintC(str, color)
#define msgPrint(str) DBG_Print(str)
#define msgPrintLnC(str, color) DBG_PrintLnC(str, color)
#define msgPrintLn(str) DBG_PrintLn(str)
#define msgOk() DBG_Ok()
#define msgFail() DBG_Fail()

#define debugSetCmdCallbackFunction(processor)   ((void) 0)
#define debugSendRAW(buff, len)                  ((void) 0)
#define debugWaitTillSending()                   ((void) 0)
#define debugInit()                              ((void) 0)
#define debugPrintC(str, color)                  ((void) 0)
#define debugPrintLnC(str, color)                ((void) 0)
#define debugPrint(str)                          ((void) 0)
#define debugPrintLn(str)                        ((void) 0)
#define debugTimeRef()                           ((void) 0)
#define debugBlockPrint(str)                     ((void) 0)
#define debugOk()                                ((void) 0)
#define debugFail()                              ((void) 0)

#define debugMonitoringInit()        			 ((void) 0)
#define messageInit()							 ((void) 0)

#define telemetryHeader()						 ((void) 0)
#define telemetryU16(x)							 ((void) 0)
#define telemetryFloat(x)						 ((void) 0)
#define telemetrySend();
#define telemetryControl(x)						 ((void) 0)
#define telemetryIsEnabled()					 ((uint8_t)0)

#endif /* MESSAGE_H_INCLUDED */
