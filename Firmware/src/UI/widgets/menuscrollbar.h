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

#ifndef UI_WIDGETS_MENUSCROLLBAR_H_
#define UI_WIDGETS_MENUSCROLLBAR_H_

#include "gfx.h"
#include "arch.h"
#include "images.h"

typedef struct MenuScrollBarObject_t {
	GWidgetObject w;  // Base Class

	const ImageDesc_t* image;
	uint16_t itemNumber;
	uint16_t topItem;
	uint16_t visibleNumber;
} MenuScrollBarObject;


GHandle menuscrollbarGCreate(GDisplay* g, MenuScrollBarObject* so, GWidgetInit* pInit, const ImageDesc_t* image);
#define menuscrollbarCreate(so, pI, image) menuscrollbarGCreate(GDISP, so, pI, image)

void menuscrollbarDraw_Default(GWidgetObject* gw, void* param);

void menuscrollbarSetImage(GHandle gh, const ImageDesc_t* image);
void menuscrollbarSetup(GHandle gh, uint16_t totalNumber, uint16_t visibleNumber);
void menuscrollbarSetTopItem(GHandle gh, uint16_t topItem);


#endif /* UI_WIDGETS_MENUSCROLLBAR_H_ */
