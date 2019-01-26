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

#include "gui.h"
#include "gui_priv.h"
#include "widgetstyles.h"
#include "resources_manager.h"
#include "control.h"
#include "process.h"

static guiPage currentPage = GAMMASCREEN;
static TimerHandle_t _thOnWarning;

// Warn user of on state
static void _warningTimerCallback(TimerHandle_t xTimer)
{
	jokePlayImperialMarch();
	xTimerChangePeriod(xTimer, (TIME_FORGOTTEN_REPEAT * 60 * 1000) / portTICK_PERIOD_MS, 0);
}

guiPage guiGetPage(void)
{
	return currentPage;
}

void guiSetPage(guiPage page)
{
	currentPage = page;
}

static void _simpleHide(GHandle gh)
{
	gh->flags &= ~(GWIN_FLG_VISIBLE|GWIN_FLG_SYSVISIBLE);
}

void guiShowPage(guiPage page)
{
	if (currentPage == page) return;

	switch (currentPage) {
		case STARTSCREEN:
			_simpleHide(ghPageContainerStartScreen);
			break;

		case MAINSCREEN:
			_simpleHide(ghPageContainerMainScreen);
			break;

		case MENUSCREEN:
			_simpleHide(ghPageContainerMenuScreen);
			break;

		case GAMMASCREEN:
			gwinHide(ghPageContainerGammaCalibrationScreen);
			break;

		case SETTINGSSCREEN:
			_simpleHide(ghPageContainerParamIOScreen);
			break;

		case NOTIFYSCREEN:
			_simpleHide(ghPageContainerAlarmScreen);
			break;

		case FIRMWARESCREEN:
			_simpleHide(ghPageContainerUpdateScreen);
			break;

		case SCOPESCREEN:
			_simpleHide(ghPageContainerScopeScreen);
			break;

		default:
			break;
	}

	vTaskDelay(pdMS_TO_TICKS(20));

	// Footer / header
	switch (page) {
		case STARTSCREEN:
		case FIRMWARESCREEN:
		case GAMMASCREEN:
			gwinHide(ghPageContainerHeader);
			gwinHide(ghPageContainerFooter);
			break;
		default:
			gwinShow(ghPageContainerHeader);
			gwinShow(ghPageContainerFooter);
			break;
	}

	// Show the selected page
	switch (page) {
		case STARTSCREEN:
			gwinShow(ghPageContainerStartScreen);
			break;

		case MAINSCREEN:
			gwinShow(ghPageContainerMainScreen);
			break;

		case MENUSCREEN:
			gwinShow(ghPageContainerMenuScreen);
			break;

		case GAMMASCREEN:
			gwinShow(ghPageContainerGammaCalibrationScreen);
			break;

		case SETTINGSSCREEN:
			gwinShow(ghPageContainerParamIOScreen);
			break;

		case NOTIFYSCREEN:
			gwinShow(ghPageContainerAlarmScreen);
			break;

		case FIRMWARESCREEN:
			gwinShow(ghPageContainerUpdateScreen);
			break;

		case SCOPESCREEN:
			gwinShow(ghPageContainerScopeScreen);
			break;

		default:
			break;
	}
	currentPage = page;

	vTaskDelay(pdMS_TO_TICKS(20));
}

static bool_t _guiSetup(void)
{
	// Initialize resources
	if (!guiResourcesManagerInit()) {
		return FALSE;
	}

	// Set GWIN default values
	gwinSetDefaultStyle(&DefaultStyle, FALSE);
	gwinSetDefaultFont(gstudioGetFont(General));

	// Create display pages
	guiCreateStartScreen();
	guiCreatePageMainScreen();
	guiCreatePageMenuScreen();
	guiCreateHeaderAndFooter();
	guiCreatePageGammaCalibrationScreen();
	guiCreatePageParamIOScreen();
	guiCreatePageNotifyScreen();
	guiCreateUpdateScreen();
	guiCreatePageScopeScreen();

	return TRUE;
}

// Menu task
static void _menuTask(void *pvParameters)
{

	Menu_t menu = MAIN_SCREEN;
	ConfigOnline_t* ConfigOnline;

	// Load configuration
	configLoad();

	debugTimeRef();
	debugPrint(TXT("Build version: "));
	debugPrintLnC(TEXT_VERSION, LCD_GREEN);

	ConfigOnline = configGetConfigOnline();
	locSetLanguage(ConfigOnline->language);
	configReturnConfigOnline();

	guiScreenSaverInit();
	doubleBufferInit();

	_thOnWarning = xTimerCreate( TXT("WarnTimer"), (TIME_FORGOTTEN * 60 * 1000) / portTICK_PERIOD_MS,
		pdTRUE, (void *) 0, _warningTimerCallback);
	xTimerStart(_thOnWarning, portMAX_DELAY);

	// Initialize the display
	gfxInit();

	gwinSetDefaultFont(gdispOpenFont("General"));   // Set default font
	gwinSetDefaultStyle(&BlackWidgetStyle, FALSE);   // Set default widget style
	_guiSetup();

	guiShowPage(STARTSCREEN);
	vTaskDelay(INIT_WAIT);
	gdispSetBacklight(100);
	vTaskDelay(SPLASH_WAIT);
	//guiShowPage(MAINSCREEN);
	//vTaskDelay(INIT_WAIT);

	guiHeaderInit();

	uiStationEventInit();

	while (1) {
		switch (menu) {
		case MAIN_SCREEN:
			menu = guiMenuMainScreen();
			break;
		case MENU_SCREEN:
			menu = guiMenuMainMenuScreen();
			break;
		case SYSTEMMENU_SCREEN:
			menu = guiMenuSystemMenuScreen();
			break;
		case GAMMA_CALIBRATION:
			menu = guiMenuGammaCalibration();
			break;
		case SETTINGS_SCREEN:
			menu = guiMenuSettings();
			break;
		case IRON_CALIBRATION:
			menu = guiMenuToolCalibration();
			break;
		case SCOPE_SCREEN:
			menu = guiMenuScope();
			break;
		default:
			menu = guiMenuMainScreen();
		}
	}

}

void guiInit()
{
	xTaskCreate(_menuTask, (char *) "Menu", 2000,
	NULL,
	mainQUEUE_UGFX_TASK_PRIORITY,
	NULL);
}

