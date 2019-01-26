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

// Temperature conversions
static uint16_t _internalToCelsius(uint16_t intTemp)
{
	intTemp -= 320;
	if (intTemp % 18 > 8)
		intTemp += 9;
	return intTemp / 18;
}

static uint16_t _internalToFahrenheit(uint16_t intTemp)
{
	if (intTemp % 10 > 4)
		intTemp += 5;
	return intTemp / 10;
}

uint16_t guiInternalToDisplay(uint16_t intTemp)
{
	ConfigOnline_t* ConfigOnline = configGetConfigOnlineNoWrite();

	if (ConfigOnline->tempUnitC) {
		return _internalToCelsius(intTemp);
	} else {
		return _internalToFahrenheit(intTemp);
	}
}

// Decrease value, roll over to maximum
void guiDecreaseValue(uint16_t* val, uint16_t limit)
{
	if (*val) {
		(*val)--;
	} else {
		*val = limit - 1;
	}
}

// Increase value, roll over to minimum
void guiIncreaseValue(uint16_t* val, uint16_t limit)
{
	(*val)++;
	if (*val >= limit)
		*val = 0;
}

// Sets up the footer buttons
void guiSetFooterButtons(font_t font, const char* text1, const char* text2,
		const char* text3)
{
	gwinSetFont(ghPreset1button, font);
	gwinSetFont(ghPreset2button, font);
	gwinSetFont(ghPreset3button, font);
	gwinSetText(ghPreset1button, text1, FALSE);
	gwinSetText(ghPreset2button, text2, FALSE);
	gwinSetText(ghPreset3button, text3, FALSE);
}

// Change text without redrawing
void guiSimpleSetText(GHandle gh, const char* text)
{
	((GWidgetObject *)gh)->text = text;
}

// Common tool control
void guiEventCheckToolControl(StationEvent_t event)
{
	DataExchange_t* DataExchange;

	if ((event != SE_TOOL1_UP) && (event != SE_TOOL2_UP)) return;

	DataExchange = controlGetDataExchange();

	switch (event) {
	case SE_TOOL1_UP:
		DataExchange->control.tool1 = 1;
		break;
	case SE_TOOL2_UP:
		DataExchange->control.tool2 = 1;
		break;
	default:
		;
	}
	controlReturnDataExchange();
}


