/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GINPUT_LLD_DIAL_BOARD_H
#define _GINPUT_LLD_DIAL_BOARD_H

#include "arch.h"
#include "gfx.h"	// IDE can't find declarations otherwise

#define GINPUT_DIAL_NUM_PORTS			1
#define GINPUT_DIAL_POLL_PERIOD			100

// Rotary encoder
#define ROTARY           		TIM4
#define ROTARY_PERIPH           LL_APB1_GRP1_PERIPH_TIM4
#define ROTARY_PORT             GPIOB
#define ROTARY_A 	  			LL_GPIO_PIN_6  // CH1
#define ROTARY_B    	        LL_GPIO_PIN_7  // CH2

static GFXINLINE void init_board_dial(void) {
	LL_TIM_InitTypeDef TIM_TimeBaseStructure;

	LL_APB1_GRP1_EnableClock(ROTARY_PERIPH);
	// Enable port clocks here if not enabled yet

	// A
	LL_GPIO_SetPinMode(ROTARY_PORT, ROTARY_A, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(ROTARY_PORT, ROTARY_A, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(ROTARY_PORT, ROTARY_A, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(ROTARY_PORT, ROTARY_A, LL_GPIO_PULL_NO);
	LL_GPIO_SetAFPin_0_7(ROTARY_PORT, ROTARY_A, LL_GPIO_AF_2);
	// B
	LL_GPIO_SetPinMode(ROTARY_PORT, ROTARY_B, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(ROTARY_PORT, ROTARY_B, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(ROTARY_PORT, ROTARY_B, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(ROTARY_PORT, ROTARY_B, LL_GPIO_PULL_NO);
	LL_GPIO_SetAFPin_0_7(ROTARY_PORT, ROTARY_B, LL_GPIO_AF_2);

	// Timer configuration
	LL_TIM_StructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.Prescaler = 0;
	TIM_TimeBaseStructure.Autoreload = 0xFFFF;
	TIM_TimeBaseStructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	TIM_TimeBaseStructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_TimeBaseStructure.RepetitionCounter = 0;

	LL_TIM_Init(ROTARY, &TIM_TimeBaseStructure);

	// Rotary encoder configuration
	LL_TIM_SetEncoderMode   (ROTARY, LL_TIM_ENCODERMODE_X2_TI1);
	LL_TIM_IC_SetActiveInput(ROTARY, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
	LL_TIM_IC_SetActiveInput(ROTARY, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
	LL_TIM_IC_SetPolarity   (ROTARY, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
	LL_TIM_IC_SetPolarity   (ROTARY, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);

	// Enable timer
	LL_TIM_EnableCounter(ROTARY);

}

static GFXINLINE uint16_t get_rotary_pos(void)
{
	return ROTARY->CNT;
}

#endif	/* _GINPUT_LLD_DIAL_BOARD_H */
