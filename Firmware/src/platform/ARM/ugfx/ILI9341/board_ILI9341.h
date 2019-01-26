/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* This template board is updated to support a DMA driven ILI9341 TFT panel with STM32F4 advanced/foundation lines using Low Layer.
 * Modified by Attila Kovács, 2018, 2019.
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

#include "arch.h"
#include "gfx.h"	// IDE can't find declarations otherwise

//#define USE_PWM_BACKLIGHT

// STM32F4xx advanced line compatible

#define LCD_PORT_SPI				GPIOA
#define LCD_PORT_LED				GPIOB
#define LCD_PORT_CS					GPIOC
#define LCD_PORT_RES				GPIOC
#define LCD_PORT_DC					GPIOB
#define LCD_MOSI					LL_GPIO_PIN_7		// PA7
#define LCD_MISO					LL_GPIO_PIN_6		// PA6
#define LCD_SCK						LL_GPIO_PIN_5		// PA5
#define LCD_CS						LL_GPIO_PIN_4		// PC4
#define LCD_DC						LL_GPIO_PIN_0		// PB0
#define LCD_RES						LL_GPIO_PIN_5		// PC5
#define LCD_LED						LL_GPIO_PIN_1		// PB1

#define LCD_SPI             		SPI1
#define LCD_SPI_PERIPH      		LL_APB2_GRP1_PERIPH_SPI1

#define LCD_BACKLIGHT_TIMER			TIM3
#define LCD_BACKLIGHT_TIMER_PERIPH  LL_APB1_GRP1_PERIPH_TIM3
#define LCD_BACKLIGHT_TIMER_CHANNEL	LL_TIM_CHANNEL_CH4
#define LCD_BACKLIGHT_PWM_Frequency 10000
#define LCD_BACKLIGHT_SETLEVEL(x)	LL_TIM_OC_SetCompareCH4(LCD_BACKLIGHT_TIMER, x)

#define SET_CS()   					LL_GPIO_SetOutputPin  (LCD_PORT_CS, LCD_CS)
#define CLR_CS()   					LL_GPIO_ResetOutputPin(LCD_PORT_CS, LCD_CS)
#define SET_RES()  					LL_GPIO_SetOutputPin  (LCD_PORT_RES, LCD_RES)
#define CLR_RES()  					LL_GPIO_ResetOutputPin(LCD_PORT_RES, LCD_RES)
#define SET_DC()   					LL_GPIO_SetOutputPin  (LCD_PORT_DC, LCD_DC)
#define CLR_DC()  					LL_GPIO_ResetOutputPin(LCD_PORT_DC, LCD_DC)
#ifndef USE_PWM_BACKLIGHT
#define SET_LED()  					LL_GPIO_SetOutputPin  (LCD_PORT_LED, LCD_LED)
#define CLR_LED()  					LL_GPIO_ResetOutputPin(LCD_PORT_LED, LCD_LED)
#endif

#if defined(GDISP_USE_DMA)
	#define LCD_SPI_DMA                   			DMA2
	#define LCD_SPI_DMA_PERIPH            			LL_AHB1_GRP1_PERIPH_DMA2
	#define LCD_SPI_DMA_Channel           			LL_DMA_CHANNEL_3
	#define LCD_SPI_DMA_Tx_Stream         			LL_DMA_STREAM_3
	#define LCD_SPI_DMA_Tx_Stream_REG				DMA2_Stream3
	#define LCD_SPI_DMA_Tx_IRQn             		DMA2_Stream3_IRQn
	#define LCD_SPI_DMA_Tx_IRQHandler       		DMA2_Stream3_IRQHandler
	#define LCD_SPI_DMA_Tx_ClearTransferFlags()   	LL_DMA_ClearFlag_TC3(LCD_SPI_DMA); LL_DMA_ClearFlag_TE3(LCD_SPI_DMA)

	static gfxSem			dmatransfer;
	static gfxMutex			buslock;
#endif

static GFXINLINE void init_board(GDisplay *g) {
	(void) g;

	LL_SPI_InitTypeDef SPI_InitStruct;
	#if defined(GDISP_USE_DMA)
		LL_DMA_InitTypeDef DMA_InitStruct;
	#endif
	#if defined(USE_PWM_BACKLIGHT)
		LL_TIM_InitTypeDef TIM_TimeBaseStructure;
		LL_TIM_OC_InitTypeDef TIM_OCInitStrcture;

		LL_APB1_GRP1_EnableClock(LCD_BACKLIGHT_TIMER_PERIPH);
	#endif

	LL_APB2_GRP1_EnableClock(LCD_SPI_PERIPH);
	// Enable port clocks here if not enabled yet

	// MOSI
	LL_GPIO_SetPinMode(LCD_PORT_SPI, LCD_MOSI, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(LCD_PORT_SPI, LCD_MOSI, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinOutputType(LCD_PORT_SPI, LCD_MOSI, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetAFPin_0_7(LCD_PORT_SPI, LCD_MOSI, LL_GPIO_AF_5);
	// MISO
	LL_GPIO_SetPinMode(LCD_PORT_SPI, LCD_MISO, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(LCD_PORT_SPI, LCD_MISO, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinOutputType(LCD_PORT_SPI, LCD_MISO, LL_GPIO_OUTPUT_PUSHPULL);// Resets current type just in case (it is still not an output)
	LL_GPIO_SetAFPin_0_7(LCD_PORT_SPI, LCD_MISO, LL_GPIO_AF_5);
	// SCK
	LL_GPIO_SetPinMode(LCD_PORT_SPI, LCD_SCK, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(LCD_PORT_SPI, LCD_SCK, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinOutputType(LCD_PORT_SPI, LCD_SCK, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetAFPin_0_7(LCD_PORT_SPI, LCD_SCK, LL_GPIO_AF_5);
	// CS
	LL_GPIO_SetPinMode(LCD_PORT_CS, LCD_CS, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(LCD_PORT_CS, LCD_CS, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(LCD_PORT_CS, LCD_CS, LL_GPIO_OUTPUT_PUSHPULL);
	SET_CS(); // Release bus
	// DC
	LL_GPIO_SetPinMode(LCD_PORT_DC, LCD_DC, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(LCD_PORT_DC, LCD_DC, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(LCD_PORT_DC, LCD_DC, LL_GPIO_OUTPUT_PUSHPULL);
	SET_DC(); // Default data mode
	// RES
	LL_GPIO_SetPinMode(LCD_PORT_RES, LCD_RES, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinSpeed(LCD_PORT_RES, LCD_RES, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(LCD_PORT_RES, LCD_RES, LL_GPIO_OUTPUT_PUSHPULL);
	// LED
	#if !defined(USE_PWM_BACKLIGHT)
		LL_GPIO_SetPinMode(LCD_PORT_LED, LCD_LED, LL_GPIO_MODE_OUTPUT);
		LL_GPIO_SetPinSpeed(LCD_PORT_LED, LCD_LED, LL_GPIO_SPEED_FREQ_HIGH);
		LL_GPIO_SetPinOutputType(LCD_PORT_LED, LCD_LED, LL_GPIO_OUTPUT_PUSHPULL);
	#else
		LL_GPIO_SetPinMode(LCD_PORT_LED, LCD_LED, LL_GPIO_MODE_ALTERNATE);
		LL_GPIO_SetPinSpeed(LCD_PORT_LED, LCD_LED, LL_GPIO_SPEED_FREQ_HIGH);
		LL_GPIO_SetPinOutputType(LCD_PORT_LED, LCD_LED, LL_GPIO_OUTPUT_PUSHPULL);
		LL_GPIO_SetAFPin_0_7(LCD_PORT_LED, LCD_LED, LL_GPIO_AF_2);
	#endif

	SPI_InitStruct.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
	SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
	SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
	SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
	SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
	SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
	SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
	SPI_InitStruct.CRCPoly = 7;
	LL_SPI_Init(LCD_SPI, &SPI_InitStruct);
	LL_SPI_Enable(LCD_SPI);

	#if defined(GDISP_USE_DMA)
		// Enable DMA clock
		LL_AHB1_GRP1_EnableClock(LCD_SPI_DMA_PERIPH);
		// Resets DMA stream configuration
		LL_DMA_DeInit(LCD_SPI_DMA, LCD_SPI_DMA_Tx_Stream);
		// Initializes configuration struct
		LL_DMA_StructInit(&DMA_InitStruct);
		// DMA configuration
		DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t) &LCD_SPI->DR;
		DMA_InitStruct.MemoryOrM2MDstAddress = 0;
		DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
		DMA_InitStruct.NbData = 0;
		DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
		DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
		DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
		DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
		DMA_InitStruct.Mode = LL_DMA_MODE_NORMAL;
		DMA_InitStruct.Priority = LL_DMA_PRIORITY_MEDIUM;
		DMA_InitStruct.Channel = LCD_SPI_DMA_Channel;
		DMA_InitStruct.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
		DMA_InitStruct.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_1_4;
		DMA_InitStruct.MemBurst = LL_DMA_MBURST_SINGLE;
		DMA_InitStruct.PeriphBurst = LL_DMA_PBURST_SINGLE;
		LL_DMA_Init(LCD_SPI_DMA, LCD_SPI_DMA_Tx_Stream, &DMA_InitStruct);
		// NVIC configuration
		NVIC_SetPriority(LCD_SPI_DMA_Tx_IRQn, NVIC_EncodePriority((7 - __NVIC_PRIO_BITS), 5, 0));
		NVIC_EnableIRQ(LCD_SPI_DMA_Tx_IRQn);

		gfxSemInit(&dmatransfer, 0, 1);
		gfxMutexInit(&buslock);
	#endif

	#if defined(USE_PWM_BACKLIGHT)
		uint16_t prescalerValue, period;
		// Compute the prescaler value
		prescalerValue = (uint16_t) (SystemCoreClock / 1200000) - 1;
		// Compute the period
		period = (uint16_t)(1200000 / LCD_BACKLIGHT_PWM_Frequency) - 1;

		// Timer configuration
		LL_TIM_StructInit(&TIM_TimeBaseStructure);
		TIM_TimeBaseStructure.Prescaler = prescalerValue;
		TIM_TimeBaseStructure.Autoreload = period;
		TIM_TimeBaseStructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
		TIM_TimeBaseStructure.CounterMode = LL_TIM_COUNTERMODE_UP;
		TIM_TimeBaseStructure.RepetitionCounter = 0;

		LL_TIM_Init(LCD_BACKLIGHT_TIMER, &TIM_TimeBaseStructure);

		// Output compare mode configuration
		LL_TIM_OC_StructInit(&TIM_OCInitStrcture);
		TIM_OCInitStrcture.OCMode = LL_TIM_OCMODE_PWM1;
		TIM_OCInitStrcture.CompareValue = 0;
		TIM_OCInitStrcture.OCState = LL_TIM_OCSTATE_ENABLE;
		LL_TIM_OC_Init(LCD_BACKLIGHT_TIMER, LCD_BACKLIGHT_TIMER_CHANNEL, &TIM_OCInitStrcture);

		LL_TIM_EnableCounter(LCD_BACKLIGHT_TIMER);
	#else
		// Turn off backlight
		CLR_LED();
	#endif
}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;

	if (state == TRUE) {
		CLR_RES();
	} else {
		SET_RES();
	}
}

static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
	(void) percent;

#if defined(USE_PWM_BACKLIGHT)
	uint32_t period = LL_TIM_GetAutoReload(LCD_BACKLIGHT_TIMER);

	period *= percent;
	period /= 100;

	LCD_BACKLIGHT_SETLEVEL(period);
#else
	if (percent) SET_LED();
	else
		CLR_LED();
#endif
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;

#if defined(GDISP_USE_DMA)
	gfxMutexEnter(&buslock);
#endif
	CLR_CS();
}

static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;

	while (LCD_SPI->SR & SPI_SR_BSY) { __NOP(); };
	CLR_CS();
	SET_CS();
#if defined(GDISP_USE_DMA)
	gfxMutexExit(&buslock);
#endif
}

static GFXINLINE void send_data(uint16_t data) {

  while (!(LCD_SPI->SR & SPI_SR_TXE)) { __NOP(); };
  LCD_SPI->DR = data;
}

static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;

	 SET_DC(); // delay
	 //SET_DC(); // delay for slow SPI
	 CLR_DC();
	 send_data(index);
	 CLR_DC(); // delay
	 //CLR_DC(); // delay for slow SPI
	 SET_DC();
}

static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;
	(void) data;

	send_data(data);
}

static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;
	return 0;
}

#if defined(GDISP_USE_DMA) || defined(__DOXYGEN__)

	static GFXINLINE void set_dma_memory_inc(void)
	{
		LCD_SPI_DMA_Tx_Stream_REG->CR |= DMA_SxCR_MINC;  // Enable DMA memory increment mode
	}

	static GFXINLINE void set_dma_memory_noinc(void)
	{
		LCD_SPI_DMA_Tx_Stream_REG->CR &= ~DMA_SxCR_MINC;  // Disable DMA memory increment mode
	}

	static GFXINLINE void set_dma_memory_address(uint32_t addr)
	{
		LCD_SPI_DMA_Tx_Stream_REG->M0AR = addr;
	}

	static GFXINLINE void start_dma_transfer(uint16_t num)
	{
		LCD_SPI_DMA_Tx_Stream_REG->NDTR = num;	// Set number of DMA operations
		LCD_SPI_DMA_Tx_Stream_REG->CR |= DMA_SxCR_EN;  // Enable DMA
	}

	static GFXINLINE void end_dma_transfer(void)
	{
		LCD_SPI_DMA_Tx_ClearTransferFlags();	// Clear interrupt flags
		LCD_SPI_DMA_Tx_Stream_REG->CR &= ~DMA_SxCR_EN; // Disable DMA
	}

	static GFXINLINE void disable_dma_transfer(void)
	{
		LCD_SPI_DMA_Tx_Stream_REG->CR &= ~(DMA_SxCR_TCIE | DMA_SxCR_TEIE); // Disable DMA transfer complete/error interrupts
		LCD_SPI->CR2 &= (uint16_t)~SPI_CR2_TXDMAEN; // Disable SPI transfer via DMA mode
	}

	static GFXINLINE void enable_dma_transfer(void)
	{
		LCD_SPI->CR2 |= SPI_CR2_TXDMAEN; // Enable SPI transfer via DMA mode
		LCD_SPI_DMA_Tx_Stream_REG->CR |= DMA_SxCR_TCIE | DMA_SxCR_TEIE; // Enable DMA transfer complete/error interrupts
	}

	static GFXINLINE void set_spi_8bit_mode(void)
	{
		LCD_SPI->CR1 &= ~SPI_CR1_SPE; // Disable
		LCD_SPI->CR1 &= ~SPI_CR1_DFF; // SPI 8 bit
		LCD_SPI->CR1 |= SPI_CR1_SPE;  // Enable
	}

	static GFXINLINE void set_spi_16bit_mode(void)
	{
		LCD_SPI->CR1 &= ~SPI_CR1_SPE; // Disable
		LCD_SPI->CR1 |= SPI_CR1_DFF;  // SPI 16 bit
		LCD_SPI->CR1 |= SPI_CR1_SPE;  // Enable
	}

	void __attribute__ ((interrupt ("IRQ"))) LCD_SPI_DMA_Tx_IRQHandler(void) {
		end_dma_transfer();
		gfxSemSignalI(&dmatransfer);
		__DSB();
	}


	static GFXINLINE void dma_with_noinc(GDisplay *g, color_t *buffer, int area) {
		(void) g;

		set_spi_16bit_mode();
		set_dma_memory_address((uint32_t)buffer);
		set_dma_memory_noinc();
		enable_dma_transfer();
		for (; area > 0; area -= 65535) {
			start_dma_transfer(area > 65535 ? 65535 : area);
			gfxSemWait(&dmatransfer, TIME_INFINITE);
		}
		disable_dma_transfer();
		set_spi_8bit_mode();
	}

	static GFXINLINE void dma_with_inc(GDisplay *g, color_t *buffer, int area) {
		(void) g;

		set_spi_16bit_mode();
		set_dma_memory_address((uint32_t)buffer);
		set_dma_memory_inc();
		enable_dma_transfer();
		for (; area > 0; area -= 65535) {
			start_dma_transfer(area > 65535 ? 65535 : area);
			gfxSemWait(&dmatransfer, TIME_INFINITE);
		}
		disable_dma_transfer();
		set_spi_8bit_mode();
	}
#endif

#endif /* _GDISP_LLD_BOARD_H */
