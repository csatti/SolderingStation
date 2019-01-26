/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"


#include "../../../src/ginput/ginput_driver_toggle.h"

GINPUT_TOGGLE_DECLARE_STRUCTURE();

void ginput_lld_toggle_init(const GToggleConfig *ptc) {
	init_board_toggle();
}

unsigned ginput_lld_toggle_getbits(const GToggleConfig *ptc) {
	return (((GPIO_TypeDef *)ptc->id)->IDR & 0x0000FFFF);
}

