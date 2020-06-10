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

// Main screen
GHandle ghPageContainerMainScreen;
GHandle ghUnitLabel;
GHandle ghSetpointLabel1;
GHandle ghSetpointLabel2;
GHandle ghSetpointLabel3;
GHandle ghProgressbar;
// Diagnostics
GHandle ghSetpointLabel;
GHandle ghRailLabel;
GHandle ghVoltageValLabel;
GHandle ghCurrentValLabel;
GHandle ghPowerValLabel;
GHandle ghThrottleNameLabel;
GHandle ghThrottle1ValLabel;
GHandle ghThrottle2ValLabel;
GHandle ghTipRawNameLabel;
GHandle ghTip1RawValLabel;
GHandle ghTip2RawValLabel;
GHandle ghTipIntNameLabel;
GHandle ghTip1IntValLabel;
GHandle ghTip2IntValLabel;
GHandle ghRefRawNameLabel;
GHandle ghRefRawValLabel;
GHandle ghSupplyNameLabel;
GHandle ghSupplyValLabel;

static char _szButton1[10];
static char _szButton2[10];
static char _szButton3[10];
static char _szMainTemp1[2] = { 0 };
static char _szMainTemp2[2] = { 0 };
static char _szMainTemp3[2] = { 0 };
static char _szSetPoint[10];

bool_t guiCreatePageMainScreen(void)
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
	ghPageContainerMainScreen = gwinContainerCreate(0, &wi, 0);
	((GWidgetObject*)ghPageContainerMainScreen)->g.flags &= ~GWIN_CONTAINER_BORDER;

	// ghProgressbar
	wi.g.show = TRUE;
	wi.g.x = 0;
	wi.g.y = 109;
	wi.g.width = 320;
	wi.g.height = 35;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = "Power";
	wi.customDraw = gwinProgressbarDrawPowerBarPM;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghProgressbar = gwinProgressbarCreate(0, &wi);
	gwinProgressbarSetRange(ghProgressbar, 0, 100);
	gwinProgressbarSetPosition(ghProgressbar, 33);

	// ghUnitLabel
	wi.g.show = TRUE;
	wi.g.x = 281;
	wi.g.y = 3;
	wi.g.width = 32;
	wi.g.height = 26;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_CELSIUS);
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &TemperatureSPStyle;
	ghUnitLabel = gwinLabelCreate(0, &wi);

	// ghSetpointLabel1
	wi.g.show = TRUE;
	wi.g.x = 37;
	wi.g.y = 0;
	wi.g.width = 82;
	wi.g.height = 106;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = "2";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &TemperatureSPStyle;
	ghSetpointLabel1 = gwinLabelCreate(0, &wi);
	gwinSetFont(ghSetpointLabel1, gstudioGetFont(LargeTemp));

	// ghSetpointLabel2
	wi.g.show = TRUE;
	wi.g.x = 119;
	wi.g.y = 0;
	wi.g.width = 82;
	wi.g.height = 106;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = "5";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &TemperatureSPStyle;
	ghSetpointLabel2 = gwinLabelCreate(0, &wi);
	gwinSetFont(ghSetpointLabel2, gstudioGetFont(LargeTemp));

	// ghSetpointLabel3
	wi.g.show = TRUE;
	wi.g.x = 201;
	wi.g.y = 0;
	wi.g.width = 82;
	wi.g.height = 106;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = "0";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &TemperatureSPStyle;
	ghSetpointLabel3 = gwinLabelCreate(0, &wi);
	gwinSetFont(ghSetpointLabel3, gstudioGetFont(LargeTemp));

	// Diagnostic values

	// ghSetpointLabel
	wi.g.show = TRUE;
	wi.g.x = 100;
	wi.g.y = 0;
	wi.g.width = 140;
	wi.g.height = 32;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = "0";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &TemperatureSPStyle;
	ghSetpointLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghSetpointLabel, gstudioGetFont(GeneralLarge));

	// ghRailLabel
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 32;
	wi.g.width = 90;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(TEXT_24VACRAIL);
	wi.customDraw = gwinLabelDrawJustifiedLeftPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghRailLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghRailLabel, gstudioGetFont(GeneralSmall));

	// ghVoltageValLabel
	wi.g.show = TRUE;
	wi.g.x = 100;
	wi.g.y = 32;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_VOLT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghVoltageValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghVoltageValLabel, gstudioGetFont(GeneralSmall));

	// ghCurrentValLabel
	wi.g.show = TRUE;
	wi.g.x = 170;
	wi.g.y = 32;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_AMPER);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghCurrentValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghCurrentValLabel, gstudioGetFont(GeneralSmall));

	// ghPowerValLabel
	wi.g.show = TRUE;
	wi.g.x = 240;
	wi.g.y = 32;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_WATT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghPowerValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghPowerValLabel, gstudioGetFont(GeneralSmall));

	// ghThrottleNameLabel
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 50;
	wi.g.width = 160;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(TEXT_THROTTLE);
	wi.customDraw = gwinLabelDrawJustifiedLeftPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghThrottleNameLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghThrottleNameLabel, gstudioGetFont(GeneralSmall));

	// ghThrottle1ValLabel
	wi.g.show = TRUE;
	wi.g.x = 240;
	wi.g.y = 50;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_WATT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghThrottle1ValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghThrottle1ValLabel, gstudioGetFont(GeneralSmall));

	// ghThrottle2ValLabel
	wi.g.show = TRUE;
	wi.g.x = 170;
	wi.g.y = 50;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_WATT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghThrottle2ValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghThrottle2ValLabel, gstudioGetFont(GeneralSmall));

	// ghTipIntNameLabel
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 68;
	wi.g.width = 160;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(TEXT_INTTEMP);
	wi.customDraw = gwinLabelDrawJustifiedLeftPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghTipIntNameLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghTipIntNameLabel, gstudioGetFont(GeneralSmall));

	// ghTip1IntValLabel
	wi.g.show = TRUE;
	wi.g.x = 240;
	wi.g.y = 68;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_WATT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghTip1IntValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghTip1IntValLabel, gstudioGetFont(GeneralSmall));

	// ghTip2IntValLabel
	wi.g.show = TRUE;
	wi.g.x = 170;
	wi.g.y = 68;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_WATT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghTip2IntValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghTip2IntValLabel, gstudioGetFont(GeneralSmall));

	// ghTipRawNameLabel
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 86;
	wi.g.width = 160;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(TEXT_ADCTEMP);
	wi.customDraw = gwinLabelDrawJustifiedLeftPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghTipRawNameLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghTipRawNameLabel, gstudioGetFont(GeneralSmall));

	// ghTip1RawValLabel
	wi.g.show = TRUE;
	wi.g.x = 240;
	wi.g.y = 86;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_WATT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghTip1RawValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghTip1RawValLabel, gstudioGetFont(GeneralSmall));

	// ghTip2RawValLabel
	wi.g.show = TRUE;
	wi.g.x = 170;
	wi.g.y = 86;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_WATT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghTip2RawValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghTip2RawValLabel, gstudioGetFont(GeneralSmall));

	// ghRefRawNameLabel
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 104;
	wi.g.width = 160;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(TEXT_ADCREF);
	wi.customDraw = gwinLabelDrawJustifiedLeftPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghRefRawNameLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghRefRawNameLabel, gstudioGetFont(GeneralSmall));

	// ghRefRawValLabel
	wi.g.show = TRUE;
	wi.g.x = 170;
	wi.g.y = 104;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_WATT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghRefRawValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghRefRawValLabel, gstudioGetFont(GeneralSmall));

	// ghSupplyNameLabel
	wi.g.show = TRUE;
	wi.g.x = 10;
	wi.g.y = 122;
	wi.g.width = 160;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(TEXT_SUPPLYVOLTAGE);
	wi.customDraw = gwinLabelDrawJustifiedLeftPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghSupplyNameLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghSupplyNameLabel, gstudioGetFont(GeneralSmall));

	// ghSupplyValLabel
	wi.g.show = TRUE;
	wi.g.x = 170;
	wi.g.y = 122;
	wi.g.width = 70;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerMainScreen;
	wi.text = GT(UNITTEXT_WATT);
	wi.customDraw = gwinLabelDrawJustifiedRightPM;
	wi.customParam = 0;
	wi.customStyle = &WarningStyle;
	ghSupplyValLabel = gwinLabelCreate(0, &wi);
	gwinSetFont(ghSupplyValLabel, gstudioGetFont(GeneralSmall));

	return TRUE;
}

// Screen object update functions

// Updates the preset 1 value (Button 1)
static void _updatePreset1(ConfigOnline_t* ConfigOnline)
{
	ltoa(guiInternalToDisplay(ConfigOnline->presetTemp1), _szButton1, 10);
	gwinSetText(ghPreset1button, _szButton1, FALSE);
}

// Updates the preset 2 value (Button 2)
static void _updatePreset2(ConfigOnline_t* ConfigOnline)
{
	ltoa(guiInternalToDisplay(ConfigOnline->presetTemp2), _szButton2, 10);
	gwinSetText(ghPreset2button, _szButton2, FALSE);
}

// Updates the preset 3 value (Button 3)
static void _updatePreset3(ConfigOnline_t* ConfigOnline)
{
	ltoa(guiInternalToDisplay(ConfigOnline->presetTemp3), _szButton3, 10);
	gwinSetText(ghPreset3button, _szButton3, FALSE);
}

// Updates the setpoint value
static void _updateSetpoint(ConfigOnline_t* ConfigOnline)
{
	char tempConv[10];

	if (ConfigOnline->tempUnitC) {
		gwinSetText(ghUnitLabel, GT(UNITTEXT_CELSIUS), FALSE);
	} else {
		gwinSetText(ghUnitLabel, GT(UNITTEXT_FAHRENHEIT), FALSE);
	}

	ltoa(guiInternalToDisplay(ConfigOnline->setpointTemp), tempConv, 10);

	if (_szMainTemp1[0] != tempConv[0]) {
		_szMainTemp1[0] = tempConv[0];
		gwinSetText(ghSetpointLabel1, _szMainTemp1, FALSE);
	}

	if (_szMainTemp2[0] != tempConv[1]) {
		_szMainTemp2[0] = tempConv[1];
		gwinSetText(ghSetpointLabel2, _szMainTemp2, FALSE);
	}

	if (_szMainTemp3[0] != tempConv[2]) {
		_szMainTemp3[0] = tempConv[2];
		gwinSetText(ghSetpointLabel3, _szMainTemp3, FALSE);
	}

	strcat(tempConv, " ");
	if (ConfigOnline->tempUnitC) {
		strcat(tempConv, GT(UNITTEXT_CELSIUS));
	} else {
		strcat(tempConv, GT(UNITTEXT_FAHRENHEIT));
	}

	strcpy(_szSetPoint, tempConv);
	gwinSetText(ghSetpointLabel, _szSetPoint, FALSE);
}

// Main screen
Menu_t guiMenuMainScreen(void)
{
	ConfigOnline_t* ConfigOnline;
	ConfigOffline_t* ConfigOffline;
	DataExchange_t* DataExchange;
	uint16_t tempStep, maxTemp, multiplier, diagnostics, tempDrop;
	uint32_t msg;

	ConfigOnline = configGetConfigOnline();
	DataExchange = controlGetDataExchange();
	guiSetFooterButtons(gstudioGetFont(SmallTemp), _szButton1, _szButton2,
			_szButton3);
	_updateSetpoint(ConfigOnline);
	_updatePreset1(ConfigOnline);
	_updatePreset2(ConfigOnline);
	_updatePreset3(ConfigOnline);
	multiplier = (ConfigOnline->tempUnitC ? 18 : 10);
	diagnostics = ConfigOnline->diagnostics;

	if ((!ConfigOnline->telemetry) !=  (!telemetryIsEnabled())) {
		telemetryControl((uint8_t)ConfigOnline->telemetry);
	}

	DataExchange->control.setpointTemp = ConfigOnline->setpointTemp;

	configReturnConfigOnline();

	ConfigOffline = configGetConfigOffline();
	tempStep = ConfigOffline->tempStep * multiplier;
	maxTemp = ConfigOffline->maxTemp;
	tempDrop = ConfigOffline->tempDrop;
	DataExchange->control.setpointSleepTemp = ConfigOffline->sleepTemp;
	DataExchange->control.delayOff = ConfigOffline->delayOff;
	DataExchange->control.powerLimit = ConfigOffline->powerLimit;
	DataExchange->control.dropTemp = 6000 / (tempDrop / 18);
	guiSetScreenSaver(ConfigOffline->screenOff);

	controlReturnDataExchange();
	configReturnConfigOffline();

	gwinSetVisible(ghUnitLabel, !diagnostics);
	gwinSetVisible(ghSetpointLabel1, !diagnostics);
	gwinSetVisible(ghSetpointLabel2, !diagnostics);
	gwinSetVisible(ghSetpointLabel3, !diagnostics);
	gwinSetVisible(ghProgressbar, !diagnostics);

	gwinSetVisible(ghSetpointLabel, diagnostics);
	gwinSetVisible(ghRailLabel, diagnostics);
	gwinSetVisible(ghVoltageValLabel, diagnostics);
	gwinSetVisible(ghCurrentValLabel, diagnostics);
	gwinSetVisible(ghPowerValLabel, diagnostics);
	gwinSetVisible(ghThrottleNameLabel, diagnostics);
	gwinSetVisible(ghThrottle1ValLabel, diagnostics);
	gwinSetVisible(ghThrottle2ValLabel, diagnostics);
	gwinSetVisible(ghTipRawNameLabel, diagnostics);
	gwinSetVisible(ghTip1RawValLabel, diagnostics);
	gwinSetVisible(ghTip2RawValLabel, diagnostics);
	gwinSetVisible(ghRefRawNameLabel, diagnostics);
	gwinSetVisible(ghRefRawValLabel, diagnostics);
	gwinSetVisible(ghSupplyNameLabel, diagnostics);
	gwinSetVisible(ghSupplyValLabel, diagnostics);
	gwinSetVisible(ghTipIntNameLabel, diagnostics);
	gwinSetVisible(ghTip1IntValLabel, diagnostics);
	gwinSetVisible(ghTip2IntValLabel, diagnostics);

	gwinSetText(ghRailLabel, GT(TEXT_24VACRAIL), FALSE);
	gwinSetText(ghThrottleNameLabel, GT(TEXT_THROTTLE), FALSE);
	gwinSetText(ghTipRawNameLabel, GT(TEXT_ADCTEMP), FALSE);
	gwinSetText(ghRefRawNameLabel, GT(TEXT_ADCREF), FALSE);
	gwinSetText(ghSupplyNameLabel, GT(TEXT_SUPPLYVOLTAGE), FALSE);
	gwinSetText(ghTipIntNameLabel, GT(TEXT_INTTEMP), FALSE);


	msg = notifyReceive();
	if (msg) {
		guiSetPage(MAINSCREEN);
		guiShowNotify(msg);
	} else {
		guiShowPage(MAINSCREEN);
	}

	while (1) {

		// Poll event
		StationEvent_t event = guiGetStationEvent();

		if (event != SE_NONE) {
			guiEventCheckToolControl(event);

			// Event with configuration
			ConfigOnline = configGetConfigOnline();
			DataExchange = controlGetDataExchange();
			if (ConfigOnline != NULL) {
				switch (event) {
				case SE_ROT_INC:
					ConfigOnline->setpointTemp += tempStep;
					_updateSetpoint(ConfigOnline);
					break;
				case SE_ROT_IINC:
					ConfigOnline->setpointTemp += tempStep * 2;
					_updateSetpoint(ConfigOnline);
					break;
				case SE_ROT_IIINC:
					ConfigOnline->setpointTemp += tempStep * 4;
					_updateSetpoint(ConfigOnline);
					break;
				case SE_ROT_DEC:
					ConfigOnline->setpointTemp -= tempStep;
					_updateSetpoint(ConfigOnline);
					break;
				case SE_ROT_DDEC:
					ConfigOnline->setpointTemp -= tempStep * 2;
					_updateSetpoint(ConfigOnline);
					break;
				case SE_ROT_DDDEC:
					ConfigOnline->setpointTemp -= tempStep * 4;
					_updateSetpoint(ConfigOnline);
					break;
				case SE_PRESET1_UP:
					ConfigOnline->setpointTemp = ConfigOnline->presetTemp1;
					_updateSetpoint(ConfigOnline);
					break;
				case SE_PRESET2_UP:
					ConfigOnline->setpointTemp = ConfigOnline->presetTemp2;
					_updateSetpoint(ConfigOnline);
					break;
				case SE_PRESET3_UP:
					ConfigOnline->setpointTemp = ConfigOnline->presetTemp3;
					_updateSetpoint(ConfigOnline);
					break;
				case SE_PRESET1_LONG:
					ConfigOnline->presetTemp1 = ConfigOnline->setpointTemp;
					_updatePreset1(ConfigOnline);
					soundNotice();
					break;
				case SE_PRESET2_LONG:
					ConfigOnline->presetTemp2 = ConfigOnline->setpointTemp;
					_updatePreset2(ConfigOnline);
					soundNotice();
					break;
				case SE_PRESET3_LONG:
					ConfigOnline->presetTemp3 = ConfigOnline->setpointTemp;
					_updatePreset3(ConfigOnline);
					soundNotice();
					break;
				default:
					;
				}

				if (ConfigOnline->setpointTemp < 2120) {
					ConfigOnline->setpointTemp = 2120; // 100 �C
					_updateSetpoint(ConfigOnline);
				}

				if (ConfigOnline->setpointTemp > maxTemp) {
					ConfigOnline->setpointTemp = maxTemp;
					_updateSetpoint(ConfigOnline);
				}

				DataExchange->control.setpointTemp = ConfigOnline->setpointTemp;
				controlReturnDataExchange();
				configReturnConfigOnline();
			}

			// Events without configuration
			switch (event) {
			case SE_ROTSW_UP:
				return MENU_SCREEN;
			default:
				;
			}
		}

		// Check for notifications
		msg = notifyReceive();
		if (msg)
			guiShowNotify(msg);
	}

	return MAIN_SCREEN;

}
