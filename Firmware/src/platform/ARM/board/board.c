/* Copyright (C) 2017, Attila Kovács
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

#include "board.h"

#include "nvic.h"
//#include "analog.h"

// Backup

void boardBackupInit(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	LL_PWR_EnableBkUpAccess();
}

void boardBackupDeInit(void)
{
	LL_PWR_DisableBkUpAccess();

	LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
}


// System clock

uint32_t boardRCCClockInit(void) {
	uint32_t counter = 0;
	uint32_t hseStatus, lseStatus;

	LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
	if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5)
		Error_Handler();

	//NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUPING);
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	// Enable HSE
	LL_RCC_HSE_Enable();

	do {
		hseStatus = LL_RCC_HSE_IsReady();
	} while ((counter++ != 0xf50000) && !hseStatus);

	if (hseStatus) {
		// For 12MHz crystal
		//LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_10, 280, LL_RCC_PLLP_DIV_2);
		//LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_10, 280, LL_RCC_PLLQ_DIV_7);
		// For 8MHz crystal
		LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4,
				168, LL_RCC_PLLP_DIV_2);
		LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4,
				168, LL_RCC_PLLQ_DIV_7);

		LL_RCC_PLL_Enable();

		/* Wait till PLL is ready */
		while (LL_RCC_PLL_IsReady() != 1) {
			__NOP();
		}

		LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
		LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
		LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
		LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

		/* Wait till System clock is ready */
		while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
			__NOP();
		}

		LL_SetSystemCoreClock(168000000);
		LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_HSE, LL_RCC_MCO1_DIV_1);
	}

	LL_PWR_EnableBkUpAccess();
	LL_RCC_ForceBackupDomainReset();
	LL_RCC_ReleaseBackupDomainReset();
	// Enable LSE
	LL_RCC_LSE_Enable();

	counter = 0;
	do {
		lseStatus = LL_RCC_LSE_IsReady();
	} while ((counter++ != 0xf5000) && !lseStatus);

	if (lseStatus) {
		LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
		LL_RCC_EnableRTC();
	}

	//TODO: Move this to RTOS init
	//LL_Init1msTick(168000000);
	//LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	//NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_8, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_8, LL_GPIO_PULL_NO);
	LL_GPIO_SetAFPin_8_15(GPIOA, LL_GPIO_PIN_8, LL_GPIO_AF_0);
	LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_HSE, LL_RCC_MCO1_DIV_1);

	// Enable ART accelerator
	FLASH->ACR |= FLASH_ACR_ICEN;
	FLASH->ACR |= FLASH_ACR_PRFTEN;
	FLASH->ACR |= FLASH_ACR_DCEN;

	return (hseStatus && lseStatus);
}

// SW-DP, JTAG config

void boardDPIntfConfig(void) {
	//DBG_Remap();
}

void boardGPIOConfig(void) {
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
	//LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE); // 100
	//LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF); // 144
	//LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG); // 144
}

#ifdef USARTDEBUG
// DEBUG - USART
void boardUSARTDebugConfig(uint32_t bufferAddr) {
	//LL_GPIO_InitTypeDef GPIO_InitStruct;
	LL_USART_InitTypeDef USART_InitStruct;
	LL_DMA_InitTypeDef DMA_InitStruct;

	LL_APB2_GRP1_EnableClock(USART_DEBUG_PERIPH);

	// PA.09(Tx)
	LL_GPIO_SetPinMode(USART_DEBUG_GPIO, USART_DEBUG_TxPin,
			LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(USART_DEBUG_GPIO, USART_DEBUG_TxPin,
			LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinOutputType(USART_DEBUG_GPIO, USART_DEBUG_TxPin,
			LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(USART_DEBUG_GPIO, USART_DEBUG_TxPin, LL_GPIO_PULL_UP);
	USART_DEBUG_TxPin_SetAF();

	// PA.10(Rx)
	LL_GPIO_SetPinMode(USART_DEBUG_GPIO, USART_DEBUG_RxPin,
			LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(USART_DEBUG_GPIO, USART_DEBUG_RxPin,
			LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinOutputType(USART_DEBUG_GPIO, USART_DEBUG_RxPin,
			LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(USART_DEBUG_GPIO, USART_DEBUG_RxPin, LL_GPIO_PULL_UP);
	USART_DEBUG_RxPin_SetAF();

	// Enable the DMA Clock
	LL_AHB1_GRP1_EnableClock(USART_DEBUG_DMA_PERIPH);

	// Configure transmit DMA
	LL_DMA_DeInit(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream);
	LL_DMA_StructInit(&DMA_InitStruct);
	DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t) &USART_DEBUG->DR; //USART_DEBUG_DR_Base;
	DMA_InitStruct.MemoryOrM2MDstAddress = bufferAddr;
	DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
	DMA_InitStruct.NbData = 0;
	DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
	DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
	DMA_InitStruct.Mode = LL_DMA_MODE_NORMAL;
	DMA_InitStruct.Priority = LL_DMA_PRIORITY_LOW;
	DMA_InitStruct.Channel = USART_DEBUG_Tx_DMA_Channel;
	DMA_InitStruct.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
	DMA_InitStruct.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_1_4;
	DMA_InitStruct.MemBurst = LL_DMA_MBURST_SINGLE;
	DMA_InitStruct.PeriphBurst = LL_DMA_PBURST_SINGLE;
	LL_DMA_Init(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream, &DMA_InitStruct);
	LL_DMA_EnableIT_TC(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream);

	nvicUSARTDebugDMAConfiguration();
	LL_USART_StructInit(&USART_InitStruct);
	USART_InitStruct.BaudRate = USART_DEBUG_BAUDRATE;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_8;
	LL_USART_Init(USART_DEBUG, &USART_InitStruct);
	LL_USART_ConfigAsyncMode(USART_DEBUG);
	LL_USART_EnableDMAReq_TX(USART_DEBUG);
	LL_USART_EnableIT_RXNE(USART_DEBUG);
	nvicUSARTDebugConfiguration();
	LL_USART_Enable(USART_DEBUG);
}

#endif

void boardRTOSTimerConfig(void)
{
	LL_TIM_InitTypeDef TIM_TimeBaseStructure;

	nvicRTOSTimerConfiguration();

	LL_APB1_GRP1_EnableClock(TIMER_RTOS_PERIPH);
	LL_DBGMCU_APB1_GRP1_FreezePeriph(TIMER_RTOS_PERIPH_STOP);

	LL_TIM_StructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.Autoreload = 0x1;
	TIM_TimeBaseStructure.Prescaler = 0x1067;
	TIM_TimeBaseStructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	TIM_TimeBaseStructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	LL_TIM_Init(TIMER_RTOS, &TIM_TimeBaseStructure);

	LL_TIM_EnableIT_UPDATE(TIMER_RTOS);
	// Enable timer
	LL_TIM_EnableCounter(TIMER_RTOS);

}

// Heating
void boardHeatingConfig(void)
{

	// Enable pin configuration
	LL_GPIO_SetPinMode(HEATING_GPIO, HEATING_Enable_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(HEATING_GPIO, HEATING_Enable_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(HEATING_GPIO, HEATING_Enable_Pin, LL_GPIO_OUTPUT_PUSHPULL);
	HEATING_Enable_CLR();

	// Select tool pin configuration
	LL_GPIO_SetPinMode(HEATING_GPIO, HEATING_SelTool_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(HEATING_GPIO, HEATING_SelTool_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(HEATING_GPIO, HEATING_SelTool_Pin, LL_GPIO_OUTPUT_PUSHPULL);
	HEATING_SelTool_SET();

	// Heating pin configuration
	LL_GPIO_SetPinMode(HEATING_GPIO, HEATING_Heating_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(HEATING_GPIO, HEATING_Heating_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(HEATING_GPIO, HEATING_Heating_Pin, LL_GPIO_OUTPUT_PUSHPULL);
	HEATING_Heating_OFF();
}

// Detection
void boardDetectConfig(void)
{
	LL_APB2_GRP1_EnableClock(DETECT_EXTI_PERIPH);

	// Zero cross detection pin configuration
	LL_GPIO_SetPinMode(DETECT_GPIO, DETECT_ZeroCross_Pin, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(DETECT_GPIO, DETECT_ZeroCross_Pin, LL_GPIO_PULL_NO);
	LL_EXTI_EnableIT_0_31(DETECT_ZeroCross_EXTI_LINE);
	LL_EXTI_EnableFallingTrig_0_31(DETECT_ZeroCross_EXTI_LINE);
	LL_EXTI_EnableRisingTrig_0_31(DETECT_ZeroCross_EXTI_LINE);
	LL_SYSCFG_SetEXTISource(DETECT_EXTI_SOURCE, DETECT_ZeroCross_SYSCFG_LINE);

	// Tool presence is now deprecated as it caused issues with temperature measurement
	// Tool presence detection pin configuration
	/*LL_GPIO_SetPinMode(DETECT_GPIO, DETECT_ToolPresence_Pin, LL_GPIO_MODE_INPUT);
	LL_GPIO_SetPinPull(DETECT_GPIO, DETECT_ToolPresence_Pin, LL_GPIO_PULL_NO);
	LL_EXTI_EnableIT_0_31(DETECT_ToolPresence_EXTI_LINE);
	LL_EXTI_EnableFallingTrig_0_31(DETECT_ToolPresence_EXTI_LINE);
	LL_EXTI_EnableRisingTrig_0_31(DETECT_ToolPresence_EXTI_LINE);
	LL_SYSCFG_SetEXTISource(DETECT_EXTI_SOURCE, DETECT_ToolPresence_SYSCFG_LINE);*/

	nvicDetectConfiguration();

}

// Control timer
void boardCTRLTimerConfig(void)
{
	LL_TIM_InitTypeDef TIM_TimeBaseStructure;
	LL_TIM_OC_InitTypeDef TIM_OCInitStrcture;

	LL_APB2_GRP1_EnableClock(TIMER_CTRL_PERIPH);

	LL_TIM_StructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.Autoreload = 0xFFFF;
	TIM_TimeBaseStructure.Prescaler = 167;			// us resolution (168MHz / (167+1))
	TIM_TimeBaseStructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	TIM_TimeBaseStructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_TimeBaseStructure.RepetitionCounter = 0;
	LL_TIM_Init(TIMER_CTRL, &TIM_TimeBaseStructure);

	// Output compare mode configuration
	LL_TIM_OC_StructInit(&TIM_OCInitStrcture);
	TIM_OCInitStrcture.OCMode = LL_TIM_OCMODE_FROZEN;
	TIM_OCInitStrcture.CompareValue = 9800;  // MOSFET switchover start
	TIM_OCInitStrcture.OCState = LL_TIM_OCSTATE_ENABLE;
	LL_TIM_OC_Init(TIMER_CTRL, LL_TIM_CHANNEL_CH1, &TIM_OCInitStrcture);
	TIM_OCInitStrcture.CompareValue = 2500;  // Low voltage cutoff
	LL_TIM_OC_Init(TIMER_CTRL, LL_TIM_CHANNEL_CH2, &TIM_OCInitStrcture);
	TIM_OCInitStrcture.CompareValue = 5000;  // Peak
	LL_TIM_OC_Init(TIMER_CTRL, LL_TIM_CHANNEL_CH3, &TIM_OCInitStrcture);
	TIM_OCInitStrcture.CompareValue = 9900;  // Zero crossing (phase shifted)
	LL_TIM_OC_Init(TIMER_CTRL, LL_TIM_CHANNEL_CH4, &TIM_OCInitStrcture);

	/*LL_TIM_EnableIT_CC1(TIMER_CTRL);
	LL_TIM_EnableIT_CC2(TIMER_CTRL);
	LL_TIM_EnableIT_CC3(TIMER_CTRL);
	LL_TIM_EnableIT_CC4(TIMER_CTRL);*/

	nvicCTRLTimerConfiguration();

	LL_TIM_EnableCounter(TIMER_CTRL);
	LL_TIM_GenerateEvent_UPDATE(TIMER_CTRL);

}

// Multiplexer
void boardMUXConfig(void)
{

	// Channel 1 pin configuration
	LL_GPIO_SetPinMode(MUX_GPIO, MUX_1_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(MUX_GPIO, MUX_1_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(MUX_GPIO, MUX_1_Pin, LL_GPIO_OUTPUT_PUSHPULL);
	MUX_1_CLR();

	// Channel 2 pin configuration
	LL_GPIO_SetPinMode(MUX_GPIO, MUX_2_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(MUX_GPIO, MUX_2_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(MUX_GPIO, MUX_2_Pin, LL_GPIO_OUTPUT_PUSHPULL);

	// TODO: Delete
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_12, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_12, LL_GPIO_OUTPUT_PUSHPULL);

	MUX_2_CLR();

	// Channel 3 pin configuration
	LL_GPIO_SetPinMode(MUX_GPIO, MUX_3_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(MUX_GPIO, MUX_3_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(MUX_GPIO, MUX_3_Pin, LL_GPIO_OUTPUT_PUSHPULL);

	// TODO: Delete
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_4, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_4, LL_GPIO_OUTPUT_PUSHPULL);

	MUX_3_CLR();
}

// ADC
void boardADCConfig(void)
{
	__IO uint32_t wait_loop_index = 0;

	LL_ADC_CommonInitTypeDef ADC_CommonInitStructure;
	LL_ADC_InitTypeDef ADC_InitStructure;
	LL_ADC_REG_InitTypeDef ADC_REGInitStructure;
	LL_DMA_InitTypeDef DMA_InitStruct;

	// Set pin modes to analog
	LL_GPIO_SetPinMode(AN_Channel_GPIO1, AN_ToolVoltage_Pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(AN_Channel_GPIO1, AN_ToolCurrent_Pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(AN_Channel_GPIO2, AN_AmbientTemp_Pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(AN_Channel_GPIO2, AN_MuxDetect_Pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(AN_Channel_GPIO2, AN_IronTemp_Pin, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(AN_Channel_GPIO2, AN_RefVoltage_Pin, LL_GPIO_MODE_ANALOG);

	LL_APB2_GRP1_EnableClock(AN_ADC1_PERIPH);
	LL_APB2_GRP1_EnableClock(AN_ADC2_PERIPH);
	LL_APB2_GRP1_EnableClock(AN_ADC3_PERIPH);

	// Common ADC parameters
	LL_ADC_CommonStructInit(&ADC_CommonInitStructure);
	ADC_CommonInitStructure.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;  					// ADC clock prescaler
	ADC_CommonInitStructure.MultiDMATransfer = LL_ADC_MULTI_REG_DMA_EACH_ADC;				// Independent DMA
	ADC_CommonInitStructure.MultiTwoSamplingDelay = LL_ADC_MULTI_TWOSMP_DELAY_5CYCLES;		// Not important
	ADC_CommonInitStructure.Multimode = LL_ADC_MULTI_INDEPENDENT;							// Independent ADCs
	LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(AN_ADC1), &ADC_CommonInitStructure);

	// ADC1 parameters
	LL_ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;								// Data aligned right
	ADC_InitStructure.Resolution = LL_ADC_RESOLUTION_12B;									// 12 bit resolution
	ADC_InitStructure.SequencersScanMode = LL_ADC_SEQ_SCAN_ENABLE;							// Sequencer enabled (all selected channels sampled one by one)
	LL_ADC_Init(AN_ADC1, &ADC_InitStructure);
	// ADC2 parameters
	ADC_InitStructure.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;							// Sequencer disabled
	LL_ADC_Init(AN_ADC2, &ADC_InitStructure);
	// ADC3 parameters
	LL_ADC_Init(AN_ADC3, &ADC_InitStructure);


	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(AN_ADC1), (LL_ADC_PATH_INTERNAL_VREFINT | LL_ADC_PATH_INTERNAL_TEMPSENSOR));
	// Wait until internal sources stabilize
	wait_loop_index = ((LL_ADC_DELAY_TEMPSENSOR_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
	while(wait_loop_index != 0)
	{
	  wait_loop_index--;
	}

	// Regular group configuration
	ADC_REGInitStructure.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	ADC_REGInitStructure.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS;
	ADC_REGInitStructure.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_8RANKS; 					// Same channel will be sampled multiple times, must be same as ADC buffer size
	ADC_REGInitStructure.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED;
	ADC_REGInitStructure.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
	LL_ADC_REG_Init(AN_ADC1, &ADC_REGInitStructure);
	// ADC2
	ADC_REGInitStructure.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
	ADC_REGInitStructure.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	ADC_REGInitStructure.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
	LL_ADC_REG_Init(AN_ADC2, &ADC_REGInitStructure);
	// ADC3
	LL_ADC_REG_Init(AN_ADC3, &ADC_REGInitStructure);

	// Preinitialization
	// ADC1
	LL_ADC_REG_SetSequencerRanks(AN_ADC1, LL_ADC_REG_RANK_1, AN_ADC_AmbientTemp_Channel);
	LL_ADC_REG_SetSequencerRanks(AN_ADC1, LL_ADC_REG_RANK_2, AN_ADC_AmbientTemp_Channel);
	LL_ADC_REG_SetSequencerRanks(AN_ADC1, LL_ADC_REG_RANK_3, AN_ADC_AmbientTemp_Channel);
	LL_ADC_REG_SetSequencerRanks(AN_ADC1, LL_ADC_REG_RANK_4, AN_ADC_AmbientTemp_Channel);
	LL_ADC_REG_SetSequencerRanks(AN_ADC1, LL_ADC_REG_RANK_5, AN_ADC_AmbientTemp_Channel);
	LL_ADC_REG_SetSequencerRanks(AN_ADC1, LL_ADC_REG_RANK_6, AN_ADC_AmbientTemp_Channel);
	LL_ADC_REG_SetSequencerRanks(AN_ADC1, LL_ADC_REG_RANK_7, AN_ADC_AmbientTemp_Channel);
	LL_ADC_REG_SetSequencerRanks(AN_ADC1, LL_ADC_REG_RANK_8, AN_ADC_AmbientTemp_Channel);
	// ADC 2
	LL_ADC_REG_SetSequencerRanks(AN_ADC2, LL_ADC_REG_RANK_1, AN_ADC_ToolVoltage_Channel);
	// ADC 3
	LL_ADC_REG_SetSequencerRanks(AN_ADC3, LL_ADC_REG_RANK_1, AN_ADC_ToolCurrent_Channel);

	// Set sampling times
	// ADC1
	LL_ADC_SetChannelSamplingTime(AN_ADC1, AN_ADC_AmbientTemp_Channel, AN_ADC_AmbientTemp_SampleTime);
	LL_ADC_SetChannelSamplingTime(AN_ADC1, AN_ADC_MuxDetect_Channel, AN_ADC_MuxDetect_SampleTime);
	LL_ADC_SetChannelSamplingTime(AN_ADC1, AN_ADC_IronTemp_Channel, AN_ADC_IronTemp_SampleTime);
	LL_ADC_SetChannelSamplingTime(AN_ADC1, AN_ADC_RefVoltage_Channel, AN_ADC_RefVoltage_SampleTime);
	LL_ADC_SetChannelSamplingTime(AN_ADC1, AN_ADC_VRefInt_Channel, AN_ADC_VRefInt_SampleTime);
	LL_ADC_SetChannelSamplingTime(AN_ADC1, AN_ADC_IntTemp_Channel, AN_ADC_VRefInt_SampleTime);
	// ADC2
	LL_ADC_SetChannelSamplingTime(AN_ADC2, AN_ADC_ToolVoltage_Channel, AN_ADC_ToolVoltage_SampleTime);
	// ADC3
	LL_ADC_SetChannelSamplingTime(AN_ADC3, AN_ADC_ToolCurrent_Channel, AN_ADC_ToolCurrent_SampleTime);

	// Enable ADCs
	LL_ADC_Enable(AN_ADC1);
	LL_ADC_Enable(AN_ADC2);
	LL_ADC_Enable(AN_ADC3);

	// Enable interrupt
	nvicAnalogConfiguration();

	// Enable DMA Clock
	LL_AHB1_GRP1_EnableClock(AN_ADC_DMA_PERIPH);

	// Configure DMA for ADC1
	LL_DMA_DeInit(AN_ADC_DMA, AN_ADC1_DMA_Stream);
	LL_DMA_StructInit(&DMA_InitStruct);
	DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)&AN_ADC1->DR;
	DMA_InitStruct.MemoryOrM2MDstAddress = 0;
	DMA_InitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
	DMA_InitStruct.NbData = AN_ADC1_BUFFSIZE;
	DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
	DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
	DMA_InitStruct.Mode = LL_DMA_MODE_CIRCULAR;
	DMA_InitStruct.Priority = LL_DMA_PRIORITY_MEDIUM;
	DMA_InitStruct.Channel = AN_ADC1_DMA_Channel;
	DMA_InitStruct.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
	DMA_InitStruct.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_1_4;
	DMA_InitStruct.MemBurst = LL_DMA_MBURST_SINGLE;
	DMA_InitStruct.PeriphBurst = LL_DMA_PBURST_SINGLE;
	LL_DMA_Init(AN_ADC_DMA, AN_ADC1_DMA_Stream, &DMA_InitStruct);

	// Configure DMA for ADC2
	LL_DMA_DeInit(AN_ADC_DMA, AN_ADC2_DMA_Stream);
	DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)&AN_ADC2->DR;
	DMA_InitStruct.NbData = AN_ADC2_3_BUFFSIZE;
	DMA_InitStruct.Priority = LL_DMA_PRIORITY_VERYHIGH;
	DMA_InitStruct.Channel = AN_ADC2_DMA_Channel;
	LL_DMA_Init(AN_ADC_DMA, AN_ADC2_DMA_Stream, &DMA_InitStruct);

	// Configure DMA for ADC3
	LL_DMA_DeInit(AN_ADC_DMA, AN_ADC3_DMA_Stream);
	DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)&AN_ADC3->DR;
	DMA_InitStruct.Channel = AN_ADC3_DMA_Channel;
	LL_DMA_Init(AN_ADC_DMA, AN_ADC3_DMA_Stream, &DMA_InitStruct);
	//LL_DMA_EnableIT_TC(AN_ADC_DMA, AN_ADC3_DMA_Stream);

	nvicAnalogDMAConfiguration();

}

// Buzzer
void boardBuzzerChangeFreq(uint16_t freq)
{
	uint16_t period;
	LL_TIM_OC_InitTypeDef TIM_OCInitStrcture;

	if (!freq) {
		LL_TIM_CC_DisableChannel(BUZZER_TIMER, BUZZER_TIMER_CHANNEL);
		LL_TIM_DisableCounter(BUZZER_TIMER);
		return;
	}

	period = (uint16_t)(BUZZER_TIMER_MAXPERIOD / freq) - 1;

	LL_TIM_SetAutoReload(BUZZER_TIMER, period);

	LL_TIM_OC_StructInit(&TIM_OCInitStrcture);
	TIM_OCInitStrcture.OCMode = LL_TIM_OCMODE_PWM1;
	TIM_OCInitStrcture.CompareValue = period / 2;
	TIM_OCInitStrcture.OCState = LL_TIM_OCSTATE_ENABLE;
	LL_TIM_OC_Init(BUZZER_TIMER, BUZZER_TIMER_CHANNEL, &TIM_OCInitStrcture);

	LL_TIM_CC_EnableChannel(BUZZER_TIMER, BUZZER_TIMER_CHANNEL);
	LL_TIM_EnableCounter(BUZZER_TIMER);
	LL_TIM_GenerateEvent_UPDATE(BUZZER_TIMER);
}

void boardBuzzerConfig(void)
{
	LL_TIM_InitTypeDef TIM_TimeBaseStructure;
	LL_TIM_OC_InitTypeDef TIM_OCInitStrcture;

	LL_APB1_GRP1_EnableClock(BUZZER_TIMER_PERIPH);

	LL_GPIO_SetPinMode(BUZZER_PORT, BUZZER_Pin, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinPull(BUZZER_PORT, BUZZER_Pin, LL_GPIO_PULL_DOWN);
	LL_GPIO_SetPinSpeed(BUZZER_PORT, BUZZER_Pin, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetPinOutputType(BUZZER_PORT, BUZZER_Pin, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetAFPin_0_7(BUZZER_PORT, BUZZER_Pin, LL_GPIO_AF_2);

	uint16_t prescalerValue, period;
	// Compute the prescaler value
	prescalerValue = (uint16_t) (SystemCoreClock / (BUZZER_TIMER_MAXPERIOD * 2)) - 1;
	// Compute the period
	period = (uint16_t)(BUZZER_TIMER_MAXPERIOD / BUZZER_DEFAULT_PWM_Frequency) - 1;

	// Timer configuration
	LL_TIM_StructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.Prescaler = prescalerValue;
	TIM_TimeBaseStructure.Autoreload = period;
	TIM_TimeBaseStructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	TIM_TimeBaseStructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	TIM_TimeBaseStructure.RepetitionCounter = 0;

	LL_TIM_Init(BUZZER_TIMER, &TIM_TimeBaseStructure);

	// Output compare mode configuration
	LL_TIM_OC_StructInit(&TIM_OCInitStrcture);
	TIM_OCInitStrcture.OCMode = LL_TIM_OCMODE_PWM1;
	TIM_OCInitStrcture.CompareValue = period / 2;
	TIM_OCInitStrcture.OCState = LL_TIM_OCSTATE_ENABLE;
	LL_TIM_OC_Init(BUZZER_TIMER, BUZZER_TIMER_CHANNEL, &TIM_OCInitStrcture);


	LL_TIM_OC_EnablePreload(BUZZER_TIMER, BUZZER_TIMER_CHANNEL);
	LL_TIM_EnableARRPreload(BUZZER_TIMER);

	boardBuzzerChangeFreq(0);

}

// EEPROM

void boardEEPROMConfig(void)
{
	LL_DMA_InitTypeDef DMA_InitStruct;
	LL_I2C_InitTypeDef I2C_InitStruct;

	// Enable I2C clock
	LL_APB1_GRP1_EnableClock(EEPROM_I2C_PERIPH);

	// SCL pin
	LL_GPIO_SetPinMode(EEPROM_I2C_GPIO, EEPROM_I2C_SCL_Pin, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(EEPROM_I2C_GPIO, EEPROM_I2C_SCL_Pin, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(EEPROM_I2C_GPIO, EEPROM_I2C_SCL_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(EEPROM_I2C_GPIO, EEPROM_I2C_SCL_Pin, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(EEPROM_I2C_GPIO, EEPROM_I2C_SCL_Pin, LL_GPIO_PULL_UP);

	// SDA pin
	LL_GPIO_SetPinMode(EEPROM_I2C_GPIO, EEPROM_I2C_SDA_Pin, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_8_15(EEPROM_I2C_GPIO, EEPROM_I2C_SDA_Pin, LL_GPIO_AF_4);
	LL_GPIO_SetPinSpeed(EEPROM_I2C_GPIO, EEPROM_I2C_SDA_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(EEPROM_I2C_GPIO, EEPROM_I2C_SDA_Pin, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(EEPROM_I2C_GPIO, EEPROM_I2C_SDA_Pin, LL_GPIO_PULL_UP);

	// WC pin
	LL_GPIO_SetPinMode(EEPROM_I2C_GPIO, EEPROM_I2C_WC_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(EEPROM_I2C_GPIO, EEPROM_I2C_WC_Pin, LL_GPIO_SPEED_FREQ_MEDIUM);
	LL_GPIO_SetPinOutputType(EEPROM_I2C_GPIO, EEPROM_I2C_WC_Pin, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(EEPROM_I2C_GPIO, EEPROM_I2C_WC_Pin, LL_GPIO_PULL_UP);

	// Enable write protection
	EEPROM_I2C_WC_ON();

	// Disable periphery
	LL_I2C_Disable(EEPROM_I2C);

	// Configure I2C
	LL_I2C_StructInit(&I2C_InitStruct);
	I2C_InitStruct.ClockSpeed = EEPROM_I2C_SPEED;
	I2C_InitStruct.OwnAddress1 = EEPROM_I2C_OWN_ADDRESS;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
	LL_I2C_Init(EEPROM_I2C, &I2C_InitStruct);

	nvicEEPROMi2cConfiguration();

	// Enable interrupts
	//LL_I2C_EnableIT_EVT(EEPROM_I2C);
	LL_I2C_EnableIT_ERR(EEPROM_I2C);

	// Enable I2C
	LL_I2C_Enable(EEPROM_I2C);

	// Enable DMA Clock
	LL_AHB1_GRP1_EnableClock(EEPROM_I2C_DMA_PERIPH);

	// Configure DMA for transmit
	LL_DMA_DeInit(EEPROM_I2C_DMA, EEPROM_I2C_Tx_DMA_Stream);
	LL_DMA_StructInit(&DMA_InitStruct);
	DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)&EEPROM_I2C->DR;
	DMA_InitStruct.MemoryOrM2MDstAddress = 0;
	DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
	DMA_InitStruct.NbData = 0;
	DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
	DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
	DMA_InitStruct.Mode = LL_DMA_MODE_NORMAL;
	DMA_InitStruct.Priority = LL_DMA_PRIORITY_HIGH;
	DMA_InitStruct.Channel = EEPROM_I2C_Tx_DMA_Channel;
	DMA_InitStruct.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
	DMA_InitStruct.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_1_4;
	DMA_InitStruct.MemBurst = LL_DMA_MBURST_SINGLE;
	DMA_InitStruct.PeriphBurst = LL_DMA_PBURST_SINGLE;
	LL_DMA_Init(EEPROM_I2C_DMA, EEPROM_I2C_Tx_DMA_Stream, &DMA_InitStruct);

	// Configure DMA for receive
	LL_DMA_DeInit(EEPROM_I2C_DMA, EEPROM_I2C_Rx_DMA_Stream);
	DMA_InitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
	DMA_InitStruct.Channel = EEPROM_I2C_Rx_DMA_Channel;
	LL_DMA_Init(EEPROM_I2C_DMA, EEPROM_I2C_Rx_DMA_Stream, &DMA_InitStruct);

	nvicEEPROMi2cDMAConfiguration();

}

#ifdef MONITOR
// Monitoring
void boardMonitoringConfig(void)
{

	//LL_APB2_GRP1_EnableClock(MONITORING_GPIO_PERIPH);

	// Monitoring pin 1 configuration
	LL_GPIO_SetPinMode(MONITORING_GPIO, MONITORING_1_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(MONITORING_GPIO, MONITORING_1_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(MONITORING_GPIO, MONITORING_1_Pin, LL_GPIO_OUTPUT_PUSHPULL);

	// Monitoring pin 2 configuration
	LL_GPIO_SetPinMode(MONITORING_GPIO, MONITORING_2_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(MONITORING_GPIO, MONITORING_2_Pin, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(MONITORING_GPIO, MONITORING_2_Pin, LL_GPIO_OUTPUT_PUSHPULL);

}
#endif


/*
 // Keys (switches)

 void boardKeysConfig(void)
 {
 // Enable clocks
 LL_APB2_GRP1_EnableClock(KEYS_GPIO_PERIPH);

 // Pins
 LL_GPIO_SetPinMode(KEYS_GPIO, KEYS_SW1_Pin, LL_GPIO_MODE_INPUT);
 LL_GPIO_SetPinPull(KEYS_GPIO, KEYS_SW1_Pin, LL_GPIO_PULL_UP);
 LL_GPIO_SetPinMode(KEYS_GPIO, KEYS_SW2_Pin, LL_GPIO_MODE_INPUT);
 LL_GPIO_SetPinPull(KEYS_GPIO, KEYS_SW2_Pin, LL_GPIO_PULL_UP);

 // Configure external interrupts
 LL_GPIO_AF_SetEXTISource(KEYS_GPIO_EXTI_PORT, KEYS_SW1_EXTI_LINE);
 LL_GPIO_AF_SetEXTISource(KEYS_GPIO_EXTI_PORT, KEYS_SW2_EXTI_LINE);

 // Enable interrupts
 LL_EXTI_EnableRisingTrig_0_31(KEYS_SW1_EXTI);
 LL_EXTI_EnableRisingTrig_0_31(KEYS_SW2_EXTI);
 LL_EXTI_EnableFallingTrig_0_31(KEYS_SW1_EXTI);
 LL_EXTI_EnableFallingTrig_0_31(KEYS_SW2_EXTI);
 LL_EXTI_EnableIT_0_31(KEYS_SW1_EXTI);
 LL_EXTI_EnableIT_0_31(KEYS_SW2_EXTI);

 Keys_EXTI_NVIC_Configuration();
 }

 */

/** \brief Initialize SysTick interrupt
 *
 * \param void
 * \return void
 *
 */
void boardSysTickInit(uint32_t tickRate) {
	uint32_t cnt = (SystemCoreClock / 8) / tickRate;

	SysTick_Config(cnt);
	NVIC_SetPriority(SysTick_IRQn, 0);
	SysTick->CTRL &= (uint32_t) 0xFFFFFFFB;
}

typedef void (*pFunction)(void);

// Load bootloader
void boardLoadBootloader(void)
{
	pFunction SysMemBootJump = (void (*)(void))(*((uint32_t*)0x1FFF0004));

	__set_PRIMASK(1);
	__set_MSP(0x20001000);

	SysMemBootJump();

	while (1);
}

// System reset
void boardReset(void)
{
	__disable_irq();

	NVIC_SystemReset();
}


/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(char * file, int line) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}



// Exception stack frame
typedef struct
{
	uint32_t r0;               // R0
	uint32_t r1;               // R1
	uint32_t r2;               // R2
	uint32_t r3;               // R3
	uint32_t r12;              // R12
	uint32_t lr;               // Link register
	uint32_t pc;               // Program counter
	uint32_t psr;              // Program status register
	uint32_t s0;               // S0
	uint32_t s1;               // S1
	uint32_t s2;               // S2
	uint32_t s3;               // S3
	uint32_t s4;               // S4
	uint32_t s5;               // S5
	uint32_t s6;               // S6
	uint32_t s7;               // S7
	uint32_t s8;               // S8
	uint32_t s9;               // S9
	uint32_t s10;              // S10
	uint32_t s11;              // S11
	uint32_t s12;              // S12
	uint32_t s13;              // S13
	uint32_t s14;              // S14
	uint32_t s15;              // S15
	uint32_t fpscr;            // Floating point status and control register
} StackFrame_t;


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
void __attribute__((optimize("O0"))) _hardFaultHandlerC(uint32_t *stackFrameAddress)
{
    volatile StackFrame_t *stackFrame;

    stackFrame = (StackFrame_t*)stackFrameAddress;

    __ASM volatile("BKPT #0");

    while (1) {}
}
#pragma GCC diagnostic pop


NAKED void _hardFaultHandler(void)
{
    __ASM volatile(
      "tst lr, #4             \n\t"     // Test bit 2 of EXC_RETURN code
      "ite eq                 \n\t"
      "mrseq r0, msp          \n\t"     // If 0, stacking used MSP, copy to R0 (main task, IRQ or kernel function)
      "mrsne r0, psp          \n\t"     // If 1, stacking used PSP, copy to R0 (any other task)
      "b _hardFaultHandlerC  \n\t"     // Call C hard fault handler function
    );
}

void WWDG_IRQHandler(void)
{
	while (1) {}
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
void __attribute__((optimize("O0"))) _usageFaultHandlerC(uint32_t *stackFrameAddress)
{
    volatile StackFrame_t *stackFrame;

    stackFrame = (StackFrame_t*)stackFrameAddress;

    __ASM volatile("BKPT #0");

    while (1) {}
}
#pragma GCC diagnostic pop


NAKED void _usageFaultHandler(void)
{
    __ASM volatile(
      "tst lr, #4             \n\t"     // Test bit 2 of EXC_RETURN code
      "ite eq                 \n\t"
      "mrseq r0, msp          \n\t"     // If 0, stacking used MSP, copy to R0 (main task, IRQ or kernel function)
      "mrsne r0, psp          \n\t"     // If 1, stacking used PSP, copy to R0 (any other task)
      "b _usageFaultHandlerC  \n\t"     // Call C hard fault handler function
    );
}
