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

// Start screen
GHandle ghPageContainerStartScreen;
GHandle ghStartPicture;
GHandle ghStartName;
GHandle ghStartCreator;
GHandle ghStartVersion;
GHandle ghStartVersionShort;

// Firmware screen
GHandle ghPageContainerUpdateScreen;
GHandle ghUpdatePicture;
GHandle ghUpdateText;

/************************** START SCREEN *****************************/

bool_t guiCreateStartScreen(void)
{
	GWidgetInit wi;

	gwinWidgetClearInit(&wi);

	// ghPageContainerStartScreen
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = 320;
	wi.g.height = 240;
	wi.g.parent = 0;
	wi.text = "Start";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghPageContainerStartScreen = gwinContainerCreate(0, &wi, 0);

	// ghStartPicture
	wi.g.show = TRUE;
	wi.g.x = 96;
	wi.g.y = 50;
	wi.g.parent = ghPageContainerStartScreen;
	wi.text = "";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghStartPicture = simpleimageCreate(0, &wi, &IMG_SPLASH_PNG);

	// ghStartName
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 162;
	wi.g.width = 314;
	wi.g.height = 32;
	wi.g.parent = ghPageContainerStartScreen;
	wi.text = GT(TEXT_STATIONNAME);
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghStartName = gwinLabelCreate(0, &wi);
	gwinSetFont(ghStartName, gstudioGetFont(General));

	// ghStartCreator
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 200;
	wi.g.width = 314;
	wi.g.height = 22;
	wi.g.parent = ghPageContainerStartScreen;
	wi.text = GT(TEXT_AUTHOR);
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghStartCreator = gwinLabelCreate(0, &wi);
	gwinSetFont(ghStartCreator, gstudioGetFont(GeneralSmall));

	// ghStartVersion
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 222;
	wi.g.width = 314;
	wi.g.height = 18;
	wi.g.parent = ghPageContainerStartScreen;
	wi.text = TEXT_VERSION;
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghStartVersion = gwinLabelCreate(0, &wi);
	gwinSetFont(ghStartVersion, gstudioGetFont(GeneralTiny));

	// ghStartVersionShort
	/*wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 162;
	wi.g.width = 314;
	wi.g.height = 19;
	wi.g.parent = ghPageContainerStartScreen;
	wi.text = TEXT_VERSIONSHORT;
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghStartVersion = gwinLabelCreate(0, &wi);
	gwinSetFont(ghStartVersion, gstudioGetFont(General));*/

	return TRUE;
}

/************************** FIRMWARE UPDATE SCREEN *****************************/

bool_t guiCreateUpdateScreen(void)
{
	GWidgetInit wi;

	gwinWidgetClearInit(&wi);

	// ghPageContainerUpdateScreen
	wi.g.show = FALSE;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = 320;
	wi.g.height = 240;
	wi.g.parent = 0;
	wi.text = "Start";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghPageContainerUpdateScreen = gwinContainerCreate(0, &wi, 0);

	// ghUpdatePicture
	wi.g.show = TRUE;
	wi.g.x = 96;
	wi.g.y = 40;
	wi.g.parent = ghPageContainerUpdateScreen;
	wi.text = "";
	wi.customDraw = 0;
	wi.customParam = 0;
	wi.customStyle = 0;
	ghUpdatePicture = simpleimageCreate(0, &wi, &IMG_USB_PNG);

	// ghUpdateText
	wi.g.show = TRUE;
	wi.g.x = 3;
	wi.g.y = 162;
	wi.g.width = 314;
	wi.g.height = 36;
	wi.g.parent = ghPageContainerUpdateScreen;
	wi.text = GT(TEXT_USBFIRMWARE);
	wi.customDraw = gwinLabelDrawJustifiedCenterPM;
	wi.customParam = 0;
	wi.customStyle = &InfoStyle;
	ghUpdateText = gwinLabelCreate(0, &wi);
	gwinSetFont(ghUpdateText, gstudioGetFont(GeneralSmall));

	return TRUE;
}


