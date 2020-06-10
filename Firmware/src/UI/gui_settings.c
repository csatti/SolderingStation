/* Copyright (C) 2018 - 2019, Attila Kov�cs
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

#define MAX_PARAM_NUM			11
#define MAX_PARAM_LENGTH		15

// Parameter I/O
GHandle ghPageContainerParamIOScreen;
GHandle ghParamName;
GHandle ghParamValue;
GHandle ghParamDesc;
GHandle ghParamScrollBar;

// Initialize screen
bool_t guiCreatePageParamIOScreen(void)
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
	ghPageContainerParamIOScreen = gwinContainerCreate(0, &wi, 0);

	// ghParamName
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 10;
	wi.g.width = 294;
	wi.g.height = 26;
	wi.g.parent = ghPageContainerParamIOScreen;
	wi.text = "";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &ParameterNameStyle;
	ghParamName = gwinLabelCreate(0, &wi);
	gwinSetFont(ghParamName, gstudioGetFont(General));

	// ghParamValue
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 45;
	wi.g.width = 294;
	wi.g.height = 34;
	wi.g.parent = ghPageContainerParamIOScreen;
	wi.text = "";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &ParameterStyle;
	ghParamValue = gwinLabelCreate(0, &wi);
	gwinSetFont(ghParamValue, gstudioGetFont(GeneralLarge));

	// ghParamDesc
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 94;
	wi.g.width = 294;
	wi.g.height = 40;
	wi.g.parent = ghPageContainerParamIOScreen;
	wi.text = "";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &ParameterDescStyle;
	ghParamDesc = gwinLabelCreate(0, &wi);
	gwinSetFont(ghParamDesc, gstudioGetFont(GeneralSmall));

	// ghParamScrollBar
	wi.g.show = TRUE;
	wi.g.x = 298;
	wi.g.y = 0;
	wi.g.parent = ghPageContainerParamIOScreen;
	wi.text = "";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghParamScrollBar = menuscrollbarCreate(0, &wi, &IMG_MENUSELECT_BAR_GREY_PNG);
	gwinSetColor(ghParamScrollBar, GFX_GRAY);
	gwinSetBgColor(ghParamScrollBar, GFX_BLACK);

	return TRUE;
}

static void _setSettingText(int textid)
{
	gwinSetText(ghParamName, GT(textid), FALSE);
	gwinSetText(ghParamDesc, GT(textid + 1), FALSE);
}

static uint16_t _fillValueTable(char values[][MAX_PARAM_LENGTH],
		const uint16_t *table, uint16_t *valNum, uint16_t currentVal, int unit, bool_t offset)
{
	uint16_t index = 0;
	const char* unitText;

	if ((unit != UNITTEXT_CELSIUSPERMINUTE) && (unit != 0)) {
		unitText = GT(unit);
	} else {
		ConfigOnline_t* ConfigOnline = configGetConfigOnlineNoWrite();
		if (unit == 0) {
			unitText = (
					ConfigOnline->tempUnitC ?
							GT(UNITTEXT_CELSIUS) : GT(UNITTEXT_FAHRENHEIT));
		} else {
			unitText = (
					ConfigOnline->tempUnitC ?
							GT(UNITTEXT_CELSIUSPERMINUTE) : GT(UNITTEXT_FAHRENHEITPERMINUTE));
			unit = 0;
		}
		if (!ConfigOnline->tempUnitC) offset = FALSE;
	}

	if (*valNum > MAX_PARAM_NUM)
		*valNum = MAX_PARAM_NUM;

	for (int i = 0; i < *valNum; i++) {
		if (currentVal == *table) {
			index = i;
		}

		if (unit) {
			ltoa(*table++, values[i], 10);
		} else {
			uint16_t val = *table++;
			if (offset) val += 320;
			ltoa(guiInternalToDisplay(val), values[i], 10);
		}

		strcat(values[i], " ");
		strcat(values[i], unitText);
	}

	return index;
}

// Settings menu
Menu_t guiMenuSettings(void)
{
	const uint8_t settingsNo = LAST_CONFIG;
	uint16_t selected = 0;
	bool_t editing = FALSE;
	bool_t newParam = TRUE;
	ConfigOffline_t* ConfigOffline;
	ConfigOnline_t* ConfigOnline;
	char values[MAX_PARAM_NUM][MAX_PARAM_LENGTH];
	uint16_t valNum, valIndex, unit;

	guiSetFooterButtons(gstudioGetFont(General), GT(MENUCONTROL_BACK),
			GT(TEXT_EMPTY), GT(MENUCONTROL_EDIT));
	_setSettingText(SETTINGTEXT_UNITOFTEMPERATURE);
	menuscrollbarSetup(ghParamScrollBar, settingsNo, 1);
	gwinSetVisible(ghParamScrollBar, TRUE);
	guiShowPage(SETTINGSSCREEN);

	while (1) {

		// Select new parameter to show, fill up valid parameter table
		if (newParam) {
			ConfigOffline = configGetConfigOffline();
			valIndex = 0;
			switch (selected) {
			case TEMP_UNIT:
				valNum = 2;
				strcpy(values[0], GT(UNITTEXT_FAHRENHEIT));
				strcpy(values[1], GT(UNITTEXT_CELSIUS));
				ConfigOnline = configGetConfigOnlineNoWrite();
				valIndex = ConfigOnline->tempUnitC;
				_setSettingText(SETTINGTEXT_UNITOFTEMPERATURE);
				break;
			case DIAGNOSTICS:
			case TELEMETRY:
				valNum = 2;
				strcpy(values[0], GT(TEXT_DISABLE));
				strcpy(values[1], GT(TEXT_ENABLE));
				ConfigOnline = configGetConfigOnlineNoWrite();
				if (selected == TELEMETRY) {
					valIndex = ConfigOnline->telemetry;
					_setSettingText(SETTINGTEXT_TELEMETRY);
				} else {
					valIndex = ConfigOnline->diagnostics;
					_setSettingText(SETTINGTEXT_DIAGNOSTICS);
				}

				break;
			case SLEEP_TEMPERATURE:
				valNum = SleepTempTableSize;
				valIndex = _fillValueTable(values, SleepTempTable, &valNum,
						ConfigOffline->sleepTemp, 0, FALSE);
				_setSettingText(SETTINGTEXT_SLEEPTEMP);
				break;
			case DELAY_SCREENOFF:
				valNum = ScreenOffTableSize;
				valIndex = _fillValueTable(values, ScreenOffTable, &valNum,
						ConfigOffline->screenOff, UNITTEXT_MINUTE, FALSE);
				_setSettingText(SETTINGTEXT_SCREENOFF);
				break;
			case DELAY_SWITCHOFF:
				valNum = DelayOffTableSize;
				valIndex = _fillValueTable(values, DelayOffTable, &valNum,
						ConfigOffline->delayOff, UNITTEXT_MINUTE, FALSE);
				_setSettingText(SETTINGTEXT_DELAYSWITCHOFF);
				break;
			case TEMPERATURE_DROP:
				valNum = TempDropTableSize;
				valIndex = _fillValueTable(values, TempDropTable, &valNum,
						ConfigOffline->tempDrop, UNITTEXT_CELSIUSPERMINUTE, TRUE);
				_setSettingText(SETTINGTEXT_TEMPERATUREDROP);
				break;
			case TEMPERATURE_STEP:
				ConfigOnline = configGetConfigOnlineNoWrite();
				unit = (ConfigOnline->tempUnitC ? UNITTEXT_CELSIUS : UNITTEXT_FAHRENHEIT);
				valNum = TempStepTableSize;
				valIndex = _fillValueTable(values, TempStepTable, &valNum,
						ConfigOffline->tempStep, unit, FALSE);
				_setSettingText(SETTINGTEXT_TEMPERATURESTEP);
				break;
			case MAX_TEMP:
				valNum = MaxTempTableSize;
				valIndex = _fillValueTable(values, MaxTempTable, &valNum,
						ConfigOffline->maxTemp, 0, FALSE);
				_setSettingText(SETTINGTEXT_MAXTEMPERATURE);
				break;
			case POWER_LIMIT:
				valNum = PowerLimitTableSize;
				valIndex = _fillValueTable(values, PowerLimitTable, &valNum,
						ConfigOffline->powerLimit, UNITTEXT_WATT, FALSE);
				_setSettingText(SETTINGTEXT_POWERLIMIT);
				break;
			}
			configReturnConfigOffline();
			newParam = FALSE;
		}

		gwinSetEnabled(ghParamValue, editing);
		gwinSetText(ghParamValue, values[valIndex], FALSE);
		menuscrollbarSetTopItem(ghParamScrollBar, selected);

		// Poll event
		StationEvent_t event = guiGetStationEvent();

		if (event != SE_NONE) {
			guiEventCheckToolControl(event);

			switch (event) {
			case SE_ROT_IIINC:
			case SE_ROT_IINC:
			case SE_ROT_INC:
				if (editing) {
					guiIncreaseValue(&valIndex, valNum);
				} else {
					guiIncreaseValue(&selected, settingsNo);
					newParam = TRUE;
				}
				break;
			case SE_ROT_DDDEC:
			case SE_ROT_DDEC:
			case SE_ROT_DEC:
				if (editing) {
					guiDecreaseValue(&valIndex, valNum);
				} else {
					guiDecreaseValue(&selected, settingsNo);
					newParam = TRUE;
				}
				break;
			case SE_PRESET1_UP:
				if (editing) {
					newParam = TRUE;
					editing = FALSE;
					guiSetFooterButtons(gstudioGetFont(General),
							GT(MENUCONTROL_BACK), GT(TEXT_EMPTY),
							GT(MENUCONTROL_EDIT));
				} else {
					ConfigOffline = configGetConfigOffline();
					configStoreConfigOffline();
					configReturnConfigOffline();
					return MENU_SCREEN;
				}
				break;
			case SE_PRESET3_UP:
			case SE_ROTSW_UP:
				if (editing) {
					ConfigOffline = configGetConfigOffline();
					ConfigOnline = configGetConfigOnline();
					switch (selected) {
					case TEMP_UNIT:
						ConfigOnline->tempUnitC = valIndex;
						break;
					case TELEMETRY:
						ConfigOnline->telemetry = valIndex;
						break;
					case DIAGNOSTICS:
						ConfigOnline->diagnostics = valIndex;
						break;
					case SLEEP_TEMPERATURE:
						ConfigOffline->sleepTemp = SleepTempTable[valIndex];
						break;
					case DELAY_SCREENOFF:
						ConfigOffline->screenOff = ScreenOffTable[valIndex];
						break;
					case DELAY_SWITCHOFF:
						ConfigOffline->delayOff = DelayOffTable[valIndex];
						break;
					case TEMPERATURE_DROP:
						ConfigOffline->tempDrop = TempDropTable[valIndex];
						break;
					case TEMPERATURE_STEP:
						ConfigOffline->tempStep = TempStepTable[valIndex];
						break;
					case MAX_TEMP:
						ConfigOffline->maxTemp = MaxTempTable[valIndex];
						break;
					case POWER_LIMIT:
						ConfigOffline->powerLimit = PowerLimitTable[valIndex];
						break;

					}
					configReturnConfigOnline();
					configReturnConfigOffline();
					editing = FALSE;
					guiSetFooterButtons(gstudioGetFont(General),
							GT(MENUCONTROL_BACK), GT(TEXT_EMPTY),
							GT(MENUCONTROL_EDIT));
				} else {
					editing = TRUE;
					guiSetFooterButtons(gstudioGetFont(General),
							GT(MENUCONTROL_CANCEL), GT(TEXT_EMPTY),
							GT(MENUCONTROL_ENTER));
				}

				break;

			default:
				;
			}
		}

		// Check for notifications
		uint32_t msg = notifyReceive();
		if (msg)
			guiShowNotify(msg);

	}

	// Should never get here
	return MENU_SCREEN;
}
