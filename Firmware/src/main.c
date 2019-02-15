/* Copyright (C) 2018 - 2019, Attila Kovács
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

#include "main.h"
#include "gui.h"
#include "sound.h"
#include <stdlib.h>
#include "analog.h"
#include "message.h"
#include "control.h"
#include "notify.h"

#define mainQUEUE_MONITORINGREPORT_TASK_PRIORITY        ( tskIDLE_PRIORITY + 1 )
#define mainQUEUE_MONITORINGREPORT_PERIOD_MS            pdMS_TO_TICKS( 2000 )

static void prvSetupHardware(void);


static void monitoringTask(void *pvParameters) ;


// Check bootloader start
void static _checkFirmwareUpdateRequest(void)
{
	volatile ConfigOnline_t* configOnline = configGetConfigOnlineNoWrite();

	boardBackupInit();

	if (configOnline->loadBootloader) {
		configOnline->loadBootloader = 0;
		boardBackupDeInit();
		boardLoadBootloader();
	}

	boardBackupDeInit();

}


/* Application start point
   Checks if firmware update was requested
   Runs initialization routines
   Starts RTOS scheduler */
int main(void) {

	_checkFirmwareUpdateRequest();

	prvSetupHardware();

#if (DEBUG != 0)
	// TODO: Restore this
	//notifySend(WARNING_DEBUGMODE);
#endif

	xTaskCreate(monitoringTask, "Monitor", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_MONITORINGREPORT_TASK_PRIORITY, NULL);

	controlInit();
	guiInit();

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	while (1) { __NOP(); }
}

/*-----------------------------------------------------------*/

static void monitoringTask(void *pvParameters) {
	char str[20];
	TickType_t xNextWakeTime;


	while (1)
	{
		vTaskDelayUntil(&xNextWakeTime, mainQUEUE_MONITORINGREPORT_PERIOD_MS);
		ltoa(xPortGetFreeHeapSize(), str, 10);
		debugTimeRef();
		debugPrint(" CCM available: ");
		debugPrintC(str, LCD_YELLOW);
		ltoa(xPortGetMinimumEverFreeHeapSize(), str, 10);
		debugPrint(" CCM min. available: ");
		debugPrintLnC(str, LCD_YELLOW);
		ltoa(xPortGetFreeHeapSizeDMA(), str, 10);
		debugTimeRef();
		debugPrint(" Std. available: ");
		debugPrintC(str, LCD_YELLOW);
		ltoa(xPortGetMinimumEverFreeHeapSizeDMA(), str, 10);
		debugPrint(" Std. min. available: ");
		debugPrintLnC(str, LCD_YELLOW);
	}
}



/*-----------------------------------------------------------*/

void vApplicationTickHook(void) {

}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void) {

	while (1) { __NOP(); }
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
	(void) pcTaskName;
	(void) xTask;

	while (1) { __NOP(); }
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void) {

	__NOP();
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
	// Template
}


static void prvSetupHardware(void) {
	/* Ensure all priority bits are assigned as preemption priority bits
	 if using a ARM Cortex-M microcontroller. */
	NVIC_SetPriorityGrouping(3);

	// Enable clocks for all GPIO
	boardGPIOConfig();
	// Configure clock source(s) and start up PLL, configure flash access mode
	boardRCCClockInit();
	// Setup systick (for RTOS)(
	boardSysTickInit(configTICK_RATE_HZ);
	// USART for debug / telemetry configuration
	messageInit();
	// EEPROM and battery powered backup memory initialization
	configInit();
	// Initialize analog sources
	analogInit();
	// Initialize cyclic time based functions
	timingInit();
	// Initialize piezo buzzer
	soundInit();
	// Initialize user notification queuing
	notifyInit();
}


