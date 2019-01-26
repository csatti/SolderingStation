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

/* The rate at which data is sent to the queue, specified in milliseconds, and
 converted to ticks using the pdMS_TO_TICKS() macro. */
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

/*-----------------------------------------------------------*/

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

	/* If all is well, the scheduler will now be running, and the following line
	 will never be reached.  If the following line does execute, then there was
	 insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	 to be created.  See the memory management section on the FreeRTOS web site
	 for more details.  */
	for (;;)
		;
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
	/* The malloc failed hook is enabled by setting
	 configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	 Called if a call to pvPortMalloc() fails because there is insufficient
	 free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	 internally by FreeRTOS API functions that create tasks, queues, software
	 timers, and semaphores.  The size of the FreeRTOS heap is set by the
	 configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
	(void) pcTaskName;
	(void) xTask;

	/* Run time stack overflow checking is performed if
	 configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	 function is called if a stack overflow is detected.  pxCurrentTCB can be
	 inspected in the debugger if the task name passed into this function is
	 corrupt. */
	for (;;)
		;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void) {
	volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	 FreeRTOSConfig.h.

	 This function is called on each cycle of the idle task.  In this case it
	 does nothing useful, other than report the amount of FreeRTOS heap that
	 remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if (xFreeStackSpace > 100) {
		/* By now, the kernel has allocated everything it is going to, so
		 if there is a lot of heap remaining unallocated then
		 the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		 reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
	/* This function will be called once only, when the daemon task starts to
	execute	(sometimes called the timer task).  This is useful if the
	application includes initialisation code that would benefit from executing
	after the scheduler has been started. */
}


static void prvSetupHardware(void) {
	/* Ensure all priority bits are assigned as preemption priority bits
	 if using a ARM Cortex-M microcontroller. */
	NVIC_SetPriorityGrouping(3);

	boardGPIOConfig();
	boardRCCClockInit();
	boardSysTickInit(configTICK_RATE_HZ);
	messageInit();
	configInit();
	analogInit();
	timingInit();
	soundInit();
	notifyInit();
}


