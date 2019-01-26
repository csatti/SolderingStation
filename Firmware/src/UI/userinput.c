/* Copyright (C) 2017 - 2019, Attila Kovács
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

#include "userinput.h"
#include "main.h"
#include "gui.h"
#include <stdlib.h>

typedef struct UIDevice_t {
	uint32_t value;
	uint32_t flags;
} UIDevice;

UIDevice _uidDevices[7] = { 0 };

static GListener	_gl;

#if GINPUT_NEED_KEYBOARD
static void DebugKeys(GEventKeyboard* event)
{
	int i;

	printf("KEYSTATE: 0x%04X [ %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s]",
		event->keystate,
		(!event->keystate ? "NONE " : ""),
		((event->keystate & GKEYSTATE_KEYUP) ? "KEYUP " : ""),
		((event->keystate & GKEYSTATE_REPEAT) ? "REPEAT " : ""),
		((event->keystate & GKEYSTATE_SPECIAL) ? "SPECIAL " : ""),
		((event->keystate & GKEYSTATE_RAW) ? "RAW " : ""),
		((event->keystate & GKEYSTATE_SHIFT_L) ? "LSHIFT " : ""),
		((event->keystate & GKEYSTATE_SHIFT_R) ? "RSHIFT " : ""),
		((event->keystate & GKEYSTATE_CTRL_L) ? "LCTRL " : ""),
		((event->keystate & GKEYSTATE_CTRL_R) ? "RCTRL " : ""),
		((event->keystate & GKEYSTATE_ALT_L) ? "LALT " : ""),
		((event->keystate & GKEYSTATE_ALT_R) ? "RALT " : ""),
		((event->keystate & GKEYSTATE_FN) ? "FN " : ""),
		((event->keystate & GKEYSTATE_COMPOSE) ? "COMPOSE " : ""),
		((event->keystate & GKEYSTATE_WINKEY) ? "WINKEY " : ""),
		((event->keystate & GKEYSTATE_CAPSLOCK) ? "CAPSLOCK " : ""),
		((event->keystate & GKEYSTATE_NUMLOCK) ? "NUMLOCK " : ""),
		((event->keystate & GKEYSTATE_SCROLLLOCK) ? "SCROLLLOCK " : "")
		);
	if (event->bytecount) {
		printf(" Keys:");
		for (i = 0; i < event->bytecount; i++)
			printf(" 0x%02X", (uint8_t)event->c[i]);
		printf(" [");
		for (i = 0; i < event->bytecount; i++)
			printf("%c", event->c[i] >= ' ' && event->c[i] <= '~' ? event->c[i] : ' ');
		printf("]");
	}
	printf("\n");
}
#endif

StationEvent_t uiGetStationEvent(void)
{
	uint32_t devicenum;
	uint32_t time;
	GEvent *			guiEvent;

	guiEvent = geventEventWait(&_gl, 100);

	if (!guiEvent) {
		uint32_t time = xTaskGetTickCount();
		// Check for long events
		for (int i = SD_ROTSW; i < SD_TOOL1 + 1; i++)
		{
			if ((_uidDevices[i].value) && (!(_uidDevices[i].flags & UIDEVFLAG_USED))) {
				if (time - _uidDevices[i].value > UIDEV_LONGPRESS) {
					_uidDevices[i].flags |= UIDEVFLAG_USED;
					return DEVBUTTON_LONG_OFFS + i;
				}
			}
		}

		return SE_NONE;
	}

	switch(guiEvent->type) {

		// Rotary encoder
		#if GINPUT_NEED_DIAL
			case GEVENT_DIAL:;
				GEventDial* eventdial = (GEventDial*)guiEvent;
				int32_t diff = (int16_t)(_uidDevices[SD_ROT].value) - (int16_t)eventdial->value;
				int32_t chng = abs(diff) / 2;
				_uidDevices[SD_ROT].value = (uint32_t)eventdial->value;
				if (diff < 0) {
					if (chng == 1) return SE_ROT_INC;
					if (chng == 2) return SE_ROT_IINC;
					return SE_ROT_IIINC;
				}
				if (diff > 0) {
					if (chng == 1) return SE_ROT_DEC;
					if (chng == 2) return SE_ROT_DDEC;
					return SE_ROT_DDDEC;
				}
				break;
		#endif

		// Pushbuttons
		#if GINPUT_NEED_TOGGLE
			case GEVENT_TOGGLE:;
				GEventToggle* eventtoggle = (GEventToggle*)guiEvent;
				time = xTaskGetTickCount();
				devicenum = SD_ROTSW;

				if (eventtoggle->on) {
					switch (eventtoggle->instance) {
						#ifdef GINPUT_TOGGLE_SW1_CANCEL_TEMP1
							case GINPUT_TOGGLE_SW5_TOOL1: devicenum++; // Tool 1
							case GINPUT_TOGGLE_SW4_TOOL2: devicenum++; // Tool 2
							case GINPUT_TOGGLE_SW3_OK_TEMP3: devicenum++; // Preset 3
							case GINPUT_TOGGLE_SW2_MOD_TEMP2: devicenum++; // Preset 2
							case GINPUT_TOGGLE_SW1_CANCEL_TEMP1: devicenum++; // Preset 1
							case GINPUT_TOGGLE_CENTER_OK:; // Enter
								_uidDevices[devicenum].flags = 0;
								_uidDevices[devicenum].value = time;
								return devicenum + DEVBUTTON_DOWN_OFFS;
						#endif

						default:;
					}
				}
				else
				{
					switch (eventtoggle->instance) {
						#ifdef GINPUT_TOGGLE_SW1_CANCEL_TEMP1
							case GINPUT_TOGGLE_SW5_TOOL1: devicenum++; // Tool 1
							case GINPUT_TOGGLE_SW4_TOOL2: devicenum++; // Tool 2
							case GINPUT_TOGGLE_SW3_OK_TEMP3: devicenum++; // Preset 3
							case GINPUT_TOGGLE_SW2_MOD_TEMP2: devicenum++; // Preset 2
							case GINPUT_TOGGLE_SW1_CANCEL_TEMP1: devicenum++; // Preset 1
							case GINPUT_TOGGLE_CENTER_OK:; // Enter
								_uidDevices[devicenum].flags = 0;
								_uidDevices[devicenum].value = 0;
								return devicenum + DEVBUTTON_UP_OFFS;
						#endif

						default:;
					}
				}
				break;
		#endif

		// Windows keyboard
		#if GINPUT_NEED_KEYBOARD
			case GEVENT_KEYBOARD:;
				GEventKeyboard* eventkey = (GEventKeyboard*)guiEvent;
				DebugKeys(eventkey);
				if (eventkey->bytecount == 0) break;
				uint8_t key = (uint8_t)eventkey->c[0];
				time = xTaskGetTickCount();
				devicenum = SD_ROTSW;

				if (eventkey->keystate & GKEYSTATE_KEYUP) {
					switch (key) {
						case '5': devicenum++; // Tool 1
						case '4': devicenum++; // Tool 2
						case '3': devicenum++; // Preset 3
						case '2': devicenum++; // Preset 2
						case '1': devicenum++; // Preset 1
						case 0x0D:; // Enter
							_uidDevices[devicenum].flags = 0;
							_uidDevices[devicenum].value = 0;
							return devicenum + DEVBUTTON_UP_OFFS;
						default:;
					}
				}
				else if (eventkey->keystate & GKEYSTATE_REPEAT) {
					switch (key) {
						case GKEY_UP: return SE_ROT_INC;
						case GKEY_DOWN: return SE_ROT_DEC;
						default:;
					}
				}
				else
				{
					switch (key) {
						case GKEY_UP: return SE_ROT_INC;
						case GKEY_DOWN: return SE_ROT_DEC;
						case '5': devicenum++; // Tool 1
						case '4': devicenum++; // Tool 2
						case '3': devicenum++; // Preset 3
						case '2': devicenum++; // Preset 2
						case '1': devicenum++; // Preset 1
						case 0x0D:; // Enter
							_uidDevices[devicenum].flags = 0;
							_uidDevices[devicenum].value = time;
							return devicenum + DEVBUTTON_DOWN_OFFS;
						default:;
					}
				}

				break;
		#endif
	}

	return SE_NONE;
}

void uiStationEventInit(void)
{
	// We want to listen for widget events
	geventListenerInit(&_gl);
	gwinAttachListener(&_gl);
	#if GINPUT_NEED_KEYBOARD
		geventAttachSource(&_gl, ginputGetKeyboard(GKEYBOARD_ALL_INSTANCES), GLISTEN_KEYUP);
	#endif
	#if GINPUT_NEED_DIAL
		geventAttachSource(&_gl, ginputGetDial(0), 0);
		ginputSetDialRange(0, 65535);
		ginputSetDialSensitivity(0, 2);
		geventAttachSource(&_gl, ginputGetToggle(GINPUT_TOGGLE_SW1_CANCEL_TEMP1), GLISTEN_TOGGLE_ON | GLISTEN_TOGGLE_OFF);
		geventAttachSource(&_gl, ginputGetToggle(GINPUT_TOGGLE_SW2_MOD_TEMP2), GLISTEN_TOGGLE_ON | GLISTEN_TOGGLE_OFF);
		geventAttachSource(&_gl, ginputGetToggle(GINPUT_TOGGLE_SW3_OK_TEMP3), GLISTEN_TOGGLE_ON | GLISTEN_TOGGLE_OFF);
		geventAttachSource(&_gl, ginputGetToggle(GINPUT_TOGGLE_SW4_TOOL2), GLISTEN_TOGGLE_ON | GLISTEN_TOGGLE_OFF);
		geventAttachSource(&_gl, ginputGetToggle(GINPUT_TOGGLE_SW5_TOOL1), GLISTEN_TOGGLE_ON | GLISTEN_TOGGLE_OFF);
		geventAttachSource(&_gl, ginputGetToggle(GINPUT_TOGGLE_CENTER_OK), GLISTEN_TOGGLE_ON | GLISTEN_TOGGLE_OFF);
	#endif
}


