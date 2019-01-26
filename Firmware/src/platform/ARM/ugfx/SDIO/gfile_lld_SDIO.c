/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* This driver is updated to support a DMA driven SDIO with STM32F4 advanced/foundation lines using Low Layer.
 * Modified by Attila Kovács, 2018.
 */

#include "gfx.h"

#if defined(GFX_USE_GFILE)

#include "board_SDIO.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

static uint8_t			cardversion, cardtype, cardcc;
static uint8_t			boardinit;
static uint32_t 		CID[4];
static uint32_t 		CSD[4];
static uint32_t 		rca;

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define SDTYPE_SDSC				0x01
#define SDTYPE_SDHC_XC			0x02
#define SDVERSION_UNKNOWN		0x00
#define SDVERSION_V1			0x01
#define SDVERSION_V2			0x02


/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

bool_t gfile_sd_card_init(void) {
	uint32_t returncode, response, busy, tries;
	uint16_t sd_rca = 1;
	uint32_t tmpscr[2];
	
	cardversion = SDVERSION_UNKNOWN;

	if (!boardinit) {
		init_board_sdio();
		boardinit = TRUE;
	}

	sd_init_400khz();
	sd_turn_on();
	returncode = sd_cmd0();  // Go idle
	if (returncode) {
		return FALSE;
	}
	returncode = sd_cmd8();  // Check interface conditions, only supported in V2.x
	if (returncode) {
		cardversion = SDVERSION_V1;
		tries = 0;
		do
		{
			if (tries++ > 100) {
				return FALSE;
			}
			returncode = sd_cmd55(0); // Next command is application specific command
			if (returncode) {
				return FALSE;
			}
			returncode = sd_cmd41(SDMMC_STD_CAPACITY);
			if (returncode) {
				return FALSE;
			}
			response = sd_r1();
			busy = !(response & (1U << 31));
			if (busy) gfxSleepMilliseconds(1); // Sleep
		}
		while (busy);
		cardtype = SDTYPE_SDSC;
	}
	else
	{
		cardversion = SDVERSION_V2;
		tries = 0;
		do
		{
			if (tries++ > 100) {
				return FALSE;
			}
			returncode = sd_cmd55(0); // Next command is application specific command
			if (returncode) {
				return FALSE;
			}
			returncode = sd_cmd41(SDMMC_HIGH_CAPACITY);
			if (returncode) {
				return FALSE;
			}
			response = sd_r1();
			busy = !(response & (1U << 31));
			if (busy) gfxSleepMilliseconds(1); // Sleep
		}
		while (busy);
		cardtype = (response & SDMMC_HIGH_CAPACITY)?SDTYPE_SDHC_XC:SDTYPE_SDSC;
	}

	returncode = sd_cmd2(); // Get CID (Card IDentification number)
	if (returncode) {
		return FALSE;
	}
	CID[0] = sd_r1();
	CID[1] = sd_r2();
	CID[2] = sd_r3();
	CID[3] = sd_r4();

	returncode = sd_cmd3(&sd_rca); // Set relative card address
	if (returncode) {
		return FALSE;
	}

	rca = sd_rca;

	returncode = sd_cmd9((uint32_t)(rca << 16U)); // Get CSD (Card Specific Data table)
	if (returncode) {
		return FALSE;
	}

	CSD[0] = sd_r1();
	CSD[1] = sd_r2();
	CSD[2] = sd_r3();
	CSD[3] = sd_r4();

	cardcc = (uint8_t)(CSD[1] >> 20); // Supported card command class

	returncode = sd_cmd7((uint32_t)(rca << 16U)); // Select card
	if (returncode) {
		return FALSE;
	}

	// Set wide bus if possible
	if (!(sd_r1() & SDMMC_CARD_LOCKED)) {
		returncode = sd_cmd16(8);
		if (returncode) {
			return FALSE;
		}

		returncode = sd_cmd55((uint32_t)(rca << 16U)); // Next command is application specific command
		if (returncode) {
			return FALSE;
		}

		sd_data_init(8, SDIO_DATABLOCK_SIZE_8B);
		returncode = sd_cmd51(); // Get SCR
		if (returncode) {
			return FALSE;
		}

		returncode = sd_read_to_buffer(tmpscr);
		if (returncode) {
			return FALSE;
		}

		sd_clear_flags();

		if (tmpscr[0] & 0x00000400) { // Wide bus supported?
			returncode = sd_cmd55((uint32_t)(rca << 16U)); // Next command is application specific command
			if (returncode) {
				return FALSE;
			}
			returncode = sd_cmd6(2);
			if (returncode) {
				return FALSE;
			}
			sd_init_high_speed();
		}

	}

	return TRUE;
}


#endif /* GFX_USE_GFILE */
