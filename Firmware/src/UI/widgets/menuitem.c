/* Copyright (C) 2018, Attila Kov�cs
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

#include "menuitem.h"
#include "src/gwin/gwin_class.h"
#include "resources_manager.h"
#include "localization.h"
#include "lzg.h"
#include <string.h>
#include "doublebuffer.h"

#define STATENUM 5

#define gh2obj ((MenuItemObject*)gh)
#define gw2obj ((MenuItemObject*)gw)

//  VMT
static const gwidgetVMT menuitemVMT = {
	{
		"MenuItem",				// The classname
		sizeof(MenuItemObject),	// The object size
		_gwidgetDestroy,			// The destroy routine
		_gwidgetRedraw,				// The redraw routine
		0,							// The after-clear routine
	},
	menuitemDraw_Default,			// The default drawing routine
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

GHandle menuitemGCreate(GDisplay* g, MenuItemObject* so, GWidgetInit* pInit, const ImageDesc_t* image)
{
	// Create the base class (the actual widget)
	if (!(so = (MenuItemObject*)_gwidgetCreate(g, &so->w, pInit, &menuitemVMT))) {
		return 0;
	}

	// Initialize the object struct
	so->image = image;
	so->selected = 0;
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

void menuitemDraw_Default(GWidgetObject* gw, void* param)
{
	GDisplay* pixmap;
	pixel_t* surface;
	coord_t	w, h;

	(void)param;

	// Make sure that this is a menuitem
	if (gw->g.vmt != (gwinVMT*)&menuitemVMT)
		return;

	if (!gw2obj->image)
		return;

	w = gw->g.width;
	h = gw->g.height;

	// Create a pixmap and get a pointer to the bits
	pixmap = gdispPixmapCreate(w, h);
	gdispGSetOrientation(pixmap, GDISP_ROTATE_0);    // default orientation
	surface = gdispPixmapGetBits(pixmap);

	if (gw2obj->selected) {
		if (gw2obj->image->format == IMAGE_RAW) {
			gdispGBlitArea(pixmap, 0, 0, w, h, 0, 0, w, (pixel_t*)gw2obj->image->image);
		} else if (gw2obj->image->format == IMAGE_LZG) {
			LZG_Decode((void*)gw2obj->image->image, gw2obj->image->len, (void*)surface, w * h * sizeof(color_t));
		}
	} else {
		gdispGFillArea(pixmap,0, 0, w, h, gw->g.bgcolor);
	}

	if (gw->text)
		gdispGDrawStringBox(pixmap, 0, 0, w, h, gw->text, gw->g.font, gw->g.color, justifyCenter);

	doubleBufferBlitArea(pixmap, gw->g.x, gw->g.y, w, h);
	/*gdispBlitArea(gw->g.x, gw->g.y, w, h, surface);
	gdispPixmapDelete(pixmap);*/
}

void menuitemSetImage(GHandle gh, const ImageDesc_t* image)
{
	const ImageDesc_t* oldval;

	// Verify the handle
	if (gh->vmt != (gwinVMT*)&menuitemVMT)
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

void menuitemSetSelected(GHandle gh, uint8_t selected)
{
	uint8_t oldval;

	// Verify the handle
	if (gh->vmt != (gwinVMT*)&menuitemVMT)
		return;

	oldval = gh2obj->selected;
	gh2obj->selected = selected;

	// Issue a redraw if the value changed
	if (oldval != selected) {
		_gwinUpdate(gh);
	}
}

#undef gh2obj
#undef gw2obj
