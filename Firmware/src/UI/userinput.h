/* Copyright (C) 2017 - 2019, Attila Kovács
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

#ifndef SRC_UI_USERINPUT_H_
#define SRC_UI_USERINPUT_H_

#include "gfx.h"

#define DEVBUTTON_UP_OFFS	100
#define DEVBUTTON_LONG_OFFS	200
#define DEVBUTTON_DOWN_OFFS	300

typedef enum {
	SD_ROT		= 0,
// Only buttons from here
	SD_ROTSW	= 1, // Must stay first button
	SD_PRESET1	= 2,
	SD_PRESET2	= 3,
	SD_PRESET3	= 4,
	SD_TOOL2	= 5,
	SD_TOOL1	= 6,  // Must stay last button
} Devices_t;



typedef enum {
	SE_NONE,
	SE_ROT_INC,
	SE_ROT_DEC,
	SE_ROT_IINC,
	SE_ROT_DDEC,
	SE_ROT_IIINC,
	SE_ROT_DDDEC,
// Button events must stay in same order
	SE_ROTSW_UP			= SD_ROTSW + DEVBUTTON_UP_OFFS,
	SE_ROTSW_LONG		= SD_ROTSW + DEVBUTTON_LONG_OFFS,
	SE_ROTSW_DOWN		= SD_ROTSW + DEVBUTTON_DOWN_OFFS,
	SE_PRESET1_UP		= SD_PRESET1 + DEVBUTTON_UP_OFFS,
	SE_PRESET1_LONG		= SD_PRESET1 + DEVBUTTON_LONG_OFFS,
	SE_PRESET1_DOWN		= SD_PRESET1 + DEVBUTTON_DOWN_OFFS,
	SE_PRESET2_UP		= SD_PRESET2 + DEVBUTTON_UP_OFFS,
	SE_PRESET2_LONG		= SD_PRESET2 + DEVBUTTON_LONG_OFFS,
	SE_PRESET2_DOWN		= SD_PRESET2 + DEVBUTTON_DOWN_OFFS,
	SE_PRESET3_UP		= SD_PRESET3 + DEVBUTTON_UP_OFFS,
	SE_PRESET3_LONG		= SD_PRESET3 + DEVBUTTON_LONG_OFFS,
	SE_PRESET3_DOWN		= SD_PRESET3 + DEVBUTTON_DOWN_OFFS,
	SE_TOOL2_UP			= SD_TOOL2 + DEVBUTTON_UP_OFFS,
	SE_TOOL2_LONG		= SD_TOOL2 + DEVBUTTON_LONG_OFFS,
	SE_TOOL2_DOWN		= SD_TOOL2 + DEVBUTTON_DOWN_OFFS,
	SE_TOOL1_UP			= SD_TOOL1 + DEVBUTTON_UP_OFFS,
	SE_TOOL1_LONG		= SD_TOOL1 + DEVBUTTON_LONG_OFFS,
	SE_TOOL1_DOWN		= SD_TOOL1 + DEVBUTTON_DOWN_OFFS,
} StationEvent_t;

#define UIDEV_LONGPRESS	1000
#define UIDEVFLAG_USED	0x00000001

StationEvent_t uiGetStationEvent(void);

void uiStationEventInit(void);

#endif /* SRC_UI_USERINPUT_H_ */
