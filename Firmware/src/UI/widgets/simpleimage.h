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

#ifndef UI_WIDGETS_SIMPLEIMAGE_H_
#define UI_WIDGETS_SIMPLEIMAGE_H_

#include "gfx.h"
#include "arch.h"
#include "images.h"

typedef struct SimpleImageObject_t {
	GWidgetObject w;  // Base Class

	const ImageDesc_t* image;

} SimpleImageObject;


GHandle simpleimageGCreate(GDisplay* g, SimpleImageObject* so, GWidgetInit* pInit, const ImageDesc_t* image);
#define simpleimageCreate(so, pI, image) simpleimageGCreate(GDISP, so, pI, image)

void simpleimageDraw_Default(GWidgetObject* gw, void* param);

void simpleimageSetImage(GHandle gh, const ImageDesc_t* image);


#endif /* UI_WIDGETS_SIMPLEIMAGE_H_ */
