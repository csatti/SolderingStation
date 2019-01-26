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

#ifndef UI_GUI_PRIV_H_
#define UI_GUI_PRIV_H_

#include "gui.h"
#include "../../../src/gwin/gwin_class.h"
// Resources
#include "ILI9341.h"
#include "images.h"
#include "resources_manager.h"
#include "widgetstyles.h"
// Widgets
#include "toolstatus.h"
#include "statusbar.h"
#include "simpleimage.h"
#include "gammarectangle.h"
#include "menuitem.h"
#include "menuscrollbar.h"
#include "wrapper.h"
// Helpers
#include "userinput.h"
#include "sound.h"
#include "joke.h"
#include "notify.h"
#include "message.h"
#include "reporting.h"
#include "utility.h"
#include "version.h"
#include "doublebuffer.h"
// Standard
#include <stdlib.h>
#include <string.h>
// System
#include "analog.h"
#include "control.h"
#include "process.h"

#define mainQUEUE_UGFX_TASK_PRIORITY        			( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_HEADER_UPDATE_TASK_PRIORITY        	( tskIDLE_PRIORITY + 1 )

#define HEADER_UPDATE_CYCLE								pdMS_TO_TICKS(75)
#define HEADER_UPDATE_START_DELAY						pdMS_TO_TICKS(400)
#define SPLASH_WAIT										pdMS_TO_TICKS(2500)
#define INIT_WAIT										pdMS_TO_TICKS(200)

typedef enum guiPage {
	MENUSCREEN,
	MAINSCREEN,
	GAMMASCREEN,
	SETTINGSSCREEN,
	NOTIFYSCREEN,
	STARTSCREEN,
	FIRMWARESCREEN,
	SCOPESCREEN,
} guiPage;

typedef enum {
	MAIN_SCREEN,
	MENU_SCREEN,
	SYSTEMMENU_SCREEN,
	SHOW_NOTIFICATION,
	IRON_CALIBRATION,
	GAMMA_CALIBRATION,
	SETTINGS_SCREEN,
	SCOPE_SCREEN,
} Menu_t;

// Widget handles
extern GHandle ghPageContainerHeader;
extern GHandle ghPageContainerFooter;
extern GHandle ghPreset1button;
extern GHandle ghPreset3button;
extern GHandle ghPreset2button;
extern GHandle ghToolLeft;
extern GHandle ghToolRight;
extern GHandle ghStatusBar;

// Main screen
extern GHandle ghPageContainerMainScreen;
extern GHandle ghUnitLabel;
extern GHandle ghSetpointLabel1;
extern GHandle ghSetpointLabel2;
extern GHandle ghSetpointLabel3;
extern GHandle ghProgressbar;
// Diagnostics
extern GHandle ghSetpointLabel;
extern GHandle ghRailLabel;
extern GHandle ghVoltageValLabel;
extern GHandle ghCurrentValLabel;
extern GHandle ghPowerValLabel;
extern GHandle ghThrottleNameLabel;
extern GHandle ghThrottle1ValLabel;
extern GHandle ghThrottle2ValLabel;
extern GHandle ghTipRawNameLabel;
extern GHandle ghTip1RawValLabel;
extern GHandle ghTip2RawValLabel;
extern GHandle ghRefRawNameLabel;
extern GHandle ghRefRawValLabel;
extern GHandle ghSupplyNameLabel;
extern GHandle ghSupplyValLabel;
extern GHandle ghTipIntNameLabel;
extern GHandle ghTip1IntValLabel;
extern GHandle ghTip2IntValLabel;

// Gamma calibration
extern GHandle ghPageContainerGammaCalibrationScreen;
extern GHandle ghGammaValue;
extern GHandle ghPosGamma[ILI9341_GAMMACURVEPOINTS];
extern GHandle ghNegGamma[ILI9341_GAMMACURVEPOINTS];

// Menu screens
extern GHandle ghPageContainerMenuScreen;
extern GHandle ghMenuItem[5];
extern GHandle ghMenuScrollBar;

// Parameter IO
extern GHandle ghPageContainerParamIOScreen;
extern GHandle ghParamName;
extern GHandle ghParamValue;
extern GHandle ghParamDesc;
extern GHandle ghParamScrollBar;

// Alarm and dialog
extern GHandle ghPageContainerAlarmScreen;
extern GHandle ghNotifyType;
extern GHandle ghNotifyText;
extern GHandle ghNotifyDesc;

// Scope view
extern GHandle ghPageContainerScopeScreen;
extern GHandle ghScopeWrapper;

// Splash
extern GHandle ghPageContainerStartScreen;
extern GHandle ghPageContainerUpdateScreen;

// Custom rendering functions
void gwinLabelDrawJustifiedLeftPM(GWidgetObject *gw, void *param);
void gwinLabelDrawJustifiedRightPM(GWidgetObject *gw, void *param);
void gwinLabelDrawJustifiedCenterPM(GWidgetObject *gw, void *param);
void gwinNoDraw(GWidgetObject *gw, void *param);
void gwinProgressbarDrawPM(GWidgetObject *gw, void *param);
void gwinProgressbarDrawPowerBarPM(GWidgetObject *gw, void *param);
void scopeDrawPM(GWidgetObject* gw, void* param);
void scopeLegendDrawPM(GWidgetObject* gw, void* param);

// Helper functions
uint16_t guiInternalToDisplay(uint16_t intTemp);
void guiDecreaseValue(uint16_t* val, uint16_t limit);
void guiIncreaseValue(uint16_t* val, uint16_t limit);
void guiSetFooterButtons(font_t font, const char* text1, const char* text2,
		const char* text3);
void guiSimpleSetText(GHandle gh, const char* text);
void guiEventCheckToolControl(StationEvent_t event);

// GUI
void guiShowPage(guiPage page);
guiPage guiGetPage(void);
void guiSetPage(guiPage page);

// Main
bool_t guiCreatePageMainScreen(void);
Menu_t guiMenuMainScreen(void);

// Menu
bool_t guiCreatePageMenuScreen(void);
Menu_t guiMenuSystemMenuScreen(void);
Menu_t guiMenuMainMenuScreen(void);

// Settings
bool_t guiCreatePageParamIOScreen(void);
Menu_t guiMenuSettings(void);

// Scope
bool_t guiCreatePageScopeScreen(void);
Menu_t guiMenuScope(void);

// Calibrations
bool_t guiCreatePageGammaCalibrationScreen(void);
Menu_t guiMenuToolCalibration(void);
Menu_t guiMenuGammaCalibration(void);

// Notification
bool_t guiCreatePageNotifyScreen(void);
void guiShowNotify(uint32_t message);
uint8_t guiShowDialog(int header, int message);

// Header
bool_t guiCreateHeaderAndFooter(void);
void guiHeaderInit(void);

// Splash screens
bool_t guiCreateStartScreen(void);
bool_t guiCreateUpdateScreen(void);

// Screen saver
void guiScreenSaverInit(void);
void guiSetScreenSaver(uint16_t time);
StationEvent_t guiGetStationEvent(void);

#endif /* UI_GUI_PRIV_H_ */
