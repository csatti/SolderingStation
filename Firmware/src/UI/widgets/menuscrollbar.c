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

/* This file is part of SolderingStation.
 */

#include "menuscrollbar.h"
#include "src/gwin/gwin_class.h"
#include "resources_manager.h"
#include "localization.h"
#include "lzg.h"
#include <string.h>
#include "doublebuffer.h"


#define STATENUM 5

#define gh2obj ((MenuScrollBarObject*)gh)
#define gw2obj ((MenuScrollBarObject*)gw)

//  VMT
static const gwidgetVMT menuscrollbarVMT = {
	{
		"MenuScrollBar",				// The classname
		sizeof(MenuScrollBarObject),	// The object size
		_gwidgetDestroy,			// The destroy routine
		_gwidgetRedraw,				// The redraw routine
		0,							// The after-clear routine
	},
	menuscrollbarDraw_Default,			// The default drawing routine
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

GHandle menuscrollbarGCreate(GDisplay* g, MenuScrollBarObject* so, GWidgetInit* pInit, const ImageDesc_t* image)
{
	// Create the base class (the actual widget)
	if (!(so = (MenuScrollBarObject*)_gwidgetCreate(g, &so->w, pInit, &menuscrollbarVMT))) {
		return 0;
	}

	// Initialize the object struct
	so->image = image;
	so->itemNumber = 0;
	so->topItem = 0;
	so->visibleNumber = 1;
	// Override width / height
	if (image) {
		so->w.g.width = image->width;
		so->w.g.height = image->height;
	} else {
		so->w.g.width = 0;
		so->w.g.height = 0;
	}

	// Set the initial visibility
	gwinSetVisible((GHandle)so, pInit->g.show);

	return (GHandle)so;
}

void menuscrollbarDraw_Default(GWidgetObject* gw, void* param)
{
	GDisplay* pixmap;
	pixel_t* surface;
	coord_t	w, h, h2;

	(void)param;

	// Make sure that this is a menuscrollbar
	if (gw->g.vmt != (gwinVMT*)&menuscrollbarVMT)
		return;

	if (!gw2obj->image)
		return;

	w = gw->g.width;
	h = gw->g.height;

	// Create a pixmap and get a pointer to the bits
	pixmap = gdispPixmapCreate(w, h);
	gdispGSetOrientation(pixmap, GDISP_ROTATE_0);    // default orientation
	surface = gdispPixmapGetBits(pixmap);

	if (gw2obj->image->format == IMAGE_RAW) {
		gdispGBlitArea(pixmap, 0, 0, w, h, 0, 0, w, (pixel_t*)gw2obj->image->image);
	} else if (gw2obj->image->format == IMAGE_LZG) {
		LZG_Decode((void*)gw2obj->image->image, gw2obj->image->len, (void*)surface, w * h * sizeof(color_t));
	}

	if (gw2obj->topItem > 0) {
		h2 = ((h - 10) * gw2obj->topItem) / gw2obj->itemNumber;
		gdispGFillArea(pixmap, 5, 5, w - 10, h2, gw->g.bgcolor);
	}

	if (gw2obj->topItem + gw2obj->visibleNumber < gw2obj->itemNumber) {
		h2 = ((h - 10) * (gw2obj->itemNumber - (gw2obj->topItem + gw2obj->visibleNumber))) / gw2obj->itemNumber;
		gdispGFillArea(pixmap, 5, h - (5 + h2), w - 10, h2, gw->g.bgcolor);
	}

	doubleBufferBlitArea(pixmap, gw->g.x, gw->g.y, w, h);
	/*gdispBlitArea(gw->g.x, gw->g.y, w, h, surface);
	gdispPixmapDelete(pixmap);*/
}

void menuscrollbarSetImage(GHandle gh, const ImageDesc_t* image)
{
	const ImageDesc_t* oldval;

	// Verify the handle
	if (gh->vmt != (gwinVMT*)&menuscrollbarVMT)
		return;

	oldval = gh2obj->image;
	gh2obj->image = image;

	// Issue a redraw if the value changed
	if (oldval != image) {
		if (image) {
			gh2obj->w.g.width = image->width;
			gh2obj->w.g.height = image->height;
		} else {
			gh2obj->w.g.width = 0;
			gh2obj->w.g.height = 0;
		}
		_gwinUpdate(gh);
	}

}

void menuscrollbarSetup(GHandle gh, uint16_t totalNumber, uint16_t visibleNumber)
{
	uint16_t oldTotalNumber, oldVisibleNumber;

	// Verify the handle
	if (gh->vmt != (gwinVMT*)&menuscrollbarVMT)
		return;

	oldTotalNumber = gh2obj->itemNumber;
	oldVisibleNumber = gh2obj->visibleNumber;
	gh2obj->itemNumber = totalNumber;
	gh2obj->visibleNumber = visibleNumber;

	// Issue a redraw if the value changed
	if ((oldTotalNumber != totalNumber) || (oldVisibleNumber != visibleNumber)) {
		_gwinUpdate(gh);
	}
}

void menuscrollbarSetTopItem(GHandle gh, uint16_t topItem)
{
	uint16_t oldTopItem;

	// Verify the handle
	if (gh->vmt != (gwinVMT*)&menuscrollbarVMT)
		return;

	oldTopItem = gh2obj->topItem;
	gh2obj->topItem = topItem;

	// Issue a redraw if the value changed
	if (oldTopItem != topItem) {
		_gwinUpdate(gh);
	}
}

#undef gh2obj
#undef gw2obj
