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

GHandle ghPageContainerAlarmScreen;
GHandle ghNotifyType;
GHandle ghNotifyText;
GHandle ghNotifyDesc;

bool_t guiCreatePageNotifyScreen(void)
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
	ghPageContainerAlarmScreen = gwinContainerCreate(0, &wi, 0);

	// ghAlarmType
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 1;
	wi.g.width = 314;
	wi.g.height = 38;
	wi.g.parent = ghPageContainerAlarmScreen;
	wi.text = "";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &AlarmStyle;
	ghNotifyType = gwinLabelCreate(0, &wi);
	gwinSetFont(ghNotifyType, gstudioGetFont(GeneralLarge));

	// ghAlarmText
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 40;
	wi.g.width = 314;
	wi.g.height = 50;
	wi.g.parent = ghPageContainerAlarmScreen;
	wi.text = "";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &AlarmStyle;
	ghNotifyText = gwinLabelCreate(0, &wi);
	gwinSetFont(ghNotifyText, gstudioGetFont(General));

	// ghAlarmDesc
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 94;
	wi.g.width = 314;
	wi.g.height = 40;
	wi.g.parent = ghPageContainerAlarmScreen;
	wi.text = "";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &AlarmStyle;
	ghNotifyDesc = gwinLabelCreate(0, &wi);
	gwinSetFont(ghNotifyDesc, gstudioGetFont(GeneralSmall));

	return TRUE;
}

static void _setNotifyStyle(const GWidgetStyle* style)
{
	gwinSetStyle(ghNotifyType, style);
	gwinSetStyle(ghNotifyText, style);
	gwinSetStyle(ghNotifyDesc, style);
}

static void _setNotifyText(int textid)
{
	gwinSetText(ghNotifyText, GT(textid), FALSE);
	gwinSetText(ghNotifyDesc, GT(textid + 1), FALSE);
}

void guiShowNotify(uint32_t message)
{
	// Save original state
	guiPage page = guiGetPage();
	const char* text1 = ((GWidgetObject *) ghPreset1button)->text;
	const char* text2 = ((GWidgetObject *) ghPreset2button)->text;
	const char* text3 = ((GWidgetObject *) ghPreset3button)->text;
	font_t font = ghPreset1button->font;
	uint32_t msg = message;
	uint8_t update = 1;

	guiSetFooterButtons(gstudioGetFont(General), GT(TEXT_EMPTY), GT(TEXT_EMPTY),
			GT(MENUCONTROL_ACK));
	soundMessage();
	guiShowPage(NOTIFYSCREEN);

	while (1) {
		if (update) {
			// Set styles
			switch (msg) {
			case ALARM_CALIBRATION:
			case ALARM_OVERVOLTAGE:
			case ALARM_OVERCURRENT:
			case ALARM_HIGHTIPTEMPERATURE:
			case ALARM_HIGHAMBIENTTEMPERATURE:
				_setNotifyStyle(&AlarmStyle);
				gwinSetText(ghNotifyType, GT(NOTIFYTYPE_ALARM), FALSE);
				break;
			case WARNING_BATTERY:
			case WARNING_DEBUGMODE:
			case WARNING_UNCALIBRATEDIRON:
			case WARNING_THERMALTHROTTLING:
			case WARNING_REPORTFAILED:
			case WARNING_SETTINGSLOST:
				_setNotifyStyle(&WarningStyle);
				gwinSetText(ghNotifyType, GT(NOTIFYTYPE_WARNING), FALSE);
				break;
			default:
				_setNotifyStyle(&InfoStyle);
				gwinSetText(ghNotifyType, GT(NOTIFYTYPE_INFORMATION), FALSE);
			}

			// Set message
			switch (msg) {
			case ALARM_CALIBRATION:
				_setNotifyText(ALARMTEXT_CALIBRATION);
				break;
			case ALARM_OVERVOLTAGE:
				_setNotifyText(ALARMTEXT_OVERVOLTAGE);
				break;
			case ALARM_OVERCURRENT:
				_setNotifyText(ALARMTEXT_OVERCURRENT);
				break;
			case ALARM_HIGHTIPTEMPERATURE:
				_setNotifyText(ALARMTEXT_HIGHTIPTEMPERATURE);
				break;
			case ALARM_HIGHAMBIENTTEMPERATURE:
				_setNotifyText(ALARMTEXT_HIGHAMBIENTTEMPERATURE);
				break;
			case WARNING_BATTERY:
				_setNotifyText(WARNINGTEXT_BATTERY);
				break;
			case WARNING_DEBUGMODE:
				_setNotifyText(WARNINGTEXT_DEBUGMODE);
				break;
			case WARNING_UNCALIBRATEDIRON:
				_setNotifyText(WARNINGTEXT_UNCALIBRATEDIRON);
				break;
			case WARNING_THERMALTHROTTLING:
				_setNotifyText(WARNINGTEXT_THERMALTHROTTLING);
				break;
			case WARNING_REPORTFAILED:
				_setNotifyText(WARNINGTEXT_REPORTFAILED);
				break;
			case WARNING_SETTINGSLOST:
				_setNotifyText(WARNINGTEXT_SETTINGSLOST);
				break;
			case INFO_CALIBRATION:
				_setNotifyText(INFOTEXT_IRONCALIBRATION);
				break;
			case INFO_CALIBRATIONDONE:
				_setNotifyText(INFOTEXT_CALIBRATIONDONE);
				break;
			case INFO_SETTINGSRESET:
				_setNotifyText(INFOTEXT_SETTINGSRESET);
			default:
				;
			}

			update = 0;
		}


		// Poll event
		StationEvent_t event = guiGetStationEvent();

		if (event != SE_NONE) {
			guiEventCheckToolControl(event);

			switch (event) {
			case SE_PRESET3_UP:
			case SE_ROTSW_UP:
				msg = notifyReceive();
				if (!msg) {
					guiSetFooterButtons(font, text1, text2, text3);
					guiShowPage(page);
					return;
				}
				soundMessage();
				update = 1;
				break;
			default:
				;
			}
		}
	}
	// Should never get here

}

// Dialog

uint8_t guiShowDialog(int header, int message)
{
	// Save original state
	guiPage page = guiGetPage();
	const char* text1 = ((GWidgetObject *) ghPreset1button)->text;
	const char* text2 = ((GWidgetObject *) ghPreset2button)->text;
	const char* text3 = ((GWidgetObject *) ghPreset3button)->text;
	font_t font = ghPreset1button->font;

	guiSetFooterButtons(gstudioGetFont(General), GT(MENUCONTROL_CANCEL),
			GT(TEXT_EMPTY), GT(MENUCONTROL_OK));
	gwinSetText(ghNotifyType, GT(header), FALSE);
	_setNotifyText(message);
	soundMessage();

	switch (header) {
	case NOTIFYTYPE_ALARM:
		_setNotifyStyle(&AlarmStyle);
		break;
	case NOTIFYTYPE_WARNING:
		_setNotifyStyle(&WarningStyle);
		break;
	default:
		_setNotifyStyle(&InfoStyle);
	}

	guiShowPage(NOTIFYSCREEN);

	while (1) {

		// Poll event
		StationEvent_t event = guiGetStationEvent();

		if (event != SE_NONE) {
			guiEventCheckToolControl(event);

			switch (event) {
			case SE_PRESET1_UP:
				guiSetFooterButtons(font, text1, text2, text3);
				guiShowPage(page);
				return 0;
			case SE_PRESET3_UP:
			case SE_ROTSW_UP:
				guiSetFooterButtons(font, text1, text2, text3);
				guiShowPage(page);
				return 1;
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

}


