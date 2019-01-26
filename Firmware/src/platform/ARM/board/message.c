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

#include "message.h"

#include "main.h"
#include <string.h>

#define MESSAGE_QUEUE_LENGTH                    ( 10 )
#define MAX_DEBUG_MESSAGE						255
#define MESSAGE_QUEUE_SEND_TASK_PRIORITY        ( tskIDLE_PRIORITY + 2 )

static char* _pszCurrentDebugMessage;
static uint8_t* _pu8CurrentDebugMessageHead;
static QueueHandle_t _qhMessageQueue = NULL;
static TaskHandle_t _thSendTask = NULL;
static uint8_t _u8Init = 0;
static uint8_t _u8Telemetry = 0, _u8TelemetryHeader = 0;

static volatile SemaphoreHandle_t _shMutexDBG  = NULL;
#define MUTEX_ENTER()	xSemaphoreTake(_shMutexDBG, portMAX_DELAY)
#define MUTEX_EXIT()	xSemaphoreGive(_shMutexDBG)

static void _checkNewMessage(void)
{
	if (!_u8Init) return;
	if (xSemaphoreGetMutexHolder(_shMutexDBG) != xTaskGetCurrentTaskHandle()) MUTEX_ENTER();
	if (_pszCurrentDebugMessage != NULL) return;
	_pu8CurrentDebugMessageHead = (uint8_t*) pvPortMallocDMA(MAX_DEBUG_MESSAGE /* + sizeof(DBG_Cmd_t)*/);
	_pszCurrentDebugMessage = (char*)(_pu8CurrentDebugMessageHead /*+ sizeof(DBG_Cmd_t)*/); // No header yet
	//TODO: Add command header to the front, normal text messages will do for now
	*_pszCurrentDebugMessage = 0;
}

ISR(USART_DEBUG_Tx_DMA_IRQHandler)
{
	BaseType_t higherPriorityTaskWoken;
	USART_DEBUG_Tx_DMA_ClearTCFlag();
	LL_DMA_DisableStream(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream);
	LL_DMA_DisableIT_TC(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream);
	if (_thSendTask != NULL) {
		vTaskNotifyGiveFromISR(_thSendTask, &higherPriorityTaskWoken);
		portYIELD_FROM_ISR( higherPriorityTaskWoken );
	}
	__DSB();
}

static void _sendTask(void *pvParameters)
{
	void* msg;
	const TickType_t maxBlockTime = pdMS_TO_TICKS( 500 );
	size_t l;

	while (1) {
		xQueueReceive(_qhMessageQueue, &msg, portMAX_DELAY);

		//TODO: string length will do for now, later proper header
		char* str = (char*) msg;

		if (_u8Telemetry) {
			l = (uint8_t)(*str++);
		}
		else
			l = strlen(str);
		USART_DEBUG_Tx_DMA_ClearTCFlag();
		LL_DMA_DisableStream(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream);
		LL_DMA_SetDataLength(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream, l);
		LL_DMA_SetMemoryAddress(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream, (uint32_t) str);
		LL_DMA_EnableIT_TC(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream);
		LL_DMA_EnableStream(USART_DEBUG_DMA, USART_DEBUG_Tx_DMA_Stream);
		ulTaskNotifyTake(pdFALSE, maxBlockTime);
		vPortFreeDMA(msg);
	}
}

static void _sendDebugMessage(void)
{
	BaseType_t res = xQueueSend( _qhMessageQueue, &_pu8CurrentDebugMessageHead, 0 );
	if (res != pdPASS) {
		vPortFreeDMA(_pu8CurrentDebugMessageHead);
	}
	_pszCurrentDebugMessage = NULL;
	MUTEX_EXIT();
}

void messageInit(void)
{
#ifdef USARTDEBUG
	boardUSARTDebugConfig(0);
	xTaskCreate(_sendTask, ( char * ) "DBGSend",	configMINIMAL_STACK_SIZE, NULL, MESSAGE_QUEUE_SEND_TASK_PRIORITY, &_thSendTask );
	_qhMessageQueue = xQueueCreate(MESSAGE_QUEUE_LENGTH, sizeof( _pszCurrentDebugMessage ) );
	_shMutexDBG = xSemaphoreCreateMutex();
	_u8Init = 1;
	vQueueAddToRegistry(_qhMessageQueue, TXT("DbgQ"));
	debugPrintLn(TXT("*** LOG STARTED ***"));
#endif
}

#ifdef USARTTELEMETRY
void telemetryHeader(void)
{

	if (!_u8Telemetry) return;
	if (!_u8Init) return;
	_checkNewMessage();
	_pszCurrentDebugMessage = (char*)(_pu8CurrentDebugMessageHead);
	*_pszCurrentDebugMessage++ = (char)1;
	*_pszCurrentDebugMessage++ = (char)0xAA;
	_u8TelemetryHeader = 1;
}

void telemetryU16(uint16_t data)
{
	uint16_t* d = (uint16_t*)_pszCurrentDebugMessage;

	if ((!_u8Telemetry) || (!_u8TelemetryHeader)) return;

	*_pu8CurrentDebugMessageHead += sizeof(uint16_t);
	*d = data;
	_pszCurrentDebugMessage += sizeof(uint16_t);
}

void telemetryFloat(float data)
{
	float* d = (float*)_pszCurrentDebugMessage;

	if ((!_u8Telemetry) || (!_u8TelemetryHeader)) return;

	*_pu8CurrentDebugMessageHead += sizeof(float);
	*d = data;
	_pszCurrentDebugMessage += sizeof(float);
}

void telemetrySend()
{
	if (!_u8Telemetry) return;
	_u8TelemetryHeader = 0;
	_sendDebugMessage();
}

void telemetryControl(uint8_t enable)
{
	_u8Telemetry = enable;
	_u8TelemetryHeader = 0;

	if ((!enable) && (_pszCurrentDebugMessage != NULL)) {
		_pszCurrentDebugMessage = (char*)(_pu8CurrentDebugMessageHead);
		*_pszCurrentDebugMessage = 0;
	}
}

uint8_t telemetryIsEnabled(void)
{
	return _u8Telemetry;
}

#endif

#ifdef USARTDEBUG
void debugPrint(const char *str)
{

	if (_u8Telemetry) return;
#ifndef SEGGER_JLINK_RTT
	if (!_u8Init) return;
	_checkNewMessage();
	strcat(_pszCurrentDebugMessage, str);
#else
	SEGGER_RTT_WriteString(0, str);
#endif

}

void debugPrintLn(const char *str)
{

	if (_u8Telemetry) return;
#ifndef SEGGER_JLINK_RTT
	if (!_u8Init) return;
	_checkNewMessage();
	strcat(_pszCurrentDebugMessage, str);
	strcat(_pszCurrentDebugMessage, TXT("\r\n"));
	_sendDebugMessage();
#else
	SEGGER_RTT_WriteString(0, str);
	SEGGER_RTT_WriteString(0, TXT("\r\n"));
#endif
}

/*void debugReceiveData(unsigned char *buff, size_t num)
 {
 LL_DMA_InitTypeDef DMA_InitStruct;

 if (!num) return;
 LOCK();

 // Configure receive DMA
 LL_DMA_DeInit(USART_DEBUG_DMA, USART_DEBUG_Rx_DMA_Stream);
 LL_DMA_StructInit(&DMA_InitStruct);
 DMA_InitStruct.PeriphOrM2MSrcAddress = (uint32_t)&USART_DEBUG->DR;//USART_DEBUG_DR_Base;
 DMA_InitStruct.MemoryOrM2MDstAddress = (uint32_t)buff;
 DMA_InitStruct.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
 DMA_InitStruct.NbData = num;
 DMA_InitStruct.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
 DMA_InitStruct.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
 DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
 DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
 DMA_InitStruct.Mode = LL_DMA_MODE_NORMAL;
 DMA_InitStruct.Priority = LL_DMA_PRIORITY_LOW;
 DMA_InitStruct.Channel = USART_DEBUG_Rx_DMA_Channel;
 DMA_InitStruct.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
 DMA_InitStruct.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_1_4;
 DMA_InitStruct.MemBurst = LL_DMA_MBURST_SINGLE;
 DMA_InitStruct.PeriphBurst = LL_DMA_PBURST_SINGLE;
 LL_DMA_Init(USART_DEBUG_DMA, USART_DEBUG_Rx_DMA_Stream, &DMA_InitStruct);
 LL_DMA_EnableIT_TC(USART_DEBUG_DMA, USART_DEBUG_Rx_DMA_Stream);
 LL_USART_EnableDMAReq_RX(USART_DEBUG);
 LL_USART_DisableIT_RXNE(USART_DEBUG);
 LL_DMA_EnableStream(USART_DEBUG_DMA, USART_DEBUG_Rx_DMA_Stream);
 _usartBufferPos = 0;
 _usartReceiving = 1;
 UNLOCK();
 }

ISR(USART_DEBUG_Rx_DMA_IRQHandler) {
	DISABLE_IRQ();
	USART_DEBUG_Rx_DMA_ClearTCFlag();
	LL_DMA_DisableStream(USART_DEBUG_DMA, USART_DEBUG_Rx_DMA_Stream);
	LL_DMA_DisableIT_TC(USART_DEBUG_DMA, USART_DEBUG_Rx_DMA_Stream);
	LL_USART_DisableDMAReq_RX(USART_DEBUG);
	LL_USART_EnableIT_RXNE(USART_DEBUG);
	ENABLE_IRQ();
	__DSB();
}*/

void debugPrintC(const char *str, uint16_t color) {

	if (_u8Telemetry) return;
#ifndef SEGGER_JLINK_RTT
	if (!_u8Init) return;
	char escstr[8] = "\x1B[36;1m";
	switch (color) {
	case LCD_BLACK:
		escstr[3] = '0';
		break;
	case LCD_RED:
		escstr[3] = '1';
		break;
	case LCD_GREEN:
		escstr[3] = '2';
		break;
	case LCD_YELLOW:
		escstr[3] = '3';
		break;
	case LCD_BLUE:
		escstr[3] = '4';
		break;
	case LCD_CYAN:
		escstr[3] = '6';
		break;
	case LCD_WHITE:
		escstr[3] = '7';
		break;
	default:
		escstr[3] = '5';
		break;
	}
	debugPrint(escstr);
	debugPrint(str);
	debugPrint(TXT("\x1B[0m"));
#else
	switch (color) {
		case LCD_BLACK:
		SEGGER_RTT_WriteString(0, RTT_CTRL_BG_BLACK);
		break;
		case LCD_RED:
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_RED);
		break;
		case LCD_GREEN:
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_GREEN);
		break;
		case LCD_YELLOW:
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_YELLOW);
		break;
		case LCD_BLUE:
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_BLUE);
		break;
		case LCD_CYAN:
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_CYAN);
		break;
		case LCD_WHITE:
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_WHITE);
		break;
		default:
		SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_BRIGHT_MAGENTA);
		break;
	}
	SEGGER_RTT_WriteString(0, str);
	SEGGER_RTT_WriteString(0, RTT_CTRL_TEXT_WHITE);
#endif
}

void debugPrintLnC(const char *str, uint16_t color) {
	debugPrintC(str, color);

	if (_u8Telemetry) return;
#ifndef SEGGER_JLINK_RTT
	if (!_u8Init) return;
	strcat(_pszCurrentDebugMessage, TXT("\r\n"));
	_sendDebugMessage();
#else
	SEGGER_RTT_WriteString(0, TXT("\r\n"));
#endif
}


void debugTimeRef(void)
{
	char str[20];

	if (_u8Telemetry) return;
	if (!_u8Init) return;

	TickType_t tick = xTaskGetTickCount();

	ltoa(tick, str, 10);
	debugPrint(TXT("["));
	strpadleft(str, ' ', 8);
	debugPrintC(str, LCD_YELLOW);
	debugPrint(TXT("]: "));
	strcpy(str, pcTaskGetName(NULL));
	strpadspaces(str, 12);
	debugPrintC(str, LCD_WHITE);
	debugPrint(TXT(" -> "));
}

void debugPrintChar(char c) {
	char s[2];

	if (_u8Telemetry) return;
	s[0] = c;
	s[1] = 0;
#ifndef SEGGER_JLINK_RTT
	if (!_u8Init) return;
	_checkNewMessage();
	strcat(_pszCurrentDebugMessage, s);
#else
	SEGGER_RTT_WriteString(0, s);
#endif
}

void debugOk(void) {

	if (_u8Telemetry) return;
	debugPrintLnC(TXT("Ok!"), LCD_GREEN);
}

void debugFail(void) {

	if (_u8Telemetry) return;
	debugPrintLnC(TXT("Failed!"), LCD_RED);
}

/*void MSG_PrintC(const char *str, uint16_t color)
 {
 DBG_PrintC(str, color);
 if (_silent) return;
 LCD_SetColor(color, LCD_BLACK);
 LCD_Print(str);
 }

 void MSG_Print(const char *str)
 {
 DBG_Print(str);
 if (_silent) return;
 LCD_SetColor(LCD_LIGHTGRAY, LCD_BLACK);
 LCD_Print(str);
 }

 void MSG_PrintLnC(const char *str, uint16_t color)
 {
 if (_silent) return;
 MSG_PrintC(str, color);
 MSG_PrintLn("");
 }

 void MSG_PrintLn(const char *str)
 {
 DBG_PrintLn(str);
 if (_silent) return;
 LCD_SetColor(LCD_LIGHTGRAY, LCD_BLACK);
 LCD_PrintLn(str);
 }

 void MSG_Ok(void)
 {
 #ifdef USARTDEBUG
 _usartSend(TXT("\x1B[32;1mOk!\x1B[0m\r\n"));
 #endif
 if (_silent) return;
 LCD_SetColor(LCD_GREEN, LCD_BLACK);
 LCD_PrintLn(TXT("Ok!"));
 }

 void MSG_Fail(void)
 {
 #ifdef USARTDEBUG
 _usartSend(TXT("\x1B[31;1mFailed!\x1B[0m\r\n"));
 #endif
 if (_silent) return;
 LCD_SetColor(LCD_RED, LCD_BLACK);
 LCD_PrintLn(TXT("Failed!"));
 }*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
ISR(USART_DEBUG_IRQHandler) {
	uint16_t data;

	if (USART_DEBUG->SR & USART_SR_RXNE) {
		data = USART_DEBUG->DR;
	} else {
		data = USART_DEBUG->DR; // Flush data
	}
	__DSB();
}
#pragma GCC diagnostic pop
#endif

