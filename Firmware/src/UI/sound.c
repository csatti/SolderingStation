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

#include "sound.h"
#include "main.h"

#define MESSAGE_QUEUE_LENGTH                    ( 100 )
#define MESSAGE_QUEUE_SOUND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 3 )

static QueueHandle_t _qhMessageQueue = NULL;
static TaskHandle_t _thSoundTask = NULL;

static void _soundTask(void *pvParameters)
{
	void* msg;

	while (1) {
		xQueueReceive(_qhMessageQueue, &msg, portMAX_DELAY);

		uint32_t snd = (uint32_t) msg;
		uint16_t freq = (uint16_t)(snd >> 16);
		uint16_t time = (uint16_t)(snd & 0xFFFF);

		boardBuzzerChangeFreq(freq);
		vTaskDelay(pdMS_TO_TICKS(time));
		boardBuzzerChangeFreq(0);
		vTaskDelay(pdMS_TO_TICKS(20));

		//if (uxQueueMessagesWaiting(_qhMessageQueue) == 0) boardBuzzerChangeFreq(0);
	}
}

void soundInit(void)
{
	boardBuzzerConfig();

	xTaskCreate(_soundTask, ( char * ) "Sound",	configMINIMAL_STACK_SIZE, NULL, MESSAGE_QUEUE_SOUND_TASK_PRIORITY, &_thSoundTask );
	_qhMessageQueue = xQueueCreate(MESSAGE_QUEUE_LENGTH, sizeof( uint32_t ) );
	vQueueAddToRegistry(_qhMessageQueue, TXT("SndQ"));
}

void soundBeep(uint16_t freq, uint16_t time)
{
	uint32_t msg, sound;
	ConfigOnline_t* ConfigOnline;

	msg = (((uint32_t)freq) << 16) | ((uint32_t)time);

	ConfigOnline = configGetConfigOnlineNoWrite();
	sound = ConfigOnline->sound;

	if (sound) xQueueSend( _qhMessageQueue, (void*)&msg, 0 );
}

