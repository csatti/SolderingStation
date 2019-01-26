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

#include "gui_priv.h"

static TimerHandle_t _thScreenSaver;
static uint8_t _u8ScreenOn = 1;

// Screen saver timer callback
static void _screenSaverTimerCallback(TimerHandle_t xTimer)
{
	_u8ScreenOn = 0;
	gdispSetBacklight(0);
}

void guiScreenSaverInit(void)
{
	_thScreenSaver = xTimerCreate( TXT("ScreenTimer"), portMAX_DELAY,
			pdTRUE, (void *) 0, _screenSaverTimerCallback);
	xTimerStart(_thScreenSaver, portMAX_DELAY);
}

void guiSetScreenSaver(uint16_t time)
{
	xTimerChangePeriod(_thScreenSaver, ((uint32_t)time * 60 * 1000) / portTICK_PERIOD_MS, portMAX_DELAY);
}

StationEvent_t guiGetStationEvent(void)
{
	StationEvent_t event = uiGetStationEvent();

	if (event != SE_NONE) {
		xTimerReset(_thScreenSaver, portMAX_DELAY);
		if (!_u8ScreenOn) {
			_u8ScreenOn = 1;
			gdispSetBacklight(100);
		}
	}

	return event;
}




