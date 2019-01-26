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

// Gamma calibration
GHandle ghPageContainerGammaCalibrationScreen;
GHandle ghGammaRect;
GHandle ghGammaValue;
GHandle ghPosGamma[ILI9341_GAMMACURVEPOINTS];
GHandle ghNegGamma[ILI9341_GAMMACURVEPOINTS];

// Gamma calibration screen
bool_t guiCreatePageGammaCalibrationScreen(void)
{

	GWidgetInit wi;
	int i;
	coord_t rw, ro;

	gwinWidgetClearInit(&wi);

	// Page container
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = 320;
	wi.g.height = 240;
	wi.g.parent = 0;
	wi.text = "Container";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghPageContainerGammaCalibrationScreen = gwinContainerCreate(0, &wi, 0);
	((GWidgetObject*)ghPageContainerGammaCalibrationScreen)->g.flags &= ~GWIN_CONTAINER_BORDER;

	// ghGammaRect
	wi.g.show = TRUE;
	wi.g.x = 12;
	wi.g.y = 3;
	wi.g.width = 297;
	wi.g.height = 112;
	wi.g.parent = ghPageContainerGammaCalibrationScreen;
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghGammaRect = gammarectangleCreate(0, &wi);

	rw = 300 / ILI9341_GAMMACURVEPOINTS;
	ro = (320 - (rw * ILI9341_GAMMACURVEPOINTS)) / 2;

	font_t font = gdispOpenFont("UI1");

	for (i = 0; i < ILI9341_GAMMACURVEPOINTS; i++)
	{
		wi.g.show = TRUE;
		wi.g.x = ro + i * rw;
		wi.g.y = 200;
		wi.g.width = rw;
		wi.g.height = 20;
		wi.g.parent = ghPageContainerGammaCalibrationScreen;
		wi.text = "00";
		wi.customDraw = gwinLabelDrawJustifiedCenterPM;
		wi.customParam = 0;
		wi.customStyle = &UnSelectedElement;
		ghPosGamma[i] = gwinLabelCreate(0, &wi);
		gwinSetFont(ghPosGamma[i], font);
		wi.g.y = 220;
		ghNegGamma[i] = gwinLabelCreate(0, &wi);
		gwinSetFont(ghNegGamma[i], font);
	}

	wi.g.show = TRUE;
	wi.g.x = 140;
	wi.g.y = 160;
	wi.g.width = 40;
	wi.g.height = 20;
	wi.g.parent = ghPageContainerGammaCalibrationScreen;
	wi.text = "00";
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &UnSelectedElement;
	ghGammaValue = gwinLabelCreate(0, &wi);

	return TRUE;
}

// Tool calibration

Menu_t guiMenuToolCalibration(void)
{
	uint8_t step = 0;
	uint16_t values[3];
	CalibOffline_t* CalibOffline;
	ConfigOnline_t* ConfigOnline;
	DataExchange_t* DataExchange;
	ToolType_t toolType;
	uint16_t ambitemp, addition;
	const char* unitText;
	char val[10];

	guiSetFooterButtons(gstudioGetFont(General), GT(MENUCONTROL_BACK),
			GT(TEXT_EMPTY), GT(MENUCONTROL_NEXT));
	gwinSetText(ghParamName, GT(CALIBTEXT_1STCALIBRATIONPOINT), FALSE);
	gwinSetText(ghParamDesc, GT(CALIBTEXT_CALIBRATION_DESC), FALSE);
	menuscrollbarSetup(ghParamScrollBar, 3, 1);
	gwinSetVisible(ghParamScrollBar, TRUE);
	guiSetPage(SETTINGSSCREEN);

	guiShowNotify(INFO_CALIBRATION);
	do {
		DataExchange = controlGetDataExchange();
		toolType = DataExchange->state.tool1.toolType;
	#ifdef USE_STARTING_TEMPERATURE
		ambitemp = DataExchange->state.startTempInt;
	#else
		ambitemp = DataExchange->state.ambientTempInt;
	#endif
		controlReturnDataExchange();
		if (toolType == TOOL_NOTYPE) {
			if (!guiShowDialog(NOTIFYTYPE_WARNING, DIALOGTEXT_NOIRON))
				return SYSTEMMENU_SCREEN;
		}
	} while (toolType == TOOL_NOTYPE);

	CalibOffline = configGetCalibOffline();
	if (toolType == TOOL_T245) {
		values[0] = CalibOffline->toolT245.calibrationPointTemp[0] + ambitemp; // With cold junction
		values[1] = CalibOffline->toolT245.calibrationPointTemp[1] + ambitemp; // With cold junction
		values[2] = CalibOffline->toolT245.calibrationPointTemp[2] + ambitemp; // With cold junction
	} else {
		values[0] = CalibOffline->toolT210.calibrationPointTemp[0] + ambitemp; // With cold junction
		values[1] = CalibOffline->toolT210.calibrationPointTemp[1] + ambitemp; // With cold junction
		values[2] = CalibOffline->toolT210.calibrationPointTemp[2] + ambitemp; // With cold junction
	}
	configReturnCalibOffline();

	ConfigOnline = configGetConfigOnline();
	addition = (ConfigOnline->tempUnitC ? 18 : 10);
	unitText = (
			ConfigOnline->tempUnitC ?
					GT(UNITTEXT_CELSIUS) : GT(UNITTEXT_FAHRENHEIT));
	configReturnConfigOnline();

	gwinSetEnabled(ghParamValue, TRUE);

	DataExchange = controlGetDataExchange();
	DataExchange->control.calibPointTool1 = 1;
	controlReturnDataExchange();

	while (1) {
		ltoa(guiInternalToDisplay(values[step]), val, 10);
		strcat(val, " ");
		strcat(val, unitText);

		gwinSetText(ghParamValue, val, FALSE);
		menuscrollbarSetTopItem(ghParamScrollBar, step);

		switch (step) {
		case 0:
			gwinSetText(ghParamName, GT(CALIBTEXT_1STCALIBRATIONPOINT), FALSE);
			break;
		case 1:
			gwinSetText(ghParamName, GT(CALIBTEXT_2NDCALIBRATIONPOINT), FALSE);
			break;
		case 2:
			gwinSetText(ghParamName, GT(CALIBTEXT_3RDCALIBRATIONPOINT), FALSE);
			break;
		}

		// Poll event
		StationEvent_t event = guiGetStationEvent();

		if (event != SE_NONE) {
			switch (event) {
			case SE_ROT_INC:
				values[step] += addition;
				break;
			case SE_ROT_IINC:
				values[step] += addition * 2;
				break;
			case SE_ROT_IIINC:
				values[step] += addition * 4;
				break;
			case SE_ROT_DEC:
				values[step] -= addition;
				break;
			case SE_ROT_DDEC:
				values[step] -= addition * 2;
				break;
			case SE_ROT_DDDEC:
				values[step] -= addition * 4;
				break;
			case SE_PRESET1_UP:
				DataExchange = controlGetDataExchange();
				DataExchange->control.calibPointTool1 = 0;
				controlReturnDataExchange();
				return SYSTEMMENU_SCREEN;
			case SE_PRESET2_UP:
				if (step)
					step--;
				if (!step) {
					guiSetFooterButtons(gstudioGetFont(General),
							GT(MENUCONTROL_BACK), GT(TEXT_EMPTY),
							GT(MENUCONTROL_NEXT));
				} else {
					guiSetFooterButtons(gstudioGetFont(General),
							GT(MENUCONTROL_BACK), GT(MENUCONTROL_PREVIOUS),
							GT(MENUCONTROL_NEXT));
				}
				DataExchange = controlGetDataExchange();
				DataExchange->control.calibPointTool1 = step + 1;
				controlReturnDataExchange();
				break;
			case SE_PRESET3_UP:
			case SE_ROTSW_UP:
				step++;
				if (step < 2) {
					guiSetFooterButtons(gstudioGetFont(General),
							GT(MENUCONTROL_BACK), GT(MENUCONTROL_PREVIOUS),
							GT(MENUCONTROL_NEXT));
				} else {
					guiSetFooterButtons(gstudioGetFont(General),
							GT(MENUCONTROL_BACK), GT(MENUCONTROL_PREVIOUS),
							GT(MENUCONTROL_ENTER));
				}
				if (step > 2) {
					CalibOffline = configGetCalibOffline();
					if (toolType == TOOL_T245) {
						CalibOffline->toolT245.calibrationPointTemp[0] =
								values[0] - ambitemp; // Without cold junction
						CalibOffline->toolT245.calibrationPointTemp[1] =
								values[1] - ambitemp; // Without cold junction
						CalibOffline->toolT245.calibrationPointTemp[2] =
								values[2] - ambitemp; // Without cold junction
						CalibOffline->toolT245.calibrated = 1;
					} else {
						CalibOffline->toolT210.calibrationPointTemp[0] =
								values[0] - ambitemp; // Without cold junction
						CalibOffline->toolT210.calibrationPointTemp[1] =
								values[1] - ambitemp; // Without cold junction
						CalibOffline->toolT210.calibrationPointTemp[2] =
								values[2] - ambitemp; // Without cold junction
						CalibOffline->toolT210.calibrated = 1;
					}
					configStoreCalibOffline();
					configReturnCalibOffline();
					DataExchange = controlGetDataExchange();
					DataExchange->control.calibPointTool1 = 0;
					controlReturnDataExchange();
					notifySend(INFO_CALIBRATIONDONE);
					return SYSTEMMENU_SCREEN;
				}
				DataExchange = controlGetDataExchange();
				DataExchange->control.calibPointTool1 = step + 1;
				controlReturnDataExchange();

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
	return SYSTEMMENU_SCREEN;
}

// Gamma calibration
Menu_t guiMenuGammaCalibration(void)
{
	StationEvent_t event;
	uint16_t selected = 0;
	bool_t editing = FALSE;
	char vals[ILI9341_GAMMACURVEPOINTS * 2][3];
	char conv[20];
	uint8_t *gamma, *gammapos = 0;
	CalibOffline_t* CalibOffline;

	guiShowPage(GAMMASCREEN);

	do {

		// Poll event
		event = guiGetStationEvent();
		CalibOffline = configGetCalibOffline();

		if (CalibOffline != NULL) {
			// Lazy update
			gamma = (uint8_t*) &(CalibOffline->gammaCurve);
			for (int i = 0; i < ILI9341_GAMMACURVEPOINTS * 2; i++) {
				ltoa(*gamma, conv, 16);
				memcpy(vals[i], conv, 3);
				vals[i][2] = 0;
				if (i < ILI9341_GAMMACURVEPOINTS) {
					gwinSetText(ghPosGamma[i], vals[i], FALSE);
					if (selected == i) {
						gwinSetStyle(ghPosGamma[i], &SelectedElement);
						gwinSetText(ghGammaValue, vals[i], FALSE);
						gammapos = gamma;
					} else
						gwinSetStyle(ghPosGamma[i], &UnSelectedElement);
				} else {
					gwinSetText(ghNegGamma[i - ILI9341_GAMMACURVEPOINTS],
							vals[i], FALSE);
					if (selected == i) {
						gwinSetStyle(ghNegGamma[i - ILI9341_GAMMACURVEPOINTS],
								&SelectedElement);
						gwinSetText(ghGammaValue, vals[i], FALSE);
						gammapos = gamma;
					} else
						gwinSetStyle(ghNegGamma[i - ILI9341_GAMMACURVEPOINTS],
								&UnSelectedElement);
				}
				gamma++;
			}
			if (editing)
				gwinSetStyle(ghGammaValue, &SelectedElement);
			else
				gwinSetStyle(ghGammaValue, &UnSelectedElement);

			switch (event) {
			case SE_ROT_IIINC:
			case SE_ROT_IINC:
			case SE_ROT_INC:
				if (editing) {
					(*gammapos)++;
					ILI9341_LimitGammaCurve(&(CalibOffline->gammaCurve));
					gdispControl(GDISP_CONTROL_GAMMACURVE,
							(void* )&(CalibOffline->gammaCurve));
				} else
					selected++;
				break;

			case SE_ROT_DDDEC:
			case SE_ROT_DDEC:
			case SE_ROT_DEC:
				if (editing) {
					(*gammapos)--;
					ILI9341_LimitGammaCurve(&(CalibOffline->gammaCurve));
					gdispControl(GDISP_CONTROL_GAMMACURVE,
							(void* )&(CalibOffline->gammaCurve));
				} else
					selected--;
				break;

			case SE_ROTSW_UP:
				editing = !editing;
				break;
			case SE_PRESET2_UP:
				(*gammapos) = 0;
				ILI9341_LimitGammaCurve(&(CalibOffline->gammaCurve));
				gdispControl(GDISP_CONTROL_GAMMACURVE,
						(void* )&(CalibOffline->gammaCurve));
				break;
			case SE_PRESET3_UP:
				(*gammapos) = 0xFF;
				ILI9341_LimitGammaCurve(&(CalibOffline->gammaCurve));
				gdispControl(GDISP_CONTROL_GAMMACURVE,
						(void* )&(CalibOffline->gammaCurve));
				break;
			case SE_TOOL2_UP:
				gamma = (uint8_t*) &(CalibOffline->gammaCurve);
				uint32_t defaultVal;
				defaultVal = (uint32_t) gammapos - (uint32_t) gamma;
				defaultVal += (uint32_t) &ILI9341_defaultGammaCurve;
				gamma = (uint8_t*) defaultVal;
				*gammapos = *gamma;
				ILI9341_LimitGammaCurve(&(CalibOffline->gammaCurve));
				gdispControl(GDISP_CONTROL_GAMMACURVE,
						(void* )&(CalibOffline->gammaCurve));
				break;
			case SE_TOOL1_UP:
				selected += ILI9341_GAMMACURVEPOINTS;
				if (selected >= ILI9341_GAMMACURVEPOINTS * 2)
					selected -= ILI9341_GAMMACURVEPOINTS * 2;
				break;
			default:
				;
			}
			configReturnCalibOffline();

			if (selected == 0xFFFF)
				selected = ILI9341_GAMMACURVEPOINTS * 2 - 1;
			if (selected >= ILI9341_GAMMACURVEPOINTS * 2)
				selected = 0;
		}

		// Check for notifications
		uint32_t msg = notifyReceive();
		if (msg)
			guiShowNotify(msg);

	} while (event != SE_PRESET1_UP);

	return SYSTEMMENU_SCREEN;
}

