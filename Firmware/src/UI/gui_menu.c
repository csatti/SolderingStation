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

// Menu
GHandle ghPageContainerMenuScreen;
GHandle ghMenuItem[5];
GHandle ghMenuScrollBar;

bool_t guiCreatePageMenuScreen(void)
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
	ghPageContainerMenuScreen = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);
	((GWidgetObject*)ghPageContainerMenuScreen)->g.flags &= ~GWIN_CONTAINER_BORDER;

	// ghMenuItem[5]
	for (int i = 0; i < 5; i++) {
		wi.g.show = TRUE;
		wi.g.x = 1;
		wi.g.y = i * 29;
		wi.g.parent = ghPageContainerMenuScreen;
		wi.text = "";
		wi.customDraw = 0;
		wi.customParam = 0;
		wi.customStyle = 0;
		ghMenuItem[i] = menuitemCreate(0, &wi, &IMG_MENUSELECT_PNG);
		gwinSetFont(ghMenuItem[i], gstudioGetFont(General));
		gwinSetColor(ghMenuItem[i], GFX_GRAY);
		gwinSetBgColor(ghMenuItem[i], GFX_BLACK);
	}

	// ghMenuScrollBar
	wi.g.show = TRUE;
	wi.g.x = 298;
	wi.g.y = 0;
	wi.g.parent = ghPageContainerMenuScreen;
	wi.text = "";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghMenuScrollBar = menuscrollbarCreate(0, &wi, &IMG_MENUSELECT_BAR_PNG);
	gwinSetColor(ghMenuScrollBar, GFX_GRAY);
	gwinSetBgColor(ghMenuScrollBar, GFX_BLACK);

	return TRUE;
}

static const char* _getSystemMenuText(uint16_t pos)
{
	switch (pos) {
	case 0: return GT(MENUITEM_IRONCALIBRATION);
	case 1: return GT(MENUITEM_LCDGAMMA);
	case 2: return GT(MENUITEM_SOUNDTEST);
	case 3: return GT(MENUITEM_FIRMWAREUPDATE);
	case 4: return GT(MENUITEM_RESTART);
	case 5: return GT(MENUITEM_REPORT);
	case 6: return GT(MENUITEM_SCOPE);
	case 7: return GT(MENUITEM_DEFAULTSETTINGS);
	}

	return GT(TEXT_EMPTY);
}

// System menu
Menu_t guiMenuSystemMenuScreen(void)
{
	uint16_t selected = 0, topPos = 0;
	ConfigOnline_t* ConfigOnline;
	const uint16_t total = 8;

	guiSetFooterButtons(gstudioGetFont(General), GT(MENUCONTROL_BACK),
			GT(TEXT_EMPTY), GT(MENUCONTROL_SELECT));

	for (int i = 0; i < 5; i++) {
		guiSimpleSetText(ghMenuItem[i], _getSystemMenuText(i));
		menuitemSetImage(ghMenuItem[i], &IMG_MENUSELECT_SHORT_PNG);
	}

	menuscrollbarSetup(ghMenuScrollBar, total, 5);
	gwinSetVisible(ghMenuScrollBar, TRUE);
	guiShowPage(MENUSCREEN);

	while (1) {
		menuscrollbarSetTopItem(ghMenuScrollBar, topPos);
		for (int i = 0; i < 5; i++) {
			menuitemSetSelected(ghMenuItem[i], (selected - topPos == i));
		}

		// Poll event
		StationEvent_t event = guiGetStationEvent();

		if (event != SE_NONE) {
			guiEventCheckToolControl(event);

			switch (event) {
			case SE_ROT_IIINC:
			case SE_ROT_IINC:
			case SE_ROT_INC:
				guiIncreaseValue(&selected, total);
				break;
			case SE_ROT_DDDEC:
			case SE_ROT_DDEC:
			case SE_ROT_DEC:
				guiDecreaseValue(&selected, total);
				break;
			case SE_PRESET1_UP:
				return MENU_SCREEN;
			case SE_PRESET3_UP:
			case SE_ROTSW_UP:
				switch (selected) {
				case 0:
					return IRON_CALIBRATION;
				case 1:
					return GAMMA_CALIBRATION;
				case 2:
					jokePlayImperialMarch();
					break;
				case 3:
					ConfigOnline = configGetConfigOnline();
					ConfigOnline->loadBootloader = 1;
					configReturnConfigOnline();
					guiShowPage(FIRMWARESCREEN);
					vTaskDelay(FIRMWARE_WAIT);
					boardReset();
					return MENU_SCREEN;
				case 4:
					boardReset();
					return MENU_SCREEN;
				case 5:
					if (telemetryIsEnabled()) guiShowNotify(WARNING_REPORTFAILED);
					else
						reportSend();
					break;
				case 6:
					return SCOPE_SCREEN;
				case 7:
					configReset();
					guiShowNotify(INFO_SETTINGSRESET);
					break;
				default:
					;
				}
			default:
				;
			}

			if (topPos > selected) topPos = selected;
			if (topPos + 4 < selected) topPos = selected - 4;

			for (int i = 0; i < 5; i++) {
				gwinSetText(ghMenuItem[i], _getSystemMenuText(topPos + i), FALSE);
			}
		}

		// Check for notifications
		uint32_t msg = notifyReceive();
		if (msg)
			guiShowNotify(msg);

	}

	return MENU_SCREEN;
}

// Set texts for menu items
static void _menuSetTexts(void)
{
	ConfigOnline_t* ConfigOnline;

	gwinSetText(ghPreset1button, GT(MENUCONTROL_BACK), FALSE);
	gwinSetText(ghPreset2button, GT(TEXT_EMPTY), FALSE);
	gwinSetText(ghPreset3button, GT(MENUCONTROL_SELECT), FALSE);
	gwinSetText(ghMenuItem[0], GT(MENUITEM_SETTINGS), FALSE);
	gwinSetText(ghMenuItem[1], GT(MENUITEM_SYSTEM), FALSE);
	gwinSetText(ghMenuItem[3], GT(MENUITEM_LANGUAGE), FALSE);
	gwinSetText(ghMenuItem[4], GT(TEXT_EMPTY), FALSE);

	ConfigOnline = configGetConfigOnline();
	if (ConfigOnline->sound) {
		gwinSetText(ghMenuItem[2], GT(MENUITEM_SOUND_ON), FALSE);
	} else {
		gwinSetText(ghMenuItem[2], GT(MENUITEM_SOUND_OFF), FALSE);
	}

	for (int i = 0; i < 5; i++) {
		menuitemSetImage(ghMenuItem[i], &IMG_MENUSELECT_PNG);
	}

	configReturnConfigOnline();
}

// Main menu
Menu_t guiMenuMainMenuScreen(void)
{
	uint16_t selected = 0;
	ConfigOnline_t* ConfigOnline;

	gwinSetFont(ghPreset1button, gstudioGetFont(General));
	gwinSetFont(ghPreset2button, gstudioGetFont(General));
	gwinSetFont(ghPreset3button, gstudioGetFont(General));
	_menuSetTexts();
	gwinSetVisible(ghMenuScrollBar, FALSE);

	guiShowPage(MENUSCREEN);

	while (1) {

		for (int i = 0; i < 5; i++) {
			menuitemSetSelected(ghMenuItem[i], (selected == i));
		}

		// Poll event
		StationEvent_t event = guiGetStationEvent();

		if (event != SE_NONE) {
			guiEventCheckToolControl(event);

			switch (event) {
			case SE_ROT_IIINC:
			case SE_ROT_IINC:
			case SE_ROT_INC:
				guiIncreaseValue(&selected, 4);
				break;
			case SE_ROT_DDDEC:
			case SE_ROT_DDEC:
			case SE_ROT_DEC:
				guiDecreaseValue(&selected, 4);
				break;
			case SE_PRESET1_UP:
				return MAIN_SCREEN;
			case SE_PRESET3_UP:
			case SE_ROTSW_UP:
				switch (selected) {
				case 0:
					return SETTINGS_SCREEN;
				case 1:
					return SYSTEMMENU_SCREEN;
				case 2:
					ConfigOnline = configGetConfigOnline();
					ConfigOnline->sound = !ConfigOnline->sound;
					configReturnConfigOnline();
					_menuSetTexts();
					break;
				case 3:
					;
					uint8_t lang = locGetLanguage();
					lang++;
					locSetLanguage(lang);
					ConfigOnline = configGetConfigOnline();
					ConfigOnline->language = locGetLanguage();
					configReturnConfigOnline();
					gwinRedraw(ghPageContainerHeader);
					gwinRedraw(ghPageContainerFooter);
					_menuSetTexts();
					break;
				default:
					;
				}
			default:
				;
			}

		}

		// Check for notifications
		uint32_t msg = notifyReceive();
		if (msg)
			guiShowNotify(msg);

	}

	return MAIN_SCREEN;
}

