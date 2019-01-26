/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* This driver is updated to support a DMA driven ILI9341 TFT panel with STM32F4 advanced/foundation lines using Low Layer.
 * Modified by Attila Kovács, 2018.
 */

#include "gfx.h"
#include "board_ILI9341.h"

#if GFX_USE_GDISP

#if defined(GDISP_SCREEN_HEIGHT) || defined(GDISP_SCREEN_HEIGHT)
	#if GFX_COMPILER_WARNING_TYPE == GFX_COMPILER_WARNING_DIRECT
		#warning "GDISP: This low level driver does not support setting a screen size. It is being ignored."
	#elif GFX_COMPILER_WARNING_TYPE == GFX_COMPILER_WARNING_MACRO
		COMPILER_WARNING("GDISP: This low level driver does not support setting a screen size. It is being ignored.")
	#endif
	#undef GDISP_SCREEN_WIDTH
	#undef GDISP_SCREEN_HEIGHT
#endif

#define GDISP_DRIVER_VMT			GDISPVMT_ILI9341
#include "gdisp_lld_config.h"
#include "../../../src/gdisp/gdisp_driver.h"

#include "board_ILI9341.h"

/*===========================================================================*/
/* Driver local definitions.                                                 */
/*===========================================================================*/

#ifndef GDISP_SCREEN_HEIGHT
	#define GDISP_SCREEN_HEIGHT		320
#endif
#ifndef GDISP_SCREEN_WIDTH
	#define GDISP_SCREEN_WIDTH		240
#endif
#ifndef GDISP_INITIAL_CONTRAST
	#define GDISP_INITIAL_CONTRAST	50
#endif
#ifndef GDISP_INITIAL_BACKLIGHT
	#define GDISP_INITIAL_BACKLIGHT	0
#endif

#include "ILI9341.h"

/*===========================================================================*/
/* Driver local functions.                                                   */
/*===========================================================================*/

// Some common routines and macros
#define dummy_read(g)				{ volatile uint16_t dummy; dummy = read_data(g); (void) dummy; }
#define write_reg(g, reg, data)		{ write_index(g, reg); write_data(g, data); }
#define write_data16(g, data)		{ write_data(g, data >> 8); write_data(g, (uint8_t)data); }
#define delay(us)					gfxSleepMicroseconds(us)
#define delayms(ms)					gfxSleepMilliseconds(ms)

static void set_viewport(GDisplay *g) {
	write_index(g, 0x2A);
	write_data(g, (g->p.x >> 8));
	write_data(g, (uint8_t) g->p.x);
	write_data(g, (g->p.x + g->p.cx - 1) >> 8);
	write_data(g, (uint8_t) (g->p.x + g->p.cx - 1));

	write_index(g, 0x2B);
	write_data(g, (g->p.y >> 8));
	write_data(g, (uint8_t) g->p.y);
	write_data(g, (g->p.y + g->p.cy - 1) >> 8);
	write_data(g, (uint8_t) (g->p.y + g->p.cy - 1));
}

static void set_gammacurve(GDisplay *g, const ILI9341_GammaCurve_t* gammacurve)
{
	write_index(g, ILI9341_POSGAMMACORRECTION_REG);
	write_data(g, gammacurve->VP63);
	write_data(g, gammacurve->VP62);
	write_data(g, gammacurve->VP61);
	write_data(g, gammacurve->VP59);
	write_data(g, gammacurve->VP57);
	write_data(g, gammacurve->VP50);
	write_data(g, gammacurve->VP43);
	write_data(g, (gammacurve->VP27 << 4) | gammacurve->VP36);
	write_data(g, gammacurve->VP20);
	write_data(g, gammacurve->VP13);
	write_data(g, gammacurve->VP6);
	write_data(g, gammacurve->VP4);
	write_data(g, gammacurve->VP2);
	write_data(g, gammacurve->VP1);
	write_data(g, gammacurve->VP0);

	write_index(g, ILI9341_NEGGAMMACORRECTION_REG);
	write_data(g, gammacurve->VN63);
	write_data(g, gammacurve->VN62);
	write_data(g, gammacurve->VN61);
	write_data(g, gammacurve->VN59);
	write_data(g, gammacurve->VN57);
	write_data(g, gammacurve->VN50);
	write_data(g, gammacurve->VN43);
	write_data(g, (gammacurve->VN36 << 4) | gammacurve->VN27);
	write_data(g, gammacurve->VN20);
	write_data(g, gammacurve->VN13);
	write_data(g, gammacurve->VN6);
	write_data(g, gammacurve->VN4);
	write_data(g, gammacurve->VN2);
	write_data(g, gammacurve->VN1);
	write_data(g, gammacurve->VN0);
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

LLDSPEC bool_t gdisp_lld_init(GDisplay *g) {
	// No private area for this controller
	g->priv = 0;

	// Initialise the board interface
	init_board(g);

	// Hardware reset
	setpin_reset(g, TRUE);
	gfxSleepMilliseconds(20);
	setpin_reset(g, FALSE);
	gfxSleepMilliseconds(20);

	// Get the bus for the following initialisation commands
	acquire_bus(g);

	write_index(g, ILI9341_SOFT_RESET_REG); //software reset
	gfxSleepMilliseconds(5);
	write_index(g, 0x28);
	// display off
	//---------------------------------------------------------
	// magic?
	write_index(g, 0xef);
	write_data(g, 0x03);
	write_data(g, 0x80);
	write_data(g, 0x02);

	write_index(g, ILI9341_POWERCTLB_REG);
	write_data(g, 0x00);
	write_data(g, 0x81); // 0x83? 0xC1?
	write_data(g, 0x30);

	write_index(g, ILI9341_POWONSEQCTL_REG);
	write_data(g, 0x64);
	write_data(g, 0x03);
	write_data(g, 0x12);
	write_data(g, 0x81);

	write_index(g, ILI9341_DIVTIMCTL_A_REG);
	write_data(g, 0x85);
	write_data(g, 0x00); // 0x01?
	write_data(g, 0x78); // 0x79?

	write_index(g, ILI9341_POWERCTLA_REG);
	write_data(g, 0x39);
	write_data(g, 0x2c);
	write_data(g, 0x00);
	write_data(g, 0x34);
	write_data(g, 0x02);

	write_index(g, ILI9341_PUMPRATIOCTL_REG);
	write_data(g, 0x20);

	write_index(g, ILI9341_DIVTIMCTL_B_REG);
	write_data(g, 0x00);
	write_data(g, 0x00);

	//------------power control------------------------------
	write_index(g, ILI9341_POWERCTL1_REG); //power control
	write_data(g, 0x23); // 0x26?

	write_index(g, ILI9341_POWERCTL2_REG); //power control
	write_data(g, 0x10); // 0x11?

	//--------------VCOM
	write_index(g, ILI9341_VCOMCTL1_REG); //vcom control
	write_data(g, 0x3e); // 4.25V 0x35?
	write_data(g, 0x28); // -1.5V 0x3E?

	write_index(g, ILI9341_VCOMCTL2_REG); //vcom control
	write_data(g, 0x86); // 0xBE? 0x94

	//------------memory access control------------------------
	write_index(g, ILI9341_MEMACCESS_REG);
	// memory access control
	write_data(g, ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR); //0048 my,mx,mv,ml,BGR,mh,0.0

	write_index(g, ILI9341_PIXFORMATSET_REG); // pixel format set
	write_data(g, 0x55);//16bit /pixel
	//----------------- frame rate------------------------------
	write_index(g, ILI9341_FRAMECTL_NOR_REG);
	// frame rate
	write_data(g, 0x00);
	write_data(g, 0x18); // 0x1B? //70

	write_index(g, ILI9341_FUNCTONCTL_REG);    // Display Function Control
	write_data(g, 0x08); // 0x0A
	write_data(g, 0x82);
	write_data(g, 0x27);

	//----------------Gamma---------------------------------
	write_index(g, ILI9341_ENABLE_3G_REG); // 3Gamma Function Disable
	write_data(g, 0x00); // 0x08

	write_index(g, ILI9341_GAMMASET_REG);
	write_data(g, 0x01); // gamma set 4 gamma curve 01/02/04/08

	set_gammacurve(g, &ILI9341_defaultGammaCurve);

	//--------------ddram ---------------------
	write_index(g, ILI9341_COLADDRSET_REG);
	// column set
	// size = 239
	write_data(g, 0x00);
	write_data(g, 0x00);
	write_data(g, 0x00);
	write_data(g, 0xEF);

	write_index(g, ILI9341_PAGEADDRSET_REG);
	// page address set
	// size = 319
	write_data(g, 0x00);
	write_data(g, 0x00);
	write_data(g, 0x01);
	write_data(g, 0x3F);

	// write_index(g, 0x34);
	//write_index(g, 0x35);
	// tearing effect off
	// tearing effect on
	// write_index(g, 0xb4); // display inversion
	// write_data(g, 0x00);

	write_index(g, ILI9341_ENTRYMODE_REG); //entry mode set
	write_data(g, 0x07);

	//-----------------display---------------------
	write_index(g, ILI9341_FUNCTONCTL_REG);
	// display function control
	write_data(g, 0x08); // 0x0a
	write_data(g, 0x82);
	write_data(g, 0x27);
	write_data(g, 0x00);

	write_index(g, ILI9341_SLEEP_OUT_REG); //sleep out
	gfxSleepMilliseconds(100);
	write_index(g, ILI9341_DISPLAYON_REG); // display on
	gfxSleepMilliseconds(100);

    // Finish Init
    post_init_board(g);

 	// Release the bus
	release_bus(g);
	
	/* Turn on the back-light */
	set_backlight(g, GDISP_INITIAL_BACKLIGHT);

	/* Initialise the GDISP structure */
	g->g.Width = GDISP_SCREEN_WIDTH;
	g->g.Height = GDISP_SCREEN_HEIGHT;
	g->g.Orientation = GDISP_ROTATE_0;
	g->g.Powermode = powerOn;
	g->g.Backlight = GDISP_INITIAL_BACKLIGHT;
	g->g.Contrast = GDISP_INITIAL_CONTRAST;

	g->p.color = GFX_BLACK;

	gdisp_lld_fill_area(g);
	return TRUE;
}

#if GDISP_HARDWARE_STREAM_WRITE
	LLDSPEC	void gdisp_lld_write_start(GDisplay *g) {
		acquire_bus(g);
		set_viewport(g);
		write_index(g, 0x2C);
	}
	LLDSPEC	void gdisp_lld_write_color(GDisplay *g) {
		write_data16(g, gdispColor2Native(g->p.color));
	}
	LLDSPEC	void gdisp_lld_write_stop(GDisplay *g) {
		release_bus(g);
	}
#endif

#if GDISP_HARDWARE_STREAM_READ
	LLDSPEC	void gdisp_lld_read_start(GDisplay *g) {
		acquire_bus(g);
		set_viewport(g);
		write_index(g, 0x2E);
		setreadmode(g);
		dummy_read(g);
	}
	LLDSPEC	color_t gdisp_lld_read_color(GDisplay *g) {
		uint16_t	data;

		data = read_data(g);
		return gdispNative2Color(data);
	}
	LLDSPEC	void gdisp_lld_read_stop(GDisplay *g) {
		setwritemode(g);
		release_bus(g);
	}
#endif

#if GDISP_HARDWARE_FILLS && GDISP_USE_DMA
	LLDSPEC void gdisp_lld_fill_area(GDisplay *g) {
		#if GDISP_NO_DMA_FROM_STACK
			static LLDCOLOR_TYPE	c;
		#else
			LLDCOLOR_TYPE	c;
		#endif

		c = gdispColor2Native(g->p.color);
		acquire_bus(g);
		set_viewport(g);
		write_index(g, 0x2C);
		dma_with_noinc(g, &c, g->p.cx*g->p.cy);
		release_bus(g);
	}
#else
	#warning "DMA is not used for ILI9341 HW filling."
#endif

#if GDISP_HARDWARE_BITFILLS && GDISP_USE_DMA
	#if GDISP_PIXELFORMAT != GDISP_LLD_PIXELFORMAT
		#error "GDISP: ILI9341: BitBlit is only available in RGB565 pixel format"
	#endif

	LLDSPEC void gdisp_lld_blit_area(GDisplay *g) {
		pixel_t		*buffer;
		coord_t		ycnt;

		buffer = (pixel_t *)g->p.ptr + g->p.x1 + g->p.y1 * g->p.x2;

		acquire_bus(g);
		set_viewport(g);
		write_index(g, 0x2C);
		if (g->p.x2 == g->p.cx) {
			dma_with_inc(g, buffer, g->p.cx*g->p.cy);
		} else {
			for (ycnt = g->p.cy; ycnt; ycnt--, buffer += g->p.x2)
				dma_with_inc(g, buffer, g->p.cy);
		}
		release_bus(g);
	}
#else
	#warning "DMA is not used for ILI9341 HW copy."
#endif

LLDSPEC	color_t gdisp_lld_get_pixel_color(GDisplay* g) {

	return GFX_BLACK;
}

LLDSPEC void gdisp_lld_control(GDisplay *g) {
	switch(g->p.x) {
	case GDISP_CONTROL_POWER:
		if (g->g.Powermode == (powermode_t)g->p.ptr)
			return;
		switch((powermode_t)g->p.ptr) {
		case powerOff:
		case powerSleep:
		case powerDeepSleep:
			acquire_bus(g);
			write_reg(g, 0x10, 0x00);	/* enter sleep mode */
			release_bus(g);
			break;
		case powerOn:
			acquire_bus(g);
			write_index(g, 0x11);	/* leave sleep mode */
			release_bus(g);
			gfxSleepMilliseconds(5);
			break;
		default:
			return;
		}
		g->g.Powermode = (powermode_t)g->p.ptr;
		return;

	case GDISP_CONTROL_ORIENTATION:
		if (g->g.Orientation == (orientation_t)g->p.ptr)
			return;
		switch((orientation_t)g->p.ptr) {
		case GDISP_ROTATE_0:
			acquire_bus(g);
			write_reg(g, ILI9341_MEMACCESS_REG, 0x48);	/* X and Y axes non-inverted */
			release_bus(g);
			g->g.Height = GDISP_SCREEN_HEIGHT;
			g->g.Width = GDISP_SCREEN_WIDTH;
			break;
		case GDISP_ROTATE_90:
			acquire_bus(g);
			write_reg(g, ILI9341_MEMACCESS_REG, 0xE8);	/* Invert X and Y axes */
			release_bus(g);
			g->g.Height = GDISP_SCREEN_WIDTH;
			g->g.Width = GDISP_SCREEN_HEIGHT;
			break;
		case GDISP_ROTATE_180:
			acquire_bus(g);
			write_reg(g, ILI9341_MEMACCESS_REG, 0x88);		/* X and Y axes non-inverted */
			release_bus(g);
			g->g.Height = GDISP_SCREEN_HEIGHT;
			g->g.Width = GDISP_SCREEN_WIDTH;
			break;
		case GDISP_ROTATE_270:
			acquire_bus(g);
			write_reg(g, ILI9341_MEMACCESS_REG, 0x28);	/* Invert X and Y axes */
			release_bus(g);
			g->g.Height = GDISP_SCREEN_WIDTH;
			g->g.Width = GDISP_SCREEN_HEIGHT;
			break;
		default:
			return;
		}
		g->g.Orientation = (orientation_t)g->p.ptr;
		return;

	case GDISP_CONTROL_BACKLIGHT:
		if ((unsigned)g->p.ptr > 100)
			g->p.ptr = (void *)100;
		set_backlight(g, (unsigned)g->p.ptr);
		g->g.Backlight = (unsigned)g->p.ptr;
		return;

	case GDISP_CONTROL_GAMMACURVE:
		if (g->p.ptr == NULL) return;
		acquire_bus(g);
		set_gammacurve(g, (ILI9341_GammaCurve_t*)g->p.ptr);
		release_bus(g);
		return;

	//case GDISP_CONTROL_CONTRAST:
	default:
		return;
	}
}

#endif /* GFX_USE_GDISP */
