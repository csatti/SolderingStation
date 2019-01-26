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

#ifndef UI_WIDGETS_STATUSBAR_H_
#define UI_WIDGETS_STATUSBAR_H_

#include "gfx.h"
#include "localization.h"

#define SB_ICON_THERMAL		0x00000001
#define SB_ICON_SOUNDOFF	0x00000002
#define SB_INFO_SHOWTEMP	0x00010000

typedef struct StatusBarObject_t {
	GWidgetObject w;  // Base Class

	uint32_t icons;
	uint16_t airTemp;

} StatusBarObject;


GHandle statusbarGCreate(GDisplay* g, StatusBarObject* so, GWidgetInit* pInit);
#define statusbarCreate(so, pI) statusbarGCreate(GDISP, so, pI)

void statusbarDraw_Default(GWidgetObject* gw, void* param);

void statusbarSetIcons(GHandle gh, uint32_t icons);
uint32_t statusbarGetIcons(GHandle gh);
void statusbarSetTemp(GHandle gh, uint16_t airTemp);


#endif /* UI_WIDGETS_STATUSBAR_H_ */
