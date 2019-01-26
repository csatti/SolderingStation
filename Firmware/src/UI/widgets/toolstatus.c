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

/* This file is part of SolderingStation.
 */

#include "toolstatus.h"
#include "images.h"
#include "src/gwin/gwin_class.h"
#include "resources_manager.h"
#include "lzg.h"
#include <string.h>
#include "doublebuffer.h"

#define STATENUM 11
#define TOOLTYPENUM	3

static const ImageDesc_t* leftImages[STATENUM] = {
		&IMG_WHITE_LT_PNG,// TOOL_NONE
		&IMG_GREY_LT_PNG,	// TOOL_DISABLED
		&IMG_BLUE_LT_PNG,	// TOOL_OFF
		&IMG_YELLOW_LT_PNG,	// TOOL_SLEEP
		&IMG_ORANGE_LT_PNG,	// TOOL_WARMUP
		&IMG_GREEN_LT_PNG,	// TOOL_READY
		&IMG_RED_LT_PNG,	// TOOL_FAULT
		&IMG_YELLOW_LT_PNG,	// TOOL_CALIBRATION
		&IMG_BLUE_LT_PNG,	// TOOL_CHANGE
		&IMG_GREY_LT_PNG,	// TOOL_SETUP
		&IMG_ORANGE_LT_PNG,	// TOOL_COOLDOWN
		};

static const ImageDesc_t* rightImages[STATENUM] = {
		&IMG_WHITE_RT_PNG,// TOOL_NONE
		&IMG_GREY_RT_PNG,	// TOOL_DISABLED
		&IMG_BLUE_RT_PNG,	// TOOL_OFF
		&IMG_YELLOW_RT_PNG,	// TOOL_SLEEP
		&IMG_ORANGE_RT_PNG,	// TOOL_WARMUP
		&IMG_GREEN_RT_PNG,	// TOOL_READY
		&IMG_RED_RT_PNG,	// TOOL_FAULT
		&IMG_YELLOW_RT_PNG,	// TOOL_CALIBRATION
		&IMG_BLUE_RT_PNG,	// TOOL_CHANGE
		&IMG_GREY_RT_PNG,	// TOOL_SETUP
		&IMG_ORANGE_RT_PNG,	// TOOL_COOLDOWN
		};

static const ImageDesc_t* leftToolImages[TOOLTYPENUM] = {
		NULL,				// TOOL_NOTYPE
		&IMG_C210_LT_PNG,	// TOOL_T210
		&IMG_C245_LT_PNG,	// TOOL_T245
		};

static const ImageDesc_t* rightToolImages[TOOLTYPENUM] = {
		NULL,				// TOOL_NOTYPE
		&IMG_C210_RT_PNG,	// TOOL_T210
		&IMG_C245_RT_PNG,	// TOOL_T245
		};

static const color_t colors[STATENUM] = {
		TSTEXT_COLOR_NONE,			// TOOL_OFF
		TSTEXT_COLOR_DISABLED,		// TOOL_SLEEP
		TSTEXT_COLOR_OFF,			// TOOL_OFF
		TSTEXT_COLOR_SLEEP,			// TOOL_SLEEP
		TSTEXT_COLOR_WARMUP,		// TOOL_WARMUP
		TSTEXT_COLOR_READY,			// TOOL_READY
		TSTEXT_COLOR_FAULT,			// TOOL_FAULT
		TSTEXT_COLOR_CALIBRATION,	// TOOL_CALIBRATION
		TSTEXT_COLOR_CHANGE,		// TOOL_CHANGE
		TSTEXT_COLOR_SETUP,			// TOOL_SETUP
		TSTEXT_COLOR_COOLDOWN,		// TOOL_COOLDOWN
		};

static const LOCTEXT texts[STATENUM] = {
		TSTEXT_NONE,		// TOOL_OFF
		TSTEXT_DISABLED,	// TOOL_SLEEP
		TSTEXT_OFF,			// TOOL_OFF
		TSTEXT_SLEEP,		// TOOL_SLEEP
		TSTEXT_WARMUP,		// TOOL_WARMUP
		TSTEXT_READY,		// TOOL_READY
		TSTEXT_FAULT,		// TOOL_FAULT
		TSTEXT_CALIBRATION,	// TOOL_CALIBRATION
		TSTEXT_CHANGE,		// TOOL_CHANGE
		TSTEXT_SETUP,		// TOOL_SETUP
		TSTEXT_COOLDOWN,	// TOOL_COOLDOWN
		};

#define gh2obj ((ToolStatusObject*)gh)
#define gw2obj ((ToolStatusObject*)gw)

//  VMT
static const gwidgetVMT toolstatusVMT = {
		{
			"ToolStatus",				// The classname
			sizeof(ToolStatusObject),	// The object size
			_gwidgetDestroy,			// The destroy routine
			_gwidgetRedraw,				// The redraw routine
			0,							// The after-clear routine
		}, toolstatusDraw_Default,		// The default drawing routine
#if GINPUT_NEED_MOUSE
		{
			0,						// Process mouse down events
			0,// Process mouse up events
			0,// Process mouse move events
		},
#endif
#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
		{
			0						// Process keyboard events
		},
#endif
#if GINPUT_NEED_TOGGLE
		{
			0,						// Toggle role
			0,// Assign Toggles
			0,// Get Toggles
			0,// Process toggle off events
			0,// Process toggle on events
		},
#endif
#if GINPUT_NEED_DIAL
		{
			0,						// No dial roles
			0,// Assign Dials
			0,// Get Dials
			0,// Process dial move events
		},
#endif
	};

GHandle toolstatusGCreate(GDisplay* g, ToolStatusObject* so, GWidgetInit* pInit,
		ToolSide_t toolSide)
{
	// Create the base class (the actual widget)
	if (!(so = (ToolStatusObject*) _gwidgetCreate(g, &so->w, pInit,
			&toolstatusVMT))) {
		return 0;
	}

	// Initialize the object struct
	so->toolSide = toolSide;
	so->toolState = TOOL_NONE;
	so->toolTemp = 0;
	so->toolType = TOOL_NOTYPE;
	// Override width / height
	so->w.g.width = leftImages[0]->width;
	so->w.g.height = leftImages[0]->height;

	// Set the initial visibility
	gwinSetVisible((GHandle) so, pInit->g.show);

	return (GHandle) so;
}

void toolstatusDraw_Default(GWidgetObject* gw, void* param)
{
	color_t color;
	const ImageDesc_t* backImage;
	const ImageDesc_t* toolImage;
	GDisplay* pixmap;
	pixel_t* surface;
	coord_t w, h, tix;
	char str[20];
	const char* text;
	justify_t justify;
	font_t tempFont = gstudioGetFont(SmallTemp);
	uint8_t even;
	const uint16_t fixWidth = 17;

	(void) param;

	// Make sure that this is a toolstatus
	if (gw->g.vmt != (gwinVMT*) &toolstatusVMT)
		return;

	w = gw->g.width;
	h = gw->g.height;

	color = colors[gw2obj->toolState];
	if (gw2obj->toolSide == TOOL_LEFT) {
		backImage = leftImages[gw2obj->toolState];
		toolImage = leftToolImages[gw2obj->toolType];
		if (toolImage != NULL) {
			tix = w - toolImage->width;
		}
		justify = justifyLeft;
	} else {
		backImage = rightImages[gw2obj->toolState];
		toolImage = rightToolImages[gw2obj->toolType];
		justify = justifyRight;
		tix = 0;
	}
	text = GT(texts[gw2obj->toolState]);

	even = 1;

	if ((gw2obj->toolState == TOOL_DISABLED) || (gw2obj->toolState == TOOL_NONE)
			|| (gw2obj->toolState == TOOL_SETUP) || (gw2obj->toolState == TOOL_CALIBRATION)) {
		str[0] = 0;
	} else {
		uint32_t temp = gw2obj->toolTemp;
		if ((temp) && (gw2obj->toolState != TOOL_OFF)) {
			ltoa(temp, str, 10);
		} else {
			strcpy(str, GT(TSTEXT_COLD));
			tempFont = gw->g.font;
			even = 0;
		}
	}

	// Create a pixmap and get a pointer to the bits
	pixmap = gdispPixmapCreate(w, h);
	gdispGSetOrientation(pixmap, GDISP_ROTATE_0);    // default orientation
	surface = gdispPixmapGetBits(pixmap);

	if (backImage->format == IMAGE_RAW) {
		gdispGBlitArea(pixmap, 0, 0, w, h, 0, 0, w,
				(pixel_t*) backImage->image);
	} else if (backImage->format == IMAGE_LZG) {
		LZG_Decode((void*) backImage->image, backImage->len, (void*) surface,
				w * h * sizeof(color_t));
	}

	if (toolImage != NULL) {
		coord_t wt = toolImage->width;
		coord_t ht = toolImage->height;

		if (toolImage->format == IMAGE_RAW) {
			gdispGBlitArea(pixmap, tix, 35, wt, ht, 0, 0, wt,
					(pixel_t*) toolImage->image);
		} else if (toolImage->format == IMAGE_LZG) {
			pixel_t* surfaceTool;
			GDisplay* pixmapTool;

			// Create a pixmap and get a pointer to the bits
			pixmapTool = gdispPixmapCreate(wt, ht);
			gdispGSetOrientation(pixmapTool, GDISP_ROTATE_0); // default orientation
			surfaceTool = gdispPixmapGetBits(pixmapTool);
			LZG_Decode((void*) toolImage->image, toolImage->len,
					(void*) surfaceTool, wt * ht * sizeof(color_t));
			gdispGBlitArea(pixmap, tix, 35, wt, ht, 0, 0, wt, surfaceTool);
			gdispPixmapDelete(pixmapTool);
		}
	}

	gdispGDrawStringBox(pixmap, 7, 3, w - 14, (h - 5) / 2, text, gw->g.font,
			color, justify);

	if (even) {
		char s[2];
		s[1] = 0;
		int x = 7;
		if (justify == justifyRight) x = w - (fixWidth * strlen(str) + 7);

		for (int i = 0; i < strlen(str); i++)
		{
			s[0] = str[i];
			gdispGDrawStringBox(pixmap, x, 3 + (h - 5) / 2, fixWidth, (h - 5) / 2, s,
				tempFont, color,  justifyNoPad | justifyCenter);
			/*gdispGDrawString(pixmap, x, 3 + (h - 5) / 2, s,
							tempFont, color);*/
			x += fixWidth;
		}
	} else {
		gdispGDrawStringBox(pixmap, 7, 3 + (h - 5) / 2, w - 14, (h - 5) / 2, str,
			tempFont, color, justify);
	}

	doubleBufferBlitArea(pixmap, gw->g.x, gw->g.y, w, h);
	/*gdispBlitArea(gw->g.x, gw->g.y, w, h, surface);
	gdispPixmapDelete(pixmap);*/
}

void toolstatusSetState(GHandle gh, ToolState_t toolstate)
{
	ToolState_t oldval;

	// Verify the handle
	if (gh->vmt != (gwinVMT*) &toolstatusVMT)
		return;

	oldval = gh2obj->toolState;
	gh2obj->toolState = toolstate;

	// Issue a redraw if the value changed
	if (oldval != toolstate)
		_gwinUpdate(gh);
}

void toolstatusSetType(GHandle gh, ToolType_t tooltype)
{
	ToolType_t oldval;

	// Verify the handle
	if (gh->vmt != (gwinVMT*) &toolstatusVMT)
		return;

	oldval = gh2obj->toolType;
	gh2obj->toolType = tooltype;

	// Issue a redraw if the value changed
	if (oldval != tooltype)
		_gwinUpdate(gh);
}

void toolstatusSetTemp(GHandle gh, uint16_t tooltemp)
{
	uint16_t oldval;

	// Verify the handle
	if (gh->vmt != (gwinVMT*) &toolstatusVMT)
		return;

	oldval = gh2obj->toolTemp;
	gh2obj->toolTemp = tooltemp;

	// Issue a redraw if the value changed
	if (oldval != tooltemp)
		_gwinUpdate(gh);
}

#undef gh2obj
#undef gw2obj
