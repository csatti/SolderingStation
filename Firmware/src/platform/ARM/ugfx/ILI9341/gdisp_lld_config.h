/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/* This driver is updated to support a DMA driven ILI9341 TFT panel.
 * Modified by Attila Kovács, 2018.
 */

#ifndef __GDISP_LLD_CONFIG_H
#define __GDISP_LLD_CONFIG_H

#include "gfxconf.h"

#if GFX_USE_GDISP

/*===========================================================================*/
/* Driver hardware support.                                                  */
/*===========================================================================*/

#define GDISP_HARDWARE_STREAM_WRITE		GFXON
#define GDISP_HARDWARE_STREAM_READ		GFXON // TODO: Implement it
#define GDISP_HARDWARE_PIXELREAD		GFXON
#define GDISP_HARDWARE_CONTROL			GFXON

#define GDISP_LLD_PIXELFORMAT			GDISP_PIXELFORMAT_RGB565

//#define GDISP_USE_DMA					GFXON
#define GDISP_NO_DMA_FROM_STACK			GFXON

#endif	/* GFX_USE_GDISP */

#endif	/* _GDISP_LLD_CONFIG_H */
