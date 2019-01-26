/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"


#include "../../../src/ginput/ginput_driver_dial.h"

void ginput_lld_dial_init(void) {
	init_board_dial();
}

void ginput_lld_dial_poll(DialCallbackFn fn) {

	fn(0, get_rotary_pos());
}

