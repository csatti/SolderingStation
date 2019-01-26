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

#include "eeprom.h"
#include "main.h"

#define FLAG_WAIT				pdMS_TO_TICKS(2)
#define FLAG_TRIES				20

#define NOTIFY_SB    			0x00000001UL
#define NOTIFY_ADDR    			0x00000002UL
#define NOTIFY_TX    			0x00000004UL
#define NOTIFY_RX    			0x00000008UL
#define NOTIFY_STOP    			0x00000010UL
#define NOTIFY_DMA_RX    		0x00000020UL
#define NOTIFY_DMA_TX    		0x00000040UL
#define NOTIFY_FAULT    		0x00000080UL
#define NOTIFY_CLEAR_MASK		0x000000FFUL

#ifdef EEPROM_M24C16
	#define I2C_ADDRESS(memaddr)		(EEPROM_I2C_SLAVE_ADDRESS | ((memaddr >> 7) & 0x0E))
	#define MAX_DELAY					pdMS_TO_TICKS(100)
#endif

#define MUTEX_ENTER()	xSemaphoreTake(_shMutexEEPROM, portMAX_DELAY)
#define MUTEX_EXIT()	xSemaphoreGive(_shMutexEEPROM)



static volatile SemaphoreHandle_t _shMutexEEPROM  = NULL;

static volatile TaskHandle_t _thEEPROMTask = NULL;

static volatile uint8_t _u8SlaveAddress = 0;

// TODO: refactor code for separate I2C HAL for multiple slave handling

// I2C event interrupt handler
ISR(EEPROM_I2C_EV_IRQHandler)
{
	BaseType_t higherPriorityTaskWoken;
	uint32_t notifyVal = 0;

	// Start signal sent, send slave address
	if (LL_I2C_IsActiveFlag_SB(EEPROM_I2C)) {
		// No notification value, send slave address, don't wake up the task yet
		LL_I2C_TransmitData8(EEPROM_I2C, _u8SlaveAddress);
		return;
	// Address acknowledged
	} else if (LL_I2C_IsActiveFlag_ADDR(EEPROM_I2C)) {
		notifyVal = NOTIFY_ADDR;
		LL_I2C_ClearFlag_ADDR(EEPROM_I2C);
	// TX shadow register empty
	} else if (LL_I2C_IsActiveFlag_TXE(EEPROM_I2C)) {
		notifyVal = NOTIFY_TX;
	// Stop signal received (slave)
	} else if(LL_I2C_IsActiveFlag_STOP(EEPROM_I2C))
	  {
		notifyVal = NOTIFY_STOP;
	    LL_I2C_ClearFlag_STOP(EEPROM_I2C);
	}
	// Wake up task if necessary
	if ((_thEEPROMTask != NULL) && (notifyVal)) {
		xTaskNotifyFromISR( _thEEPROMTask, notifyVal, eSetBits, &higherPriorityTaskWoken);
		portYIELD_FROM_ISR( higherPriorityTaskWoken );
	}
	// Disable interrupts
	LL_I2C_DisableIT_BUF(EEPROM_I2C);
	LL_I2C_DisableIT_EVT(EEPROM_I2C);
	__DSB();
}

// I2C error interrupt handler
ISR(EEPROM_I2C_ER_IRQHandler)
{
	BaseType_t higherPriorityTaskWoken;

	// Clear acknowledge failure flag
	if (LL_I2C_IsActiveFlag_AF(EEPROM_I2C)) LL_I2C_ClearFlag_AF(EEPROM_I2C);
	// Clear overrun / underrun flag
	if (LL_I2C_IsActiveFlag_OVR(EEPROM_I2C)) LL_I2C_ClearFlag_OVR(EEPROM_I2C);
	// Clear byte error flag
	if (LL_I2C_IsActiveFlag_BERR(EEPROM_I2C)) LL_I2C_ClearFlag_BERR(EEPROM_I2C);

	// Stop the communication
	LL_I2C_GenerateStopCondition(EEPROM_I2C);

	// Send fault notification to task
	if (_thEEPROMTask != NULL) {
		xTaskNotifyFromISR( _thEEPROMTask, NOTIFY_FAULT, eSetBits, &higherPriorityTaskWoken);
		portYIELD_FROM_ISR( higherPriorityTaskWoken );
	}
	__DSB();
}

// DMA receive interrupt handler
ISR(EEPROM_I2C_Rx_DMA_IRQHandler)
{
	BaseType_t higherPriorityTaskWoken;

	// TODO: implement transfer error

	// Clear DMA transfer complete flag
	EEPROM_I2C_Rx_DMA_ClearTCFlag();
	// Disable I2C receive via DMA
	LL_I2C_DisableDMAReq_RX(EEPROM_I2C);
	// Disable DMA stream
	LL_DMA_DisableStream(EEPROM_I2C_DMA, EEPROM_I2C_Rx_DMA_Stream);
	// Disable TC interrupt
	LL_DMA_DisableIT_TC(EEPROM_I2C_DMA, EEPROM_I2C_Rx_DMA_Stream);

	// Send DMA complete notification to task
	if (_thEEPROMTask != NULL) {
		xTaskNotifyFromISR( _thEEPROMTask, NOTIFY_DMA_RX, eSetBits, &higherPriorityTaskWoken);
		portYIELD_FROM_ISR( higherPriorityTaskWoken );
	}
	__DSB();
}

// DMA transmit interrupt handler
ISR(EEPROM_I2C_Tx_DMA_IRQHandler)
{
	BaseType_t higherPriorityTaskWoken;

	// TODO: implement transfer error

	// Clear DMA transfer complete flag
	EEPROM_I2C_Tx_DMA_ClearTCFlag();
	// Disable I2C transmit via DMA
	LL_I2C_DisableDMAReq_TX(EEPROM_I2C);
	// Disable TC interrupt
	LL_DMA_DisableIT_TC(EEPROM_I2C_DMA, EEPROM_I2C_Tx_DMA_Stream);
	// Disable DMA stream
	LL_DMA_DisableStream(EEPROM_I2C_DMA, EEPROM_I2C_Tx_DMA_Stream);

	// Send DMA complete notification to task
	if (_thEEPROMTask != NULL) {
		xTaskNotifyFromISR( _thEEPROMTask, NOTIFY_DMA_TX, eSetBits, &higherPriorityTaskWoken);
		portYIELD_FROM_ISR( higherPriorityTaskWoken );
	}
	__DSB();
}

// I2C release bus
static void _i2cReleaseBus(void)
{
	// If master mode enabled and no stop request is active, generate stop signal
	if ((LL_I2C_IsActiveFlag_MSL(EEPROM_I2C)) && (!(EEPROM_I2C->CR1 & I2C_CR1_STOP))) {
		LL_I2C_GenerateStopCondition(EEPROM_I2C);
	}
}

// Wait for event notification from interrupt handlers
static uint32_t _waitForEvent(uint32_t notify)
{
	BaseType_t result;
	uint32_t notifyVal = 0, prevVal = 0;

	// Wait until notification
	do {
		// Enable I2C event interrupt for address / transmit buffer empty and stop events
		if (notify & (NOTIFY_ADDR | NOTIFY_TX | NOTIFY_STOP))
			LL_I2C_EnableIT_EVT(EEPROM_I2C);
		// Enable I2C data buffer events for transmit buffer empty
		if (notify & NOTIFY_TX)
			LL_I2C_EnableIT_BUF(EEPROM_I2C);

		// Store accumulated events
		prevVal = notifyVal;
		// Wait for notification
		result = xTaskNotifyWait( pdFALSE, notify | NOTIFY_FAULT, &notifyVal, MAX_DELAY );
		// Combine new event with previous events
		notifyVal |= prevVal;
		// If no event in allocated time, generate fault
		if ((result != pdPASS) || (notifyVal & NOTIFY_FAULT)) {
			_i2cReleaseBus();
			return 0;
		}
		// Wait until all required event is collected
	} while ((notifyVal & notify) != notify);

	return notifyVal;
}

// Wait until I2C bus is not blocked
static uint16_t _waitForNotBusy(void)
{
	//int tries = FLAG_TRIES * 50;

	/*if (LL_I2C_IsActiveFlag_BUSY(EEPROM_I2C)) {
		while (tries)
		{
			vTaskDelay(FLAG_WAIT);
			if (!LL_I2C_IsActiveFlag_BUSY(EEPROM_I2C)) break;
			tries--;
		}
		if (!tries) return 0;
	}*/


	// TODO: implement something not blocking here
	while (LL_I2C_IsActiveFlag_BUSY(EEPROM_I2C)) {
		_i2cReleaseBus();
		/*tries--;
		if (!tries) return 1;*/
	}

	return 1;
}

// Initialize I2C bus assigned to EEPROM and local variable(s)
void eepromInit(void)
{
	boardEEPROMConfig();
	// Create mutex for EEPROM
	_shMutexEEPROM = xSemaphoreCreateMutex();
	vQueueAddToRegistry(_shMutexEEPROM, TXT("EEPROMMx"));
}


// Start I2C communication
static uint16_t _i2cStart(void)
{
	int tries = FLAG_TRIES;

	do
	{
		// Generate start signal (interrupt handler sends the slave address)
		LL_I2C_GenerateStartCondition(EEPROM_I2C);

		// Wait for slave acknowledging address
		if (_waitForEvent(NOTIFY_ADDR)) break;
		tries--;
	} while (tries);

	// If run out of tries, release bus and return fault signal
	if (!tries) {
		_i2cReleaseBus();
		return 0;
	}

	return 1;
}

// Read a data block from the I2C EEPROM
static uint16_t _readBlock(uint16_t address, uint8_t* buffer, uint16_t numberOfBytes)
{
	// Wait for ready state
	if (!_waitForNotBusy()) return 0;

	// Calculate slave address for writing mode (certain EEPROMs use part of the target memory address in their own slave address)
	_u8SlaveAddress = I2C_ADDRESS(address);
	// Clear notification status
	xTaskNotifyStateClear(NULL);

	// Start signal
	if (!_i2cStart()) return 0;

	#ifdef EEPROM_M24C16
		// Send lower 8 bit of memory address (top 3 bit was sent as part of the slave address)
		LL_I2C_TransmitData8(EEPROM_I2C, address & 0xff);
		// Send stop signal
		LL_I2C_GenerateStopCondition(EEPROM_I2C);
	#endif

	// Wait for ready state
	if (!_waitForNotBusy()) return 0;

	// Calculate slave address for reading mode
	_u8SlaveAddress = I2C_ADDRESS(address) | 0x01;
	// Clear notification status
	xTaskNotifyStateClear(NULL);

	// Clear DMA transfer complete flag
	EEPROM_I2C_Rx_DMA_ClearTCFlag();
	// Set buffer length as receive length
	LL_DMA_SetDataLength(EEPROM_I2C_DMA, EEPROM_I2C_Rx_DMA_Stream, numberOfBytes);
	// Set buffer as target memory address
	LL_DMA_SetMemoryAddress(EEPROM_I2C_DMA, EEPROM_I2C_Rx_DMA_Stream, (uint32_t)buffer);
	// Enable DMA interrupt
	LL_DMA_EnableIT_TC(EEPROM_I2C_DMA, EEPROM_I2C_Rx_DMA_Stream);
	// Enable DMA stream
	LL_DMA_EnableStream(EEPROM_I2C_DMA, EEPROM_I2C_Rx_DMA_Stream);

	// Send start signal
	if (!_i2cStart()) return 0;

	// Send ACK signal after each new data byte
	LL_I2C_AcknowledgeNextData(EEPROM_I2C, LL_I2C_ACK);
	// Last data byte during DMA transaction must not be acknowledged, NACK signal (slave will stop the transaction if no ACK received)
	LL_I2C_EnableLastDMA(EEPROM_I2C);
	// Enable DMA requests for I2C receive
	LL_I2C_EnableDMAReq_RX(EEPROM_I2C);

	// Wait until DMA is done
	if (!_waitForEvent(NOTIFY_DMA_RX)) return 0;

	// Send stop signal
	LL_I2C_GenerateStopCondition(EEPROM_I2C);
	return numberOfBytes;
}

// Write a data block to the I2C EEPROM
static uint16_t _writeBlock(uint16_t address, uint8_t* buffer, uint16_t numberOfBytes)
{
	// Wait for ready state
	if (!_waitForNotBusy()) return 0;

	// Calculate slave address for writing mode (certain EEPROMs use part of the target memory address in their own slave address)
	_u8SlaveAddress = I2C_ADDRESS(address);
	// Clear notification status
	xTaskNotifyStateClear(NULL);

	// Send start signal
	if (!_i2cStart()) return 0;

	#ifdef EEPROM_M24C16
		// Send lower 8 bit of memory address (top 3 bit was sent as part of the slave address)
		LL_I2C_TransmitData8(EEPROM_I2C, address & 0xff);
	#endif

	// Clear DMA transfer complete flag
	EEPROM_I2C_Tx_DMA_ClearTCFlag();
	// Set buffer length as transmit length
	LL_DMA_SetDataLength(EEPROM_I2C_DMA, EEPROM_I2C_Tx_DMA_Stream, numberOfBytes);
	// Set buffer as source memory address
	LL_DMA_SetMemoryAddress(EEPROM_I2C_DMA, EEPROM_I2C_Tx_DMA_Stream, (uint32_t)buffer);
	// Enable DMA interrupt
	LL_DMA_EnableIT_TC(EEPROM_I2C_DMA, EEPROM_I2C_Tx_DMA_Stream);
	// Enable DMA stream
	LL_DMA_EnableStream(EEPROM_I2C_DMA, EEPROM_I2C_Tx_DMA_Stream);
	// Is it necessary?
	LL_I2C_DisableLastDMA(EEPROM_I2C);
	// Enable DMA requests for I2C transmit
	LL_I2C_EnableDMAReq_TX(EEPROM_I2C);

	// Wait until DMA is done
	if (!_waitForEvent(NOTIFY_DMA_TX)) return 0;

	// Wait for last byte to leave transmit buffer (otherwise last byte is lost as
	// DMA transfer complete interrupt comes after all the bytes in the buffer is sent to
	// the I2C periphery, but before it actually is put on the bus itself).
	if (!_waitForEvent(NOTIFY_TX)) return 0;

	// Send stop signal
	LL_I2C_GenerateStopCondition(EEPROM_I2C);
	// Wait for ready state
	if (!_waitForNotBusy()) return 0;
	return numberOfBytes;
}

// Read data from EEPROM
uint16_t eepromReadBlock(uint16_t address, uint8_t* buffer, uint16_t numberOfBytes)
{

	int tries = FLAG_TRIES;

	// Exit if end of read area is outside of EEPROM memory
	if (address + numberOfBytes > EEPROM_SIZE) return 0;

	// Lock EEPROM (if it is locked, wait)
	MUTEX_ENTER();
	// Store current task for notification
	_thEEPROMTask = xTaskGetCurrentTaskHandle();
	// Read data block
	while ((!_readBlock(address, buffer, numberOfBytes)) && (tries))
	{
		tries--;
	}
	// Clear task
	_thEEPROMTask = NULL;
	// Release EEPROM
	MUTEX_EXIT();
	return tries;
}

// Write data to EEPROM
uint16_t eepromWriteBlock(uint16_t address, uint8_t* buffer, uint16_t numberOfBytes)
{
	static uint8_t olddata[EEPROM_PAGE_SIZE]; // No CCM memory allowed for DMA!
	uint16_t remaining = numberOfBytes;
	uint16_t cnt = 0;
	int tries = FLAG_TRIES;

	// Exit if end of write area is outside of EEPROM memory
	if (address + numberOfBytes > EEPROM_SIZE) return 0;

	// Lock EEPROM (if it is locked, wait)
	MUTEX_ENTER();
	// Switch off write protection
	EEPROM_I2C_WC_OFF();
	// Store current task for notification
	_thEEPROMTask = xTaskGetCurrentTaskHandle();

	while (remaining)
	{
		// EEPROMs can be written up to their page boundary, if address does not start at a page boundary, we have to write less bytes than a full page
		cnt = EEPROM_PAGE_SIZE - (address & (EEPROM_PAGE_SIZE - 1)); // Align by page size
		// If remaining bytes is less than available pages in a page, decrease the write count
		if (cnt > remaining) cnt = remaining;

		// Read old data
		while ((!_readBlock(address, olddata, cnt)) && (tries))
		{
			tries--;
		}

		// Break writing if failed
		if (!tries) {
			numberOfBytes = 0;
			break;
		}

		// Store current buffer pointer
		uint8_t* pos = buffer;
		// Initial value for difference
		uint8_t diff = 0;
		// Compare new data with old
		for (int i = 0; i < cnt; i++) {
			if (*pos++ != olddata[i]) {
				diff = 1;
				break;
			}
		}

		// If new data is different from old, write EEPROM (decrease wear)
		if (diff) {
			while ((!_writeBlock(address, buffer, cnt)) && (tries))
			{
				tries--;
			}
			// Break writing if failed
			if (!tries) {
				numberOfBytes = 0;
				break;
			}
		}

		// Move buffer pointer to next page
		buffer += cnt;
		// Increase target address
		address += cnt;
		// Decrease remaining bytes
		remaining -= cnt;

		// Release CPU until EEPROM is probably ready to be written again
		if ((remaining) && (diff)) {
			vTaskDelay(pdMS_TO_TICKS(EEPROM_WRITE_TIME));
		}
	}
	// Clear task
	_thEEPROMTask = NULL;
	// Switch on write protection
	EEPROM_I2C_WC_ON(); // TODO: test write complete
	// Release EEPROM
	MUTEX_EXIT();
	return numberOfBytes;
}


