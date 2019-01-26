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

// Scope view
GHandle ghPageContainerScopeScreen;
GHandle ghScopeWrapper;
GHandle ghScopeLegendWrapper;

static TaskHandle_t _thScopeTask;

static enum scopeMode_t {
	SCOPE_CONTINUOUS,
	SCOPE_ONHEAT,
	// Always last
	SCOPE_LASTITEM
} _scopeMode;

bool_t guiCreatePageScopeScreen(void)
{
	GWidgetInit wi;

	gwinWidgetClearInit(&wi);

	// Page container
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 60;
	wi.g.width = 320;
	wi.g.height = 145;
	wi.g.parent = 0;
	wi.text = "Container";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghPageContainerScopeScreen = gwinContainerCreate(0, &wi, 0);

	// ghScopeWrapper
	wi.g.show = TRUE;
	wi.g.x = 319 - SCOPE_LEN;
	wi.g.y = 10;
	wi.g.width = SCOPE_LEN;
	wi.g.height = 128;
	wi.g.parent = ghPageContainerScopeScreen;
	wi.text = "";
	wi.customDraw = scopeDrawPM;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghScopeWrapper = wrapperCreate(0, &wi);

	// ghScopeLegendWrapper
	wi.g.show = TRUE;
	wi.g.x = 0;
	wi.g.y = 10;
	wi.g.width = 319 - SCOPE_LEN;
	wi.g.height = 128;
	wi.g.parent = ghPageContainerScopeScreen;
	wi.text = "";
	wi.customDraw = scopeLegendDrawPM;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghScopeLegendWrapper = wrapperCreate(0, &wi);
	gwinSetFont(ghScopeLegendWrapper, gstudioGetFont(GeneralSmall));

	return TRUE;
}

// This task updates the scope state
static void _scopeUpdateTask(void *pvParameters)
{
	uint32_t notifyVal;

	processSetScopeTask(_thScopeTask);
	processSetScopeDataIsUsed();

	while (1) {

		notifyVal = waitForAnyEvent(
				NOTIFY_NEWDATATRIGGER | NOTIFY_HEATTRIGGER | NOTIFY_EXITSCOPE, portMAX_DELAY);

		if (notifyVal & NOTIFY_EXITSCOPE) break;

		if ((notifyVal & NOTIFY_NEWDATATRIGGER) && (_scopeMode == SCOPE_CONTINUOUS)) {
			gwinRedraw(ghScopeWrapper);
		} else if ((notifyVal & NOTIFY_HEATTRIGGER) && (_scopeMode == SCOPE_ONHEAT)) {
			gwinRedraw(ghScopeWrapper);
		} else {
			processSetScopeDataIsUsed();
		}
	}

	vTaskDelete(NULL);

	while (1) { vTaskDelay(portMAX_DELAY); };
}

Menu_t guiMenuScope(void)
{
	guiSetFooterButtons(gstudioGetFont(General), GT(MENUCONTROL_BACK),
			GT(MENUCONTROL_MODE), GT(TEXT_EMPTY));
	guiShowPage(SCOPESCREEN);

	_scopeMode = SCOPE_CONTINUOUS;

	xTaskCreate(_scopeUpdateTask, TXT("Scope"), 500,
		NULL,
		mainQUEUE_UGFX_TASK_PRIORITY,
		&_thScopeTask);

	while (1) {

		// Poll event
		StationEvent_t event = guiGetStationEvent();

		if (event != SE_NONE) {
			guiEventCheckToolControl(event);

			switch (event) {
			case SE_PRESET2_UP:
				_scopeMode++;
				if (_scopeMode == SCOPE_LASTITEM) _scopeMode = SCOPE_CONTINUOUS;
				break;
			case SE_PRESET1_UP:
				processSetScopeTask(NULL);
				xTaskNotify( _thScopeTask, NOTIFY_EXITSCOPE, eSetBits);
				return SYSTEMMENU_SCREEN;
			default:
				;
			}
		}

		//gwinRedraw(ghScopeWrapper);

		// Check for notifications
		uint32_t msg = notifyReceive();
		if (msg)
			guiShowNotify(msg);

	}

	// Should never get here
	return SYSTEMMENU_SCREEN;
}

