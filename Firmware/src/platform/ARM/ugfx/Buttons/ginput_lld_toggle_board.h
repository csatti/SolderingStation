/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_TOGGLE_BOARD_H
#define _GDISP_LLD_TOGGLE_BOARD_H

#include "arch.h"
#include "gfx.h"

#define GINPUT_TOGGLE_NUM_PORTS			6				// The total number of toggle inputs
#define GINPUT_TOGGLE_CONFIG_ENTRIES	2				// The total number of GToggleConfig entries

// Numbering should follow physical layout (first config entry first, lowest bit input first)
#define GINPUT_TOGGLE_SW1_CANCEL_TEMP1	5				// Switch 1 / Cancel / Temperature 1
#define GINPUT_TOGGLE_SW2_MOD_TEMP2		0				// Switch 2 / Modify / Temperature 2
#define GINPUT_TOGGLE_SW3_OK_TEMP3		1				// Switch 3 / Ok / Temperature 3
#define GINPUT_TOGGLE_SW4_TOOL2			3				// Switch 4 / Tool 2
#define GINPUT_TOGGLE_SW5_TOOL1			4				// Switch 5 / Tool 1
#define GINPUT_TOGGLE_CENTER_OK			2				// Center / Ok

#define TOGGLE_PORT_1           		GPIOB
#define TOGGLE_SW2 	  					LL_GPIO_PIN_3
#define TOGGLE_SW3 	  					LL_GPIO_PIN_4
#define TOGGLE_CENTER 	  				LL_GPIO_PIN_5
#define TOGGLE_SW4 	  					LL_GPIO_PIN_8
#define TOGGLE_SW5 	  					LL_GPIO_PIN_9
#define TOGGLE_PORT_2           		GPIOC
#define TOGGLE_SW1 	  					LL_GPIO_PIN_10



#define GINPUT_TOGGLE_DECLARE_STRUCTURE()											\
	const GToggleConfig GInputToggleConfigTable[GINPUT_TOGGLE_CONFIG_ENTRIES] = {	\
		{TOGGLE_PORT_1,																\
			TOGGLE_SW2|TOGGLE_SW3|TOGGLE_CENTER|TOGGLE_SW4|TOGGLE_SW5,				\
			TOGGLE_SW2|TOGGLE_SW3|TOGGLE_CENTER|TOGGLE_SW4|TOGGLE_SW5,				\
			0},																		\
		{TOGGLE_PORT_2,																\
			TOGGLE_SW1,																\
			TOGGLE_SW1,																\
			0},																		\
	}

static GFXINLINE void init_board_toggle(void) {
	// Enable port clocks here if not enabled yet

	// SW1
	LL_GPIO_SetPinMode(TOGGLE_PORT_2, TOGGLE_SW1, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(TOGGLE_PORT_2, TOGGLE_SW1, LL_GPIO_PULL_UP);
	// SW2
	LL_GPIO_SetPinMode(TOGGLE_PORT_1, TOGGLE_SW2, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(TOGGLE_PORT_1, TOGGLE_SW2, LL_GPIO_PULL_UP);
	// SW3
	LL_GPIO_SetPinMode(TOGGLE_PORT_1, TOGGLE_SW3, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(TOGGLE_PORT_1, TOGGLE_SW3, LL_GPIO_PULL_UP);
	// SW4
	LL_GPIO_SetPinMode(TOGGLE_PORT_1, TOGGLE_SW4, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(TOGGLE_PORT_1, TOGGLE_SW4, LL_GPIO_PULL_UP);
	// SW5
	LL_GPIO_SetPinMode(TOGGLE_PORT_1, TOGGLE_SW5, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(TOGGLE_PORT_1, TOGGLE_SW5, LL_GPIO_PULL_UP);
	// CENTER
	LL_GPIO_SetPinMode(TOGGLE_PORT_1, TOGGLE_CENTER, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(TOGGLE_PORT_1, TOGGLE_CENTER, LL_GPIO_PULL_UP);

}

#endif /* _GDISP_LLD_TOGGLE_BOARD_H */
