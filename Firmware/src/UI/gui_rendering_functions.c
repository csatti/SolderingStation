/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.com/license.html
 */

#include "gui_priv.h"
#include "lzg.h"

// simple: single line with no wrapping
static coord_t getwidth(const char *text, font_t font, coord_t maxwidth) {
	(void) maxwidth;

	return gdispGetStringWidth(text, font)+2;		// Allow one pixel of padding on each side
}

// simple: single line with no wrapping
static coord_t getheight(const char *text, font_t font, coord_t maxwidth) {
	(void) text;
	(void) maxwidth;

	return gdispGetFontMetric(font, fontHeight);
}

/**********    LABEL    ********/

// macros to assist in data type conversions
#define gh2obj					((GLabelObject *)gh)
#define gw2obj					((GLabelObject *)gw)

static void gwinLabelDrawPM(GWidgetObject *gw, justify_t justify) {
	coord_t				w, h;
	color_t				c;
	GDisplay* 			pixmap;

	w = (gw->g.flags & GLABEL_FLG_WAUTO) ? getwidth(gw->text, gw->g.font, gdispGGetWidth(gw->g.display) - gw->g.x) : gw->g.width;
	h = (gw->g.flags & GLABEL_FLG_HAUTO) ? getheight(gw->text, gw->g.font, gdispGGetWidth(gw->g.display) - gw->g.x) : gw->g.height;
	c = (gw->g.flags & GWIN_FLG_SYSENABLED) ? gw->pstyle->enabled.text : gw->pstyle->disabled.text;

	if (gw->g.width != w || gw->g.height != h) {
		/* Only allow the widget to be resize if it will grow larger.
		 * Growing smaller is problematic because it requires a temporary change in visibility.
		 * This is a work-around for a crashing bug caused by calling gwinResize() in the draw function
		 * (dubious) as it may try to reclaim the drawing lock.
		 */
		if (gw->g.width < w || gw->g.height < h) {
			gwinResize(&gw->g, (w > gw->g.width ? w : gw->g.width), (h > gw->g.height ? h : gw->g.height));
			return;
		}
		w = gw->g.width;
		h = gw->g.height;
	}

	// Create a pixmap and get a pointer to the bits
	pixmap = gdispPixmapCreate(w, h);
	gdispGSetOrientation(pixmap, GDISP_ROTATE_0);    // default orientation

	#if GWIN_LABEL_ATTRIBUTE
		if (gw2obj->attr) {
			gdispGFillStringBox(pixmap, 0, 0, gw2obj->tab, h, gw2obj->attr, gw->g.font, c, gw->pstyle->background, justify);
			gdispGFillStringBox(pixmap, gw2obj->tab, 0, w-gw2obj->tab, h, gw->text, gw->g.font, c, gw->pstyle->background, justify);
		} else
			gdispGFillStringBox(pixmap, 0, 0, w, h, gw->text, gw->g.font, c, gw->pstyle->background, justify);
	#else
		gdispGFillStringBox(pixmap, 0, 0, w, h, gw->text, gw->g.font, c, gw->pstyle->background, justify);
	#endif

	// render the border (if any)
	if (gw->g.flags & GLABEL_FLG_BORDER)
		gdispGDrawBox(pixmap, 0, 0, w, h, (gw->g.flags & GWIN_FLG_SYSENABLED) ? gw->pstyle->enabled.edge : gw->pstyle->disabled.edge);

	doubleBufferBlitArea(pixmap, gw->g.x, gw->g.y, w, h);
}

void gwinLabelDrawJustifiedLeftPM(GWidgetObject *gw, void *param) {
	(void)param;

	gwinLabelDrawPM(gw, justifyLeft);
}

void gwinLabelDrawJustifiedRightPM(GWidgetObject *gw, void *param) {
	(void)param;

	gwinLabelDrawPM(gw, justifyRight);
}

void gwinLabelDrawJustifiedCenterPM(GWidgetObject *gw, void *param) {
	(void)param;

	gwinLabelDrawPM(gw, justifyCenter);
}

void gwinNoDraw(GWidgetObject *gw, void *param) {
	(void)gw;
	(void)param;

	gdispFillArea(gw->g.x, gw->g.y, gw->g.width, gw->g.height, gw->g.bgcolor);

}

#undef gh2obj
#undef gw2obj

/**********    PROGRESSBAR    ********/

// macros to assist in data type conversions
#define gh2obj					((GProgressbarObject *)gh)
#define gw2obj					((GProgressbarObject *)gw)

void gwinProgressbarDrawPM(GWidgetObject *gw, void *param) {
	#define gsw			((GProgressbarObject *)gw)

	const GColorSet *	pcol;
	GDisplay* 			pixmap;

	(void)				param;

	// get the colors right
	if ((gw->g.flags & GWIN_FLG_SYSENABLED))
		pcol = &gw->pstyle->enabled;
	else
		pcol = &gw->pstyle->disabled;

	// Create a pixmap and get a pointer to the bits
	pixmap = gdispPixmapCreate(gw->g.width, gw->g.height);
	gdispGSetOrientation(pixmap, GDISP_ROTATE_0);    // default orientation

	// Vertical progressbar
	if (gw->g.width < gw->g.height) {
		if (gsw->dpos != gw->g.height-1)
			gdispGFillArea(pixmap, 0, gsw->dpos, gw->g.width, gw->g.height - gsw->dpos, pcol->progress);				// Active Area
		if (gsw->dpos != 0)
			gdispGFillArea(pixmap, 0, 0, gw->g.width, gsw->dpos, pcol->fill);											// Inactive area
		gdispGDrawBox(pixmap, 0, 0, gw->g.width, gw->g.height, pcol->edge);												// Edge
		gdispGDrawLine(pixmap, 0, gsw->dpos, gw->g.width-1, gsw->dpos, pcol->edge);					// Thumb

	// Horizontal progressbar
	} else {
		if (gsw->dpos != gw->g.width-1)
			gdispGFillArea(pixmap, gsw->dpos, 0, gw->g.width-gsw->dpos, gw->g.height, pcol->fill);						// Inactive area
		if (gsw->dpos != 0)
			gdispGFillArea(pixmap, 0, 0, gsw->dpos, gw->g.height, pcol->progress);										// Active Area
		gdispGDrawBox(pixmap, 0, 0, gw->g.width, gw->g.height, pcol->edge);												// Edge
		gdispGDrawLine(pixmap, gsw->dpos, 0, gsw->dpos, gw->g.height-1, pcol->edge);					// Thumb
	}
	gdispGDrawStringBox(pixmap, 1, 1, gw->g.width-2, gw->g.height-2, gw->text, gw->g.font, pcol->text, justifyCenter);

	doubleBufferBlitArea(pixmap, gw->g.x, gw->g.y, gw->g.width, gw->g.height);

	#undef gsw
}

void gwinProgressbarDrawPowerBarPM(GWidgetObject *gw, void *param) {
	#define gsw			((GProgressbarObject *)gw)

	const ImageDesc_t* 	backImage = &IMG_POWERBAR_PNG;
	color_t				pcol = HTML2COLOR(0xB2B2B2);
	color_t				gcol = HTML2COLOR(0x606060);
	uint32_t 			pos;
	GDisplay* 			pixmap;
	pixel_t* 			surface;

	(void)				param;


	// Create a pixmap and get a pointer to the bits
	pixmap = gdispPixmapCreate(gw->g.width, gw->g.height);
	gdispGSetOrientation(pixmap, GDISP_ROTATE_0);    // default orientation
	surface = gdispPixmapGetBits(pixmap);

	if (backImage->format == IMAGE_RAW) {
		gdispGBlitArea(pixmap, 0, 0, gw->g.width, gw->g.height, 0, 0, gw->g.width,
				(pixel_t*) backImage->image);
	} else if (backImage->format == IMAGE_LZG) {
		LZG_Decode((void*) backImage->image, backImage->len, (void*) surface,
				gw->g.width * gw->g.height * sizeof(color_t));
	}

	pos = (gsw->dpos * 30) / 32;

	gdispGFillArea(pixmap, 10 + pos, 10, gw->g.width - (20 + pos), gw->g.height - 20, pcol);
	gdispGDrawLine(pixmap, 9, gw->g.height - 10, gw->g.width - 11, gw->g.height - 10, gcol);

	for (int i = 0; i < 11; i++) {
		int offs = (i * (gw->g.width - 20)) / 10;
		int offsa = (gw->g.width - 20) / 20;
		gdispGDrawLine(pixmap, 9 + offs, 16, 9 + offs, gw->g.height - 10, gcol);
		if (i) gdispGDrawLine(pixmap, 9 + offs - offsa, 20, 9 + offs - offsa, gw->g.height - 10, gcol);
	}

	doubleBufferBlitArea(pixmap, gw->g.x, gw->g.y, gw->g.width, gw->g.height);

	#undef gsw
}

#undef gh2obj
#undef gw2obj

/**********    SCOPE    ********/

// macros to assist in data type conversions
#define gw2obj					((WrapperObject_t *)gw)

void scopeDrawPM(GWidgetObject* gw, void* param)
{
	#define MAX_SURFACE	15000

	color_t	gcol = HTML2COLOR(0xB2B2B2);
	color_t	mcol = HTML2COLOR(0x00F0F0);
	color_t	bgcol = HTML2COLOR(0x303030);
	color_t	vcol = HTML2COLOR(0x30F030);
	color_t	icol = HTML2COLOR(0xF03030);
	coord_t	w, h, wstep, j;
	uint8_t* voltage = (uint8_t*)pu8VoltageScope;
	uint8_t* current = (uint8_t*)pu8CurrentScope;
	GDisplay* 			pixmap;

	(void)param;

	w = gw->g.width;
	h = gw->g.height;

	wstep = MAX_SURFACE / h;
	j = 0;

	processLockScope();
	for (coord_t i = 0; i < w; i += wstep)
	{
		coord_t wr = ((i + wstep < w) ? wstep : w - i);
		coord_t wp = 0;
		// Create a pixmap and get a pointer to the bits
		pixmap = gdispPixmapCreate(wr, h);
		gdispGSetOrientation(pixmap, GDISP_ROTATE_0);    // default orientation
		gdispGFillArea(pixmap, 0, 0, wr, h, bgcol);
		gdispGDrawLine(pixmap, 0, h - 1, wr - 1, h - 1, gcol);
		// 20 V, 10 A (approx)
		gdispGDrawLine(pixmap, 0, h - 51, wr - 1, h - 51, gcol);
		// 40 V, 20 A
		gdispGDrawLine(pixmap, 0, h - 101, wr - 1, h - 101, gcol);

		for (int k = 0; k < MARKER_NUM; k++)
		{
			if ((u16Marker[k] >= j) && (u16Marker[k] < i + wr)) {
				gdispGDrawLine(pixmap, u16Marker[k] - j, 0, u16Marker[k] - j, h - 2, mcol);
			}
		}


		for (;j < i + wr;j++)
		{
			if (j < SCOPE_LEN) {
				gdispGDrawPixel(pixmap, wp, h - *voltage++, vcol);
				gdispGDrawPixel(pixmap, wp, h - *current++, icol);
				wp++;
			}
		}

		doubleBufferBlitArea(pixmap, gw->g.x + i, gw->g.y, wr, h);
	}
	processSetScopeDataIsUsed();
	processUnlockScope();

}

void scopeLegendDrawPM(GWidgetObject* gw, void* param)
{
	color_t	gcol = HTML2COLOR(0xB2B2B2);
	color_t	bgcol = HTML2COLOR(0x303030);
	color_t	vcol = HTML2COLOR(0x30F030);
	color_t	icol = HTML2COLOR(0xF03030);
	coord_t	w, h;
	GDisplay* 			pixmap;

	(void)param;

	w = gw->g.width;
	h = gw->g.height;

	// Create a pixmap and get a pointer to the bits
	pixmap = gdispPixmapCreate(w, h);
	gdispGSetOrientation(pixmap, GDISP_ROTATE_0);    // default orientation

	gdispGFillArea(pixmap, 0, 0, w, h, bgcol);
	gdispGDrawLine(pixmap, w-1, 0, w - 1, h - 1, gcol);
	gdispGDrawStringBox(pixmap, 1, h - 68, gw->g.width - 4, 16, "20 V", gw->g.font, vcol, justifyRight);
	gdispGDrawStringBox(pixmap, 1, h - 51, gw->g.width - 4, 16, "10 A", gw->g.font, icol, justifyRight);
	gdispGDrawStringBox(pixmap, 1, h - 118, gw->g.width - 4, 16, "40 V", gw->g.font, vcol, justifyRight);
	gdispGDrawStringBox(pixmap, 1, h - 101, gw->g.width - 4, 16, "20 A", gw->g.font, icol, justifyRight);

	doubleBufferBlitArea(pixmap, gw->g.x, gw->g.y, w, h);
}

#undef gw2obj
