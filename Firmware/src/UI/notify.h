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

#ifndef UI_NOTIFY_H_
#define UI_NOTIFY_H_

#include "board.h"

typedef enum {
	NONE,
	ALARM_CALIBRATION,
	ALARM_OVERVOLTAGE,
	ALARM_OVERCURRENT,
	ALARM_HIGHTIPTEMPERATURE,
	ALARM_HIGHAMBIENTTEMPERATURE,
	WARNING_BATTERY,
	WARNING_DEBUGMODE,
	WARNING_UNCALIBRATEDIRON,
	WARNING_THERMALTHROTTLING,
	WARNING_REPORTFAILED,
	WARNING_SETTINGSLOST,
	INFO_CALIBRATION,
	INFO_CALIBRATIONDONE,
	INFO_SETTINGSRESET,
} NotifyCodes_t;

void notifyInit(void);
void notifySend(uint32_t message);
uint32_t notifyReceive(void);

#endif /* UI_NOTIFY_H_ */
