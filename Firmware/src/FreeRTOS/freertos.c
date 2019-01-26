#include "main.h"

/* Hook prototypes */
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

__weak void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
}

__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}

__weak  void vApplicationDaemonTaskStartupHook( void )
{

}

volatile unsigned long ulHighFrequencyTimerTicks = 0;

#ifdef TIMER_RTOS_IRQHandler
ISR(TIMER_RTOS_IRQHandler)
{

    NVIC_ClearPendingIRQ(TIMER_RTOS_IRQn);
	LL_TIM_ClearFlag_UPDATE(TIMER_RTOS);

    /* Increment the counter used to mease execution time */
    ulHighFrequencyTimerTicks++;
}
#endif
