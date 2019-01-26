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

#ifndef UI_WIDGETS_TOOLSTATUS_H_
#define UI_WIDGETS_TOOLSTATUS_H_

#include "gfx.h"
#include "localization.h"
#include "tool.h"

#define TSTEXT_COLOR_NONE 			HTML2COLOR(0x000000)
#define TSTEXT_COLOR_DISABLED 		HTML2COLOR(0x000000)
#define TSTEXT_COLOR_OFF 			HTML2COLOR(0xEEEE00)
#define TSTEXT_COLOR_SLEEP 			HTML2COLOR(0x000000)
#define TSTEXT_COLOR_WARMUP			HTML2COLOR(0x000000)
#define TSTEXT_COLOR_READY			HTML2COLOR(0x000000)
#define TSTEXT_COLOR_FAULT 			HTML2COLOR(0xFFFF00)
#define TSTEXT_COLOR_CALIBRATION	HTML2COLOR(0xFF0000)
#define TSTEXT_COLOR_CHANGE			HTML2COLOR(0xEEEE00)
#define TSTEXT_COLOR_SETUP			HTML2COLOR(0x000000)
#define TSTEXT_COLOR_COOLDOWN		HTML2COLOR(0x000000)

typedef enum
{
    TOOL_LEFT,
	TOOL_RIGHT,
} ToolSide_t;

typedef struct ToolStatusObject_t {
	GWidgetObject w;  // Base Class

	ToolSide_t toolSide;
	ToolType_t toolType;
	ToolState_t toolState;
	uint16_t toolTemp;

} ToolStatusObject;


GHandle toolstatusGCreate(GDisplay* g, ToolStatusObject* so, GWidgetInit* pInit, ToolSide_t toolSide);
#define toolstatusCreate(so, pI, toolSide) toolstatusGCreate(GDISP, so, pI, toolSide)

void toolstatusDraw_Default(GWidgetObject* gw, void* param);

void toolstatusSetState(GHandle gh, ToolState_t toolstate);
void toolstatusSetType(GHandle gh, ToolType_t tooltype);
void toolstatusSetTemp(GHandle gh, uint16_t tooltemp);


#endif /* UI_WIDGETS_TOOLSTATUS_H_ */
