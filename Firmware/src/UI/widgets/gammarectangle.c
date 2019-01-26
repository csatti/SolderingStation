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

#include "gammarectangle.h"
#include "src/gwin/gwin_class.h"
#include "resources_manager.h"
#include "localization.h"
#include <string.h>

#define gh2obj ((GammaRectangleObject*)gh)
#define gw2obj ((GammaRectangleObject*)gw)

//  VMT
static const gwidgetVMT gammarectangleVMT = {
	{
		"GammaRectangle",				// The classname
		sizeof(GammaRectangleObject),	// The object size
		_gwidgetDestroy,			// The destroy routine
		_gwidgetRedraw,				// The redraw routine
		0,							// The after-clear routine
	},
	gammarectangleDraw_Default,			// The default drawing routine
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

GHandle gammarectangleGCreate(GDisplay* g, GammaRectangleObject* so, GWidgetInit* pInit)
{
	// Create the base class (the actual widget)
	if (!(so = (GammaRectangleObject*)_gwidgetCreate(g, &so->w, pInit, &gammarectangleVMT))) {
		return 0;
	}

	// Initialize the object struct

	// Set the initial visibility
	gwinSetVisible((GHandle)so, pInit->g.show);

	return (GHandle)so;
}

void gammarectangleDraw_Default(GWidgetObject* gw, void* param)
{
	coord_t	w, h, rw, rh;
	color_t color, border;
	uint16_t i;
	uint32_t val = 0;


	(void)param;

	// Make sure that this is a gammarectangle
	if (gw->g.vmt != (gwinVMT*)&gammarectangleVMT)
		return;

	w = gw->g.width;
	h = gw->g.height;

	rw = w / 9;
	rh = h / 8;

	border = HTML2COLOR(0x00);

	for (i = 0; i < 9; i++)
	{
		color = HTML2COLOR((val << 16) | (val << 8) | val);
		gdispFillArea(gw->g.x + rw * i, gw->g.y, rw, rh*2, color);
		gdispDrawBox(gw->g.x + rw * i, gw->g.y, rw, rh*2, border);

		color = HTML2COLOR(val << 16);
		gdispFillArea(gw->g.x + rw * i, gw->g.y + rh*2, rw, rh, color);
		gdispDrawBox(gw->g.x + rw * i, gw->g.y + rh*2, rw, rh, border);

		color = HTML2COLOR(val << 8);
		gdispFillArea(gw->g.x + rw * i, gw->g.y + rh * 4, rw, rh, color);
		gdispDrawBox(gw->g.x + rw * i, gw->g.y + rh * 4, rw, rh, border);

		color = HTML2COLOR(val);
		gdispFillArea(gw->g.x + rw * i, gw->g.y + rh * 6, rw, rh, color);
		gdispDrawBox(gw->g.x + rw * i, gw->g.y + rh * 6, rw, rh, border);
		val+= 32;
		if (val > 255) val = 255;
	}
	val = 0;
	rw = w / 33;
	for (i = 0; i < 33; i++)
	{
		color = HTML2COLOR(val << 16);
		gdispFillArea(gw->g.x + rw * i, gw->g.y + rh * 3, rw, rh, color);

		color = HTML2COLOR(val << 8);
		gdispFillArea(gw->g.x + rw * i, gw->g.y + rh * 5, rw, rh, color);

		color = HTML2COLOR(val);
		gdispFillArea(gw->g.x + rw * i, gw->g.y + rh * 7, rw, rh, color);
		val+= 8;
		if (val > 255) val = 255;
	}

}

#undef gh2obj
#undef gw2obj
