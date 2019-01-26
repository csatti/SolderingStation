#include "../../../platform/ARM/board/nvic.h"

#include "FreeRTOS.h"

// RTC interrupt

void nvicRTCConfiguration(void)
{
  // Set priority and enable the RTC Interrupts
  //NVIC_SetPriority(RTC_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, 1, 0));
  //NVIC_EnableIRQ(RTC_IRQn);
}

// USART - DEBUG DMA interrupt

void nvicUSARTDebugDMAConfiguration(void)
{
  // Set priority and enable the USART - DEBUG DMA Interrupts
  NVIC_SetPriority(USART_DEBUG_Tx_DMA_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2, 0));
  NVIC_EnableIRQ(USART_DEBUG_Tx_DMA_IRQn);
  NVIC_SetPriority(USART_DEBUG_Rx_DMA_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2, 0));
  NVIC_EnableIRQ(USART_DEBUG_Rx_DMA_IRQn);
}

// USART - DEBUG interrupt

void nvicUSARTDebugConfiguration(void)
{
  // Set priority and enable the USART - DEBUG Interrupt
  NVIC_SetPriority(USART_DEBUG_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2, 0));
  NVIC_EnableIRQ(USART_DEBUG_IRQn);
}

// IR Remote

void nvicIRRemoteConfiguration(void)
{
  // Set priority and enable the IR Remote Interrupt
  //NVIC_SetPriority(TIMER_IRREMOTE_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, 1, 0));
  //NVIC_EnableIRQ(TIMER_IRREMOTE_IRQn);
}

// Keys EXTI

void nvicKeysEXTIConfiguration(void)
{
  // Set priority and enable the Keys EXTI Interrupts
  //NVIC_SetPriority(KEYS_EXTI1_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, 2, 0));
  //NVIC_EnableIRQ(KEYS_EXTI1_IRQn);
}

// SPI FLASH DMA interrupt

void nvicFlashDMAConfiguration(void)
{
  // Set priority and enable the FLASH SPI DMA Interrupts
  /*NVIC_SetPriority(FLASH_SPI_Tx_DMA_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, 4, 0));
  NVIC_EnableIRQ(FLASH_SPI_Tx_DMA_IRQn);
  NVIC_SetPriority(FLASH_SPI_Rx_DMA_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, 4, 0));
  NVIC_EnableIRQ(FLASH_SPI_Rx_DMA_IRQn);*/
}

// Analog interrupt
void nvicAnalogConfiguration(void)
{
  // Set priority and enable the analog Interrupt
  NVIC_SetPriority(AN_ADC_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1, 0));
  NVIC_EnableIRQ(AN_ADC_IRQn);
}


// Analog conversion DMA interrupt

void nvicAnalogDMAConfiguration(void)
{
  // Set priority and enable the analog conversion DMA Interrupts
  NVIC_SetPriority(AN_ADC1_DMA_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1, 0));
  NVIC_EnableIRQ(AN_ADC1_DMA_IRQn);
  NVIC_SetPriority(AN_ADC2_DMA_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1, 0));
  NVIC_EnableIRQ(AN_ADC2_DMA_IRQn);
  NVIC_SetPriority(AN_ADC3_DMA_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1, 0));
  NVIC_EnableIRQ(AN_ADC3_DMA_IRQn);
}

// Detection EXTI interrupt

void nvicDetectConfiguration(void)
{
  // Set priority and enable the EXTI Interrupt
  NVIC_SetPriority(DETECT_EXTI_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1, 0));
  NVIC_EnableIRQ(DETECT_EXTI_IRQn);
}


// Timer for control position interrupt

void nvicCTRLTimerConfiguration(void)
{
  // Set priority and enable the timer Interrupt
  NVIC_SetPriority(TIMER_CTRL_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1, 0));
  NVIC_EnableIRQ(TIMER_CTRL_IRQn);
}

// Timer for runtime stats interrupt

void nvicRTOSTimerConfiguration(void)
{
  // Set priority and enable the timer Interrupt
  NVIC_SetPriority(TIMER_RTOS_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, 2, 0));
  NVIC_EnableIRQ(TIMER_RTOS_IRQn);
}

// EEPROM I2C interrupts

void nvicEEPROMi2cConfiguration(void)
{
  // Set priority and enable the EEPROM I2C Interrupts
  NVIC_SetPriority(EEPROM_I2C_EV_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2, 0));
  NVIC_EnableIRQ(EEPROM_I2C_EV_IRQn);
  NVIC_SetPriority(EEPROM_I2C_ER_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2, 0));
  NVIC_EnableIRQ(EEPROM_I2C_ER_IRQn);
}

// EEPROM I2C DMA interrupts

void nvicEEPROMi2cDMAConfiguration(void)
{
  // Set priority and enable the EEPROM I2C DMA Interrupts
  NVIC_SetPriority(EEPROM_I2C_Tx_DMA_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2, 0));
  NVIC_EnableIRQ(EEPROM_I2C_Tx_DMA_IRQn);
  NVIC_SetPriority(EEPROM_I2C_Rx_DMA_IRQn, NVIC_EncodePriority(NVIC_PRIORITYGROUPING, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2, 0));
  NVIC_EnableIRQ(EEPROM_I2C_Rx_DMA_IRQn);
}

