/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* This template board is updated to support a DMA driven SDIO with STM32F4 advanced/foundation lines using Low Layer.
 * Modified by Attila Kovács, 2018.
 */

#ifndef _GFILE_LLD_BOARD_H
#define _GFILE_LLD_BOARD_H

#include "arch.h"
#include "gfx.h"	// IDE can't find declarations otherwise

// STM32F4xx advanced line compatible

#define SD_PORT_MAIN							GPIOC
#define SD_PORT_CMD								GPIOD

#define SD_D0									LL_GPIO_PIN_8		// PC8
#define SD_D1									LL_GPIO_PIN_9		// PC9
#define SD_D2									LL_GPIO_PIN_10		// PC10
#define SD_D3									LL_GPIO_PIN_11		// PC11
#define SD_SCK									LL_GPIO_PIN_12		// PC12
#define SD_CMD									LL_GPIO_PIN_2		// PD2

#define SD_PERIPH      							LL_APB2_GRP1_PERIPH_SDIO
#define SD_INIT_CLK_DIV							0x76				// 48MHz / ( 118 + 2) = 400kHz , MCU running at 168MHz with PLL Q = 7
#define SD_OP_CLK_DIV							0x00				// 48MHz / ( 0 + 2) = 24MHz , MCU running at 168MHz with PLL Q = 7

#define SD_DMA                   				DMA2
#define SD_DMA_PERIPH            				LL_AHB1_GRP1_PERIPH_DMA2
#define SD_DMA_Channel           				LL_DMA_CHANNEL_4
#define SD_DMA_Stream         					LL_DMA_STREAM_6
#define SD_DMA_Stream_REG						DMA2_Stream6
#define SD_DMA_IRQn             				DMA2_Stream6_IRQn
#define SD_DMA_IRQHandler       				DMA2_Stream6_IRQHandler
#define SD_DMA_ClearTransferFlags()   			LL_DMA_ClearFlag_TC6(SD_DMA); LL_DMA_ClearFlag_TE6(SD_DMA)

static gfxSem			dmatransfer;

#define sd_cmd0()				SDMMC_CmdGoIdleState(SDIO)
#define sd_cmd8()				SDMMC_CmdOperCond(SDIO)
#define sd_cmd55(arg)			SDMMC_CmdAppCommand(SDIO, arg)
#define sd_cmd41(sdtype)		SDMMC_CmdAppOperCommand(SDIO, sdtype)
#define sd_cmd2()				SDMMC_CmdSendCID(SDIO)
#define sd_cmd3(rca)			SDMMC_CmdSetRelAdd(SDIO, rca);
#define sd_cmd9(arg)			SDMMC_CmdSendCSD(SDIO, arg)
#define sd_cmd7(addr)			SDMMC_CmdSendCSD(SDIO, addr)
#define sd_cmd16(blocksize)		SDMMC_CmdBlockLength(SDIO, blocksize)
#define sd_cmd51()				SDMMC_CmdSendSCR(SDIO)
#define sd_cmd6(buswidth)		SDMMC_CmdBusWidth(SDIO, buswidth)


#define sd_r1()				(SDIO->RESP1)
#define sd_r2()				(SDIO->RESP2)
#define sd_r3()				(SDIO->RESP3)
#define sd_r4()				(SDIO->RESP4)


static GFXINLINE void init_board_sdio(void) {

	LL_DMA_InitTypeDef DMA_InitStruct;

	LL_APB2_GRP1_EnableClock(SD_DMA_PERIPH);
	// Enable port clocks here if not enabled yet

	// D0
	LL_GPIO_SetPinMode(SD_PORT_MAIN, SD_D0, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(SD_PORT_MAIN, SD_D0, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(SD_PORT_MAIN, SD_D0, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(SD_PORT_MAIN, SD_D0, LL_GPIO_PULL_UP);
	LL_GPIO_SetAFPin_8_15(SD_PORT_MAIN, SD_D0, LL_GPIO_AF_12);
	// D1
	LL_GPIO_SetPinMode(SD_PORT_MAIN, SD_D1, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(SD_PORT_MAIN, SD_D1, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(SD_PORT_MAIN, SD_D1, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(SD_PORT_MAIN, SD_D1, LL_GPIO_PULL_UP);
	LL_GPIO_SetAFPin_8_15(SD_PORT_MAIN, SD_D1, LL_GPIO_AF_12);
	// D2
	LL_GPIO_SetPinMode(SD_PORT_MAIN, SD_D2, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(SD_PORT_MAIN, SD_D2, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(SD_PORT_MAIN, SD_D2, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(SD_PORT_MAIN, SD_D2, LL_GPIO_PULL_UP);
	LL_GPIO_SetAFPin_8_15(SD_PORT_MAIN, SD_D2, LL_GPIO_AF_12);
	// D3
	LL_GPIO_SetPinMode(SD_PORT_MAIN, SD_D3, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(SD_PORT_MAIN, SD_D3, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(SD_PORT_MAIN, SD_D3, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(SD_PORT_MAIN, SD_D3, LL_GPIO_PULL_UP);
	LL_GPIO_SetAFPin_8_15(SD_PORT_MAIN, SD_D3, LL_GPIO_AF_12);
	// SCK
	LL_GPIO_SetPinMode(SD_PORT_MAIN, SD_SCK, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(SD_PORT_MAIN, SD_SCK, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(SD_PORT_MAIN, SD_SCK, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(SD_PORT_MAIN, SD_SCK, LL_GPIO_PULL_NO);
	LL_GPIO_SetAFPin_8_15(SD_PORT_MAIN, SD_SCK, LL_GPIO_AF_12);
	// CMD
	LL_GPIO_SetPinMode(SD_PORT_CMD, SD_CMD, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(SD_PORT_CMD, SD_CMD, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(SD_PORT_CMD, SD_CMD, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(SD_PORT_CMD, SD_CMD, LL_GPIO_PULL_UP);
	LL_GPIO_SetAFPin_0_7(SD_PORT_CMD, SD_CMD, LL_GPIO_AF_12);

	// Enable DMA clock
	LL_AHB1_GRP1_EnableClock(SD_DMA_PERIPH);
	// Resets DMA stream configuration
	LL_DMA_DeInit(SD_DMA, SD_DMA_Stream);
	// Initializes configuration struct
	LL_DMA_StructInit(&DMA_InitStruct);
	// DMA configuration
	DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t) &SDIO->FIFO;
	DMA_InitStruct.MemoryOrM2MDstAddress = 0;
	DMA_InitStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
	DMA_InitStruct.NbData = 0;
	DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
	DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
	DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD; // 4 bytes
	DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD; // 4 bytes
	DMA_InitStruct.Mode = LL_DMA_MODE_NORMAL;
	DMA_InitStruct.Priority = LL_DMA_PRIORITY_MEDIUM;
	DMA_InitStruct.Channel = SD_DMA_Channel;
	DMA_InitStruct.FIFOMode = LL_DMA_FIFOMODE_ENABLE;
	DMA_InitStruct.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
	DMA_InitStruct.MemBurst = LL_DMA_MBURST_INC4;
	DMA_InitStruct.PeriphBurst = LL_DMA_PBURST_INC4;
	LL_DMA_Init(SD_DMA, SD_DMA_Stream, &DMA_InitStruct);
	// NVIC configuration
	NVIC_SetPriority(SD_DMA_IRQn, NVIC_EncodePriority((7 - __NVIC_PRIO_BITS), 5, 0));
	NVIC_EnableIRQ(SD_DMA_IRQn);

	gfxSemInit(&dmatransfer, 0, 1);

}

static GFXINLINE void sd_init_400khz()
{
	SDIO_InitTypeDef SD_InitStruct;

	// SD Init
	SD_InitStruct.BusWide = SDIO_BUS_WIDE_1B;
	SD_InitStruct.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	SD_InitStruct.ClockDiv = SD_INIT_CLK_DIV;
	SD_InitStruct.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	SD_InitStruct.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	SD_InitStruct.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	SDIO_Init(SDIO, SD_InitStruct);
}

static GFXINLINE void sd_init_high_speed()
{
	SDIO_InitTypeDef SD_InitStruct;

	// SD Init
	SD_InitStruct.BusWide = SDIO_BUS_WIDE_4B;
	SD_InitStruct.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	SD_InitStruct.ClockDiv = SD_OP_CLK_DIV;
	SD_InitStruct.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	SD_InitStruct.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	SD_InitStruct.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_ENABLE;
	SDIO_Init(SDIO, SD_InitStruct);
}

static GFXINLINE void sd_data_init(uint32_t size, uint32_t blocksize)
{
	SDIO_DataInitTypeDef config;

	config.DataTimeOut   = SDMMC_DATATIMEOUT;
	config.DataLength    = size;
	config.DataBlockSize = blocksize;
	config.TransferDir   = SDIO_TRANSFER_DIR_TO_SDIO;
	config.TransferMode  = SDIO_TRANSFER_MODE_BLOCK;
	config.DPSM          = SDIO_DPSM_ENABLE;
	SDIO_ConfigData(SDIO, &config);
}

static GFXINLINE void sd_turn_on()
{
	SDIO->CLKCR &= ~SDIO_CLKCR_CLKEN; // Disable clock
	SDIO_PowerState_ON(SDIO); // Power on
	SDIO->CLKCR |= SDIO_CLKCR_CLKEN; // Enable clock
	gfxSleepMilliseconds(2); // Sleep
}

static GFXINLINE void sd_turn_off()
{
	SDIO_PowerState_OFF(SDIO); // Power off
}

static GFXINLINE void sd_clear_flags()
{
	SDIO->ICR = SDIO_STATIC_FLAGS;
}

static GFXINLINE uint32_t sd_read_to_buffer(uint32_t* buffer)
{
	while (!(SDIO->STA & (SDIO_STA_RXOVERR | SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT | SDIO_STA_DBCKEND)))
	{
		if (SDIO->STA & SDIO_STA_RXDAVL) *buffer++ = SDIO->FIFO;
	}

	return (SDIO->STA & (SDIO_STA_RXOVERR | SDIO_STA_DCRCFAIL | SDIO_STA_DTIMEOUT));
}

#endif /* _GFILE_LLD_BOARD_H */
