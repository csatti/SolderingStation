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

#include "statusbar.h"
#include "images.h"
#include "src/gwin/gwin_class.h"
#include "resources_manager.h"
#include "lzg.h"
#include <string.h>
#include "config.h"
#include "doublebuffer.h"


#define STATENUM 5

static const ImageDesc_t* background = &IMG_BACK_MT_PNG;

#define gh2obj ((StatusBarObject*)gh)
#define gw2obj ((StatusBarObject*)gw)

//  VMT
static const gwidgetVMT statusbarVMT = {
	{
		"StatusBar",				// The classname
		sizeof(StatusBarObject),	// The object size
		_gwidgetDestroy,			// The destroy routine
		_gwidgetRedraw,				// The redraw routine
		0,							// The after-clear routine
	},
	statusbarDraw_Default,			// The default drawing routine
	#if GINPUT_NEED_MOUSE
		{
			0,						// Process mouse down events
			0,						// Process mouse up events
			0,						// Process mouse move events
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
			0,						// Assign Toggles
			0,						// Get Toggles
			0,						// Process toggle off events
			0,						// Process toggle on events
		},
	#endif
	#if GINPUT_NEED_DIAL
		{
			0,						// No dial roles
			0,						// Assign Dials
			0,						// Get Dials
			0,						// Process dial move events
		},
	#endif
};

GHandle statusbarGCreate(GDisplay* g, StatusBarObject* so, GWidgetInit* pInit)
{
	// Create the base class (the actual widget)
	if (!(so = (StatusBarObject*)_gwidgetCreate(g, &so->w, pInit, &statusbarVMT))) {
		return 0;
	}

	// Initialize the object struct
	so->airTemp = 0;
	so->icons = 0;
	// Override width / height
	so->w.g.width = background->width;
	so->w.g.height = background->height;

	// Set the initial visibility
	gwinSetVisible((GHandle)so, pInit->g.show);

	return (GHandle)so;
}

static uint16_t _paintIcon(const ImageDesc_t* icon, GDisplay *g, coord_t x, coord_t y)
{
	if (icon != NULL) {
		coord_t wt = icon->width;
		coord_t ht = icon->height;

		if (icon->format == IMAGE_RAW) {
			gdispGBlitArea(g, x, y, wt, ht, 0, 0, wt, (pixel_t*) icon->image);
		} else if (icon->format == IMAGE_LZG) {
			pixel_t* surfaceIcon;
			GDisplay* pixmapIcon;

			// Create a pixmap and get a pointer to the bits
			pixmapIcon = gdispPixmapCreate(wt, ht);
			gdispGSetOrientation(pixmapIcon, GDISP_ROTATE_0); // default orientation
			surfaceIcon = gdispPixmapGetBits(pixmapIcon);
			LZG_Decode((void*) icon->image, icon->len,
					(void*) surfaceIcon, wt * ht * sizeof(color_t));
			gdispGBlitArea(g, x, y, wt, ht, 0, 0, wt, surfaceIcon);
			gdispPixmapDelete(pixmapIcon);
			return wt;
		}
	}

	return 0;
}



void statusbarDraw_Default(GWidgetObject* gw, void* param)
{
	GDisplay* pixmap;
	pixel_t* surface;
	coord_t	w, h;
	char str[20];

	(void)param;

	// Make sure that this is a statusbar
	if (gw->g.vmt != (gwinVMT*)&statusbarVMT)
		return;


	uint32_t temp = gw2obj->airTemp;
	ltoa(temp, str, 10);

	ConfigOnline_t* ConfigOnline = configGetConfigOnlineNoWrite();
	const char* unitText = (ConfigOnline->tempUnitC ? GT(UNITTEXT_CELSIUS) : GT(UNITTEXT_FAHRENHEIT));
	strcat(str, unitText);

	w = gw->g.width;
	h = gw->g.height;

	// Create a pixmap and get a pointer to the bits
	pixmap = gdispPixmapCreate(w, h);
	gdispGSetOrientation(pixmap, GDISP_ROTATE_0);    // default orientation
	surface = gdispPixmapGetBits(pixmap);

	if (background->format == IMAGE_RAW) {
		gdispGBlitArea(pixmap, 0, 0, w, h, 0, 0, w, (pixel_t*)background->image);
	} else if (background->format == IMAGE_LZG) {
		LZG_Decode((void*)background->image, background->len, (void*)surface, w * h * sizeof(color_t));
	}

	if (gw2obj->icons != 0) {
		uint16_t xi = 15;
		if (gw2obj->icons & SB_ICON_THERMAL)
			xi += _paintIcon(&IMG_ICON_THERMAL_PNG, pixmap, xi, 8);
		if (gw2obj->icons & SB_ICON_SOUNDOFF)
			xi += _paintIcon(&IMG_ICON_SOUNDOFF_PNG, pixmap, xi, 8);
	}

	gdispGDrawStringBox(pixmap, 9, 3, w - 18, h - 5, str, gw->g.font, gw->g.color, justifyRight);

	/*gdispBlitArea(gw->g.x, gw->g.y, w, h, surface);
	gdispPixmapDelete(pixmap);*/
	doubleBufferBlitArea(pixmap, gw->g.x, gw->g.y, w, h);
}

void statusbarSetIcons(GHandle gh, uint32_t icons)
{
	uint32_t oldval;

	// Verify the handle
	if (gh->vmt != (gwinVMT*)&statusbarVMT)
		return;

	oldval = gh2obj->icons;
	gh2obj->icons = icons;

	// Issue a redraw if the value changed
	if (oldval != icons)
		_gwinUpdate(gh);
}

uint32_t statusbarGetIcons(GHandle gh)
{
	// Verify the handle
	if (gh->vmt != (gwinVMT*)&statusbarVMT)
		return 0;
	return gh2obj->icons;
}

void statusbarSetTemp(GHandle gh, uint16_t airTemp)
{
	uint16_t oldval;

	// Verify the handle
	if (gh->vmt != (gwinVMT*)&statusbarVMT)
		return;

	oldval = gh2obj->airTemp;
	gh2obj->airTemp = airTemp;

	// Issue a redraw if the value changed
	if (oldval != airTemp)
		_gwinUpdate(gh);
}

#undef gh2obj
#undef gw2obj
