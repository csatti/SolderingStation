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

GHandle ghPageContainerHeader;
GHandle ghPageContainerFooter;
GHandle ghPreset1button;
GHandle ghPreset3button;
GHandle ghPreset2button;
GHandle ghToolLeft;
GHandle ghToolRight;
GHandle ghStatusBar;

static char _szVoltage[10];
static char _szCurrent[10];
static char _szPower[10];
static char _szThrottle1[10];
static char _szThrottle2[10];
static char _szTip1Raw[10];
static char _szTip2Raw[10];
static char _szTip1Int[10];
static char _szTip2Int[10];
static char _szRefRaw[10];
static char _szSupply[10];

bool_t guiCreateHeaderAndFooter(void)
{
	GWidgetInit wi;

	gwinWidgetClearInit(&wi);

	// Header
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = 320;
	wi.g.height = 60;
	wi.g.parent = 0;
	wi.text = "Header";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghPageContainerHeader = gwinContainerCreate(0, &wi, 0);
	((GWidgetObject*)ghPageContainerHeader)->g.flags &= ~GWIN_CONTAINER_BORDER;

	// ghToolLeft
	wi.g.show = TRUE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.parent = ghPageContainerHeader;
	wi.customDraw = 0;
	wi.customParam = 0;
	ghToolLeft = toolstatusCreate(0, &wi, TOOL_LEFT);
	gwinSetFont(ghToolLeft, gstudioGetFont(General));
	toolstatusSetState(ghToolLeft, TOOL_SLEEP);
	toolstatusSetTemp(ghToolLeft, 189U);
	toolstatusSetType(ghToolLeft, TOOL_T245);

	// ghToolRight
	wi.g.show = TRUE;
	wi.g.x = 210;
	wi.g.y = 0;
	wi.g.parent = ghPageContainerHeader;
	wi.customDraw = 0;
	wi.customParam = 0;
	ghToolRight = toolstatusCreate(0, &wi, TOOL_RIGHT);
	gwinSetFont(ghToolRight, gstudioGetFont(General));
	toolstatusSetState(ghToolRight, TOOL_READY);
	toolstatusSetTemp(ghToolRight, 322U);
	toolstatusSetType(ghToolRight, TOOL_T245);

	// ghStatusBar
	wi.g.show = TRUE;
	wi.g.x = 110;
	wi.g.y = 0;
	wi.g.parent = ghPageContainerHeader;
	wi.customDraw = 0;
	wi.customParam = 0;
	ghStatusBar = statusbarCreate(0, &wi);
	gwinSetFont(ghStatusBar, gstudioGetFont(GeneralSmall));
	gwinSetColor(ghStatusBar, GFX_BLACK);
	statusbarSetIcons(ghStatusBar, 0);
	statusbarSetTemp(ghStatusBar, 23U);

	// Footer
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 205;
	wi.g.width = 320;
	wi.g.height = 35;
	wi.g.parent = 0;
	wi.text = "Footer";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghPageContainerFooter = gwinContainerCreate(0, &wi, 0);
	((GWidgetObject*)ghPageContainerFooter)->g.flags &= ~GWIN_CONTAINER_BORDER;

	// ghPreset1button
	wi.g.show = TRUE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.parent = ghPageContainerFooter;
	wi.text = "230";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghPreset1button = simpleimageCreate(0, &wi, &IMG_REDBUTTON_PNG);
	gwinSetFont(ghPreset1button, gstudioGetFont(SmallTemp));
	gwinSetColor(ghPreset1button, GFX_BLACK);

	// ghPreset3button
	wi.g.show = TRUE;
	wi.g.x = 213;
	wi.g.y = 0;
	wi.g.parent = ghPageContainerFooter;
	wi.text = "420";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghPreset3button = simpleimageCreate(0, &wi, &IMG_GREENBUTTON_PNG);
	gwinSetFont(ghPreset3button, gstudioGetFont(SmallTemp));
	gwinSetColor(ghPreset3button, GFX_BLACK);

	// ghPreset2button
	wi.g.show = TRUE;
	wi.g.x = 107;
	wi.g.y = 0;
	wi.g.parent = ghPageContainerFooter;
	wi.text = "310";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghPreset2button = simpleimageCreate(0, &wi, &IMG_WHITEBUTTON_PNG);
	gwinSetFont(ghPreset2button, gstudioGetFont(SmallTemp));
	gwinSetColor(ghPreset2button, GFX_BLACK);


	return TRUE;
}

static void _convertRailVal(uint16_t val, char* str, const char* unit)
{
	int len;

	ltoa(val, str, 10);
	len = strlen(str);
	if (len == 1) {
		len++;
		*(str+1) = '0';
	}

	*(str+len+1) = 0;
	*(str+len) = *(str+len-1);
	*(str+len-1) = *GT(UNITTEXT_DECIMALPOINT);

	strcat(str, " ");
	strcat(str, unit);
}

// This task updates the header state
static void _headerUpdateTask(void *pvParameters)
{
	uint8_t tempUpdate = 0;

	vTaskDelay(HEADER_UPDATE_START_DELAY);

	while (1) {
		ConfigOnline_t* ConfigOnline = configGetConfigOnline();
		DataExchange_t* DataExchange = controlGetDataExchange();
		uint32_t icons = 0;

		uint16_t ambitemp = guiInternalToDisplay(
				DataExchange->state.ambientTempInt);
		uint16_t toolTemp, power, railPower, railVoltage, railCurrent;
		uint16_t tool1Raw, tool2Raw, refRaw, throttle1, throttle2, supply , tool1Int, tool2Int;

		if (!ConfigOnline->sound)
			icons += SB_ICON_SOUNDOFF;

		configReturnConfigOnline();

		if ((DataExchange->state.tool1.throttlingOn) || (DataExchange->state.tool2.throttlingOn))
			icons += SB_ICON_THERMAL;

		toolstatusSetState(ghToolRight, DataExchange->state.tool1.toolState);
		toolTemp = tool1Int = DataExchange->state.tool1.toolTempInt;
		toolTemp = (toolTemp < TEMP_COLD ? 0 : guiInternalToDisplay(toolTemp));

		if (!tempUpdate) toolstatusSetTemp(ghToolRight, toolTemp);
		toolstatusSetType(ghToolRight, DataExchange->state.tool1.toolType);

		toolstatusSetState(ghToolLeft, DataExchange->state.tool2.toolState);
		toolTemp = tool2Int = DataExchange->state.tool2.toolTempInt;
		toolTemp = (toolTemp < TEMP_COLD ? 0 : guiInternalToDisplay(toolTemp));
		if (!tempUpdate) toolstatusSetTemp(ghToolLeft, toolTemp);
		toolstatusSetType(ghToolLeft, DataExchange->state.tool2.toolType);

		railPower = DataExchange->state.railPower;
		railVoltage = DataExchange->state.railVoltage;
		railCurrent = DataExchange->state.railCurrent;
		tool1Raw = DataExchange->state.tool1.toolTempRaw;
		tool2Raw = DataExchange->state.tool2.toolTempRaw;
		throttle1 = DataExchange->state.tool1.thermalThrottle;
		throttle2 = DataExchange->state.tool2.thermalThrottle;
		refRaw = DataExchange->state.refVoltageRaw;
		supply = DataExchange->state.supplyVoltage;

		controlReturnDataExchange();

		power = (uint16_t)(((uint32_t)railPower * 10UL) / (uint32_t)NOMINAL_POWER);
		if (power > 100) power = 100;

		if (!tempUpdate) statusbarSetTemp(ghStatusBar, ambitemp);
		statusbarSetIcons(ghStatusBar, icons);

		gwinProgressbarSetPosition(ghProgressbar, power);

		_convertRailVal(railPower, _szPower, GT(UNITTEXT_WATT));
		_convertRailVal(railVoltage, _szVoltage, GT(UNITTEXT_VOLT));
		_convertRailVal(railCurrent, _szCurrent, GT(UNITTEXT_AMPER));

		ltoa(tool1Raw, _szTip1Raw, 10);
		ltoa(tool2Raw, _szTip2Raw, 10);
		ltoa(tool1Int, _szTip1Int, 10);
		ltoa(tool2Int, _szTip2Int, 10);
		ltoa(throttle1, _szThrottle1, 10);
		ltoa(throttle2, _szThrottle2, 10);
		ltoa(refRaw, _szRefRaw, 10);
		ltoa(supply, _szSupply, 10);

		strcat(_szSupply, " ");
		strcat(_szSupply, GT(UNITTEXT_MILIVOLT));

		gwinSetText(ghVoltageValLabel, _szPower, FALSE);
		gwinSetText(ghCurrentValLabel, _szVoltage, FALSE);
		gwinSetText(ghPowerValLabel, _szCurrent, FALSE);
		gwinSetText(ghThrottle1ValLabel, _szThrottle1, FALSE);
		gwinSetText(ghThrottle2ValLabel, _szThrottle2, FALSE);
		gwinSetText(ghTip1RawValLabel, _szTip1Raw, FALSE);
		gwinSetText(ghTip2RawValLabel, _szTip2Raw, FALSE);
		gwinSetText(ghTip1IntValLabel, _szTip1Int, FALSE);
		gwinSetText(ghTip2IntValLabel, _szTip2Int, FALSE);
		gwinSetText(ghRefRawValLabel, _szRefRaw, FALSE);
		gwinSetText(ghSupplyValLabel, _szSupply, FALSE);

		vTaskDelay(HEADER_UPDATE_CYCLE);
		tempUpdate++;
		if (tempUpdate > 2) tempUpdate = 0;
	}
}

void guiHeaderInit(void)
{
	xTaskCreate(_headerUpdateTask, (char *) "HeaderUpd",
		120,
		NULL,
		mainQUEUE_HEADER_UPDATE_TASK_PRIORITY,
		NULL);;
}
