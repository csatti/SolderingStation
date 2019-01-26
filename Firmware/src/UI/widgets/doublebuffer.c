/* Copyright (C) 2018, Attila Kovács
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

#include "doublebuffer.h"
#include "main.h"

static QueueHandle_t _qhBufferQueue = NULL;
static TaskHandle_t _thDrawTask = NULL;

static void _drawTask(void *pvParameters)
{
	while (1) {
		Buffer_t buffer;
		pixel_t* surface;

		xQueueReceive(_qhBufferQueue, &buffer, portMAX_DELAY);

		surface = gdispPixmapGetBits(buffer.pixmap);
		gdispBlitArea(buffer.x, buffer.y, buffer.w, buffer.h, surface);
		gdispPixmapDelete(buffer.pixmap);
	}
}


void doubleBufferInit(void)
{
	_qhBufferQueue = xQueueCreate(BUFFER_QUEUE_LENGTH, sizeof( Buffer_t ) );
	vQueueAddToRegistry(_qhBufferQueue, TXT("DBufQ"));
	xTaskCreate(_drawTask, ( char * ) "DoubeBuf", 120, NULL, BUFFER_TASK_PRIORITY, &_thDrawTask );
}

void doubleBufferDraw(Buffer_t *buffer)
{
	xQueueSendToBack( _qhBufferQueue, (void*)buffer, portMAX_DELAY );
}

void doubleBufferBlitArea(GDisplay* g, coord_t x, coord_t y, coord_t cx, coord_t cy)
{
	Buffer_t buffer;

	buffer.pixmap = g;
	buffer.x = x;
	buffer.y = y;
	buffer.w = cx;
	buffer.h = cy;

#ifdef PLATFORM_ARM
	doubleBufferDraw(&buffer);
#else
	pixel_t* surface;
	surface = gdispPixmapGetBits(buffer.pixmap);
	gdispBlitArea(buffer.x, buffer.y, buffer.w, buffer.h, surface);
	gdispPixmapDelete(buffer.pixmap);
#endif

}
