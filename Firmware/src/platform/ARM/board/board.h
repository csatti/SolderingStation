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

#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include "arch.h"
#include "defines.h"

// Unique identifier
#define UNIQUE_ID                         0x1FFFF7E8

// DEBUG USART
#define USART_DEBUG                       USART1
#define USART_DEBUG_GPIO                  GPIOA
#define USART_DEBUG_PERIPH                LL_APB2_GRP1_PERIPH_USART1
#define USART_DEBUG_RxPin                 LL_GPIO_PIN_10
#define USART_DEBUG_RxPin_SetAF()         LL_GPIO_SetAFPin_8_15(USART_DEBUG_GPIO, USART_DEBUG_RxPin, LL_GPIO_AF_7)
#define USART_DEBUG_TxPin                 LL_GPIO_PIN_9
#define USART_DEBUG_TxPin_SetAF()         LL_GPIO_SetAFPin_8_15(USART_DEBUG_GPIO, USART_DEBUG_TxPin, LL_GPIO_AF_7)
#define USART_DEBUG_IRQn                  USART1_IRQn
#define USART_DEBUG_IRQHandler            USART1_IRQHandler
#define USART_DEBUG_DMA_PERIPH            LL_AHB1_GRP1_PERIPH_DMA2
#define USART_DEBUG_DMA                   DMA2
#define USART_DEBUG_Tx_DMA_Channel        LL_DMA_CHANNEL_4     // Depends on USART! See DMA request mapping
#define USART_DEBUG_Tx_DMA_Stream         LL_DMA_STREAM_7
#define USART_DEBUG_Tx_DMA_IRQn           DMA2_Stream7_IRQn
#define USART_DEBUG_Tx_DMA_IRQHandler     DMA2_Stream7_IRQHandler
#define USART_DEBUG_Tx_DMA_ClearTCFlag()  LL_DMA_ClearFlag_TC7(USART_DEBUG_DMA)
#define USART_DEBUG_Rx_DMA_Channel        LL_DMA_CHANNEL_4     // Depends on USART! See DMA request mapping
#define USART_DEBUG_Rx_DMA_Stream         LL_DMA_STREAM_5
#define USART_DEBUG_Rx_DMA_IRQn           DMA2_Stream5_IRQn
#define USART_DEBUG_Rx_DMA_IRQHandler     DMA2_Stream5_IRQHandler
#define USART_DEBUG_Rx_DMA_ClearTCFlag()  LL_DMA_ClearFlag_TC5(USART_DEBUG_DMA)

// Monitoring
/*#define MONITORING_GPIO                   GPIOC
#define MONITORING_GPIO_PERIPH            LL_APB2_GRP1_PERIPH_GPIOC
#define MONITORING_1_Pin                  LL_GPIO_PIN_13
#define MONITORING_2_Pin                  LL_GPIO_PIN_14
#if defined(MONITORING)
#define MONITOR
#define MONITOR_1_SET()                   LL_GPIO_SetOutputPin(MONITORING_GPIO, MONITORING_1_Pin)
#define MONITOR_2_SET()                   LL_GPIO_SetOutputPin(MONITORING_GPIO, MONITORING_2_Pin)
#define MONITOR_1_CLR()                   LL_GPIO_ResetOutputPin(MONITORING_GPIO, MONITORING_1_Pin)
#define MONITOR_2_CLR()                   LL_GPIO_ResetOutputPin(MONITORING_GPIO, MONITORING_2_Pin)
#else*/
#define MONITOR_1_SET()                   ((void) 0)
#define MONITOR_2_SET()                   ((void) 0)
#define MONITOR_1_CLR()                   ((void) 0)
#define MONITOR_2_CLR()                   ((void) 0)
/*#endif*/

#define TIMER_RTOS                    	  TIM6
#define TIMER_RTOS_PERIPH                 LL_APB1_GRP1_PERIPH_TIM6
#define TIMER_RTOS_IRQn                   TIM6_DAC_IRQn
#define TIMER_RTOS_IRQHandler             TIM6_DAC_IRQHandler
#define TIMER_RTOS_PERIPH_STOP			  LL_DBGMCU_APB1_GRP1_TIM6_STOP

// Backup
#define BACKUPBYBATTERY					  (RTC_BASE + 0x50)		// RTC->BKP0R

// LCD
// LCD is in board_ILI9341.h

// Buzzer
#define BUZZER_PORT						  GPIOA
#define BUZZER_Pin						  LL_GPIO_PIN_3		// PA3
#define BUZZER_TIMER					  TIM5
#define BUZZER_TIMER_PERIPH  			  LL_APB1_GRP1_PERIPH_TIM5
#define BUZZER_TIMER_CHANNEL			  LL_TIM_CHANNEL_CH4
#define BUZZER_TIMER_MAXPERIOD 	  		  480000
#define BUZZER_DEFAULT_PWM_Frequency 	  1000

// I2C EEPROM
#define EEPROM_I2C                        I2C2
#define EEPROM_I2C_GPIO               	  GPIOB
#define EEPROM_I2C_PERIPH                 LL_APB1_GRP1_PERIPH_I2C2
#define EEPROM_I2C_SCL_Pin                LL_GPIO_PIN_10
#define EEPROM_I2C_SDA_Pin                LL_GPIO_PIN_11
#define EEPROM_I2C_WC_Pin                 LL_GPIO_PIN_12
#define EEPROM_I2C_WC_OFF()               LL_GPIO_ResetOutputPin(EEPROM_I2C_GPIO, EEPROM_I2C_WC_Pin)
#define EEPROM_I2C_WC_ON()                LL_GPIO_SetOutputPin(EEPROM_I2C_GPIO, EEPROM_I2C_WC_Pin)
#define EEPROM_I2C_EV_IRQn            	  I2C2_EV_IRQn
#define EEPROM_I2C_EV_IRQHandler      	  I2C2_EV_IRQHandler
#define EEPROM_I2C_ER_IRQn            	  I2C2_ER_IRQn
#define EEPROM_I2C_ER_IRQHandler      	  I2C2_ER_IRQHandler
#define EEPROM_I2C_DMA                    DMA1
#define EEPROM_I2C_DMA_PERIPH             LL_AHB1_GRP1_PERIPH_DMA1
#define EEPROM_I2C_Tx_DMA_Channel         LL_DMA_CHANNEL_7
#define EEPROM_I2C_Tx_DMA_Stream          LL_DMA_STREAM_7
#define EEPROM_I2C_Tx_DMA_IRQn            DMA1_Stream7_IRQn
#define EEPROM_I2C_Tx_DMA_IRQHandler      DMA1_Stream7_IRQHandler
#define EEPROM_I2C_Tx_DMA_ClearTCFlag()   LL_DMA_ClearFlag_TC7(EEPROM_I2C_DMA)
#define EEPROM_I2C_Rx_DMA_Channel         LL_DMA_CHANNEL_7
#define EEPROM_I2C_Rx_DMA_Stream          LL_DMA_STREAM_2
#define EEPROM_I2C_Rx_DMA_IRQn            DMA1_Stream2_IRQn
#define EEPROM_I2C_Rx_DMA_IRQHandler      DMA1_Stream2_IRQHandler
#define EEPROM_I2C_Rx_DMA_ClearTCFlag()   LL_DMA_ClearFlag_TC2(EEPROM_I2C_DMA)
#define EEPROM_I2C_SPEED                  400000
#define EEPROM_I2C_OWN_ADDRESS            0x90
#define EEPROM_I2C_SLAVE_ADDRESS          0xA0
#define EEPROM_M24C16					  1


/*

// Lights control
#define LIGHTS_GPIO                       GPIOB
#define LIGHTS_GPIO_PERIPH                LL_APB2_GRP1_PERIPH_GPIOB
#define LIGHTS_Ch1Out_Pin                 LL_GPIO_PIN_0
#define LIGHTS_Ch2Out_Pin                 LL_GPIO_PIN_1
#define LIGHTS_Ch1On_Pin                  LL_GPIO_PIN_8                                               // Warning! 5V tolerant mode
#define LIGHTS_Ch2On_Pin                  LL_GPIO_PIN_9                                               // Warning! 5V tolerant mode
#define LIGHTS_CHANNEL1_OFF()             LL_GPIO_SetOutputPin(LIGHTS_GPIO, LIGHTS_Ch1On_Pin)
#define LIGHTS_CHANNEL2_OFF()             LL_GPIO_SetOutputPin(LIGHTS_GPIO, LIGHTS_Ch2On_Pin)
#define LIGHTS_CHANNEL1_ON()              LL_GPIO_ResetOutputPin(LIGHTS_GPIO, LIGHTS_Ch1On_Pin)
#define LIGHTS_CHANNEL2_ON()              LL_GPIO_ResetOutputPin(LIGHTS_GPIO, LIGHTS_Ch2On_Pin)

#define TIMER_LIGHTS                        TIM3
#define TIMER_LIGHTS_PERIPH                 LL_APB1_GRP1_PERIPH_TIM3
#define TIMER_LIGHTS_Channel1               LL_TIM_CHANNEL_CH3
#define TIMER_LIGHTS_Channel2               LL_TIM_CHANNEL_CH4
#define TIMER_LIGHTS_Channel1_CCR           TIMER_LIGHTS->CCR3
#define TIMER_LIGHTS_Channel2_CCR           TIMER_LIGHTS->CCR4
#define TIMER_LIGHTS_Channel1_CCR_Addr      ((uint32_t)&TIMER_LIGHTS_Channel1_CCR)
#define TIMER_LIGHTS_Channel2_CCR_Addr      ((uint32_t)&TIMER_LIGHTS_Channel2_CCR)


#define TIMER_LIGHTS_Channel1_EnableDMA()   LL_TIM_EnableDMAReq_CC3(TIMER_LIGHTS)
#define TIMER_LIGHTS_Channel2_EnableDMA()   LL_TIM_EnableDMAReq_CC4(TIMER_LIGHTS)
#define TIMER_LIGHTS_DMA_PERIPH             LL_AHB1_GRP1_PERIPH_DMA1
#define TIMER_LIGHTS_DMA                    DMA1
#define TIMER_LIGHTS_Ch1_DMA_Channel        LL_DMA_CHANNEL_2     // Depends on TIMER and its channel! See DMA request mapping
#define TIMER_LIGHTS_Ch1_DMA_IRQn           DMA1_Channel2_IRQn
#define TIMER_LIGHTS_Ch1_DMA_IRQHandler     DMA1_Channel2_IRQHandler
#define TIMER_LIGHTS_Ch1_DMA_ClearTCFlag()  LL_DMA_ClearFlag_TC2(TIMER_LIGHTS_DMA)

#define TIMER_LIGHTS_Ch2_DMA_Channel        LL_DMA_CHANNEL_3     // Depends on TIMER and its channel! See DMA request mapping
#define TIMER_LIGHTS_Ch2_DMA_IRQn           DMA1_Channel3_IRQn
#define TIMER_LIGHTS_Ch2_DMA_IRQHandler     DMA1_Channel3_IRQHandler
#define TIMER_LIGHTS_Ch2_DMA_ClearTCFlag()  LL_DMA_ClearFlag_TC3(TIMER_LIGHTS_DMA)

// EXTI
#define KEYS_EXTI1_IRQn                   EXTI15_10_IRQn
#define KEYS_EXTI1_IRQHandler             EXTI15_10_IRQHandler

*/
// Timing

#define TIMER_CTRL                    	  TIM8
#define TIMER_CTRL_PERIPH                 LL_APB2_GRP1_PERIPH_TIM8
#define TIMER_CTRL_IRQn                   TIM8_CC_IRQn
#define TIMER_CTRL_IRQHandler             TIM8_CC_IRQHandler

// Heating
#define HEATING_GPIO                   	  GPIOC
#define HEATING_Enable_Pin                LL_GPIO_PIN_6
#define HEATING_SelTool_Pin               LL_GPIO_PIN_7
#define HEATING_Heating_Pin               LL_GPIO_PIN_8
#define HEATING_Enable_SET()              LL_GPIO_SetOutputPin(HEATING_GPIO, HEATING_Enable_Pin)
#define HEATING_SelTool_SET()             LL_GPIO_SetOutputPin(HEATING_GPIO, HEATING_SelTool_Pin)
#define HEATING_Heating_ON()              LL_GPIO_SetOutputPin(HEATING_GPIO, HEATING_Heating_Pin)
#define HEATING_Enable_CLR()              LL_GPIO_ResetOutputPin(HEATING_GPIO, HEATING_Enable_Pin)
#define HEATING_SelTool_CLR()             LL_GPIO_ResetOutputPin(HEATING_GPIO, HEATING_SelTool_Pin)
#define HEATING_Heating_OFF()             LL_GPIO_ResetOutputPin(HEATING_GPIO, HEATING_Heating_Pin)

// Zero cross and tool detection
#define DETECT_GPIO                   	  GPIOB
#define DETECT_ZeroCross_Pin			  LL_GPIO_PIN_14
#define DETECT_ToolPresence_Pin 		  LL_GPIO_PIN_15
#define DETECT_EXTI_PERIPH				  LL_APB2_GRP1_PERIPH_SYSCFG
#define DETECT_EXTI_SOURCE                LL_SYSCFG_EXTI_PORTB
#define DETECT_ZeroCross_EXTI_LINE		  LL_EXTI_LINE_14
#define DETECT_ZeroCross_SYSCFG_LINE  	  LL_SYSCFG_EXTI_LINE14
#define DETECT_ToolPresence_EXTI_LINE	  LL_EXTI_LINE_15
#define DETECT_ToolPresence_SYSCFG_LINE   LL_SYSCFG_EXTI_LINE15
#define DETECT_EXTI_IRQn				  EXTI15_10_IRQn
#define DETECT_EXTI_IRQHandler			  EXTI15_10_IRQHandler

// Multiplexer
#define MUX_GPIO                   	  	  GPIOC
#define MUX_1_Pin                		  LL_GPIO_PIN_13
#define MUX_2_Pin     			          LL_GPIO_PIN_14
#define MUX_3_Pin    			          LL_GPIO_PIN_15
#define MUX_1_SET()              		  LL_GPIO_SetOutputPin(MUX_GPIO, MUX_1_Pin)
// TODO: restore code after MCU is replaced
#define MUX_2_SET()             		  do { LL_GPIO_SetOutputPin(MUX_GPIO, MUX_2_Pin); LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_12); } while (0)
#define MUX_3_SET()              		  do { LL_GPIO_SetOutputPin(MUX_GPIO, MUX_3_Pin); LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4); } while (0)
#define MUX_1_CLR()              		  LL_GPIO_ResetOutputPin(MUX_GPIO, MUX_1_Pin)
#define MUX_2_CLR()             		  do { LL_GPIO_ResetOutputPin(MUX_GPIO, MUX_2_Pin); LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_12); } while (0)
#define MUX_3_CLR()             		  do { LL_GPIO_ResetOutputPin(MUX_GPIO, MUX_3_Pin); LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4); } while (0)

// Analog
#define AN_Channel_GPIO1                  GPIOA
#define AN_ToolVoltage_Pin                LL_GPIO_PIN_1  // PA1, ADC1
#define AN_ToolCurrent_Pin                LL_GPIO_PIN_2  // PA2, ADC2

#define AN_Channel_GPIO2                  GPIOC
#define AN_AmbientTemp_Pin                LL_GPIO_PIN_0  // PC0, ADC10
#define AN_MuxDetect_Pin                  LL_GPIO_PIN_1  // PC1, ADC11
#define AN_IronTemp_Pin                   LL_GPIO_PIN_2  // PC2, ADC12
#define AN_RefVoltage_Pin                 LL_GPIO_PIN_3  // PC3, ADC13

#define ADC_CALIBRATION_TIMEOUT_MS       ((uint32_t)   1)
#define ADC_ENABLE_TIMEOUT_MS            ((uint32_t)   1)
#define ADC_DISABLE_TIMEOUT_MS           ((uint32_t)   1)
#define ADC_STOP_CONVERSION_TIMEOUT_MS   ((uint32_t)   1)
#define ADC_CONVERSION_TIMEOUT_MS        ((uint32_t)   2)

#define AN_ADC1                           ADC1
#define AN_ADC1_PERIPH                    LL_APB2_GRP1_PERIPH_ADC1
#define AN_ADC2                           ADC2
#define AN_ADC2_PERIPH                    LL_APB2_GRP1_PERIPH_ADC2
#define AN_ADC3                           ADC3
#define AN_ADC3_PERIPH                    LL_APB2_GRP1_PERIPH_ADC3

#define AN_ADC_IRQn                  	  ADC_IRQn
#define AN_ADC_IRQHandler				  ADC_IRQHandler

#define AN_ADC_DMA                        DMA2
#define AN_ADC_DMA_PERIPH                 LL_AHB1_GRP1_PERIPH_DMA2
#define AN_ADC1_DMA_Channel               LL_DMA_CHANNEL_0
#define AN_ADC1_DMA_Stream                LL_DMA_STREAM_4
#define AN_ADC1_DMA_IRQn                  DMA2_Stream4_IRQn
#define AN_ADC1_DMA_IRQHandler            DMA2_Stream4_IRQHandler
#define AN_ADC1_DMA_ClearTCFlag()         LL_DMA_ClearFlag_TC4(AN_ADC_DMA)
#define AN_ADC2_DMA_Channel               LL_DMA_CHANNEL_1
#define AN_ADC2_DMA_Stream                LL_DMA_STREAM_2
#define AN_ADC2_DMA_IRQn                  DMA2_Stream2_IRQn
#define AN_ADC2_DMA_IRQHandler            DMA2_Stream2_IRQHandler
#define AN_ADC2_DMA_ClearTCFlag()         LL_DMA_ClearFlag_TC2(AN_ADC_DMA)
#define AN_ADC2_DMA_IsTCActive()          LL_DMA_IsActiveFlag_TC2(AN_ADC_DMA)
#define AN_ADC2_DMA_ClearHTFlag()         LL_DMA_ClearFlag_HT2(AN_ADC_DMA)
#define AN_ADC2_DMA_IsHTActive()          LL_DMA_IsActiveFlag_HT2(AN_ADC_DMA)
#define AN_ADC3_DMA_Channel               LL_DMA_CHANNEL_2
#define AN_ADC3_DMA_Stream                LL_DMA_STREAM_1
#define AN_ADC3_DMA_IRQn                  DMA2_Stream1_IRQn
#define AN_ADC3_DMA_IRQHandler            DMA2_Stream1_IRQHandler
#define AN_ADC3_DMA_ClearTCFlag()         LL_DMA_ClearFlag_TC1(AN_ADC_DMA)
#define AN_ADC3_DMA_IsTCActive()          LL_DMA_IsActiveFlag_TC1(AN_ADC_DMA)
#define AN_ADC3_DMA_ClearHTFlag()         LL_DMA_ClearFlag_HT1(AN_ADC_DMA)
#define AN_ADC3_DMA_IsHTActive()          LL_DMA_IsActiveFlag_HT1(AN_ADC_DMA)

// Table 73. Internal reference voltage calibration values in STM32F405/407 datasheet
#define ADC_VREFINTCAL					  ((uint16_t*)0x1FFF7A2A)
// Table 70. Temperature sensor calibration values in STM32F405/407 datasheet
#define ADC_TEMPINTCAL30				  ((uint16_t*)0x1FFF7A2C)
#define ADC_TEMPINTCAL110				  ((uint16_t*)0x1FFF7A2E)


/* ADC sampling calculation:
   ADC cycle for conversion:        Tconv = 12 + SampleTime  (12 bit sampling)
   Samplerate:                      SR = ADCCLK / Tconv

   @ADC clock:                      ADCCLK = 42 MHz (APB2 / 2)

	Max theoretical speed (max 36MHz ADC clock is allowed, numbers are at 42MHz)
    LL_ADC_SAMPLINGTIME_3CYCLES		2.8 MHz
	LL_ADC_SAMPLINGTIME_15CYCLES	1.4 Mhz
	LL_ADC_SAMPLINGTIME_28CYCLES	1.05 MHz
	LL_ADC_SAMPLINGTIME_56CYCLES	617647 Hz
 	LL_ADC_SAMPLINGTIME_84CYCLES	437500 Hz
 	LL_ADC_SAMPLINGTIME_112CYCLES	338710 Hz
	LL_ADC_SAMPLINGTIME_144CYCLES	269231 Hz
 	LL_ADC_SAMPLINGTIME_480CYCLES	85366 Hz

	For this project a divisor of 4 is used, these frequencies are halved.
*/

#define AN_ADC1_BUFFSIZE				  8
#define AN_ADC2_3_BUFFSIZE				  5500	// (((269231 / 2) / 50) * 2)  for 50Hz AC
#define AN_ADC2_3_SAMPLERATE			  (269230UL / 2UL)

#define AN_ADC_ToolVoltage_Channel        LL_ADC_CHANNEL_1
#define AN_ADC_ToolVoltage_SampleTime     LL_ADC_SAMPLINGTIME_144CYCLES
#define AN_ADC_ToolCurrent_Channel        LL_ADC_CHANNEL_2
#define AN_ADC_ToolCurrent_SampleTime     LL_ADC_SAMPLINGTIME_144CYCLES
#define AN_ADC_AmbientTemp_Channel        LL_ADC_CHANNEL_10
#define AN_ADC_AmbientTemp_SampleTime     LL_ADC_SAMPLINGTIME_144CYCLES
#define AN_ADC_MuxDetect_Channel          LL_ADC_CHANNEL_11
#define AN_ADC_MuxDetect_SampleTime       LL_ADC_SAMPLINGTIME_144CYCLES
#define AN_ADC_IronTemp_Channel        	  LL_ADC_CHANNEL_12
#define AN_ADC_IronTemp_SampleTime        LL_ADC_SAMPLINGTIME_144CYCLES
#define AN_ADC_RefVoltage_Channel         LL_ADC_CHANNEL_13
#define AN_ADC_RefVoltage_SampleTime      LL_ADC_SAMPLINGTIME_144CYCLES
#define AN_ADC_VRefInt_Channel         	  LL_ADC_CHANNEL_VREFINT
#define AN_ADC_VRefInt_SampleTime         LL_ADC_SAMPLINGTIME_144CYCLES
#define AN_ADC_IntTemp_Channel         	  LL_ADC_CHANNEL_TEMPSENSOR
#define AN_ADC_VRefInt_SampleTime         LL_ADC_SAMPLINGTIME_144CYCLES

typedef enum {
	AMBIENT_TEMP 	= AN_ADC_AmbientTemp_Channel,
	MULTIPLEXER 	= AN_ADC_MuxDetect_Channel,
	IRON_TEMP 		= AN_ADC_IronTemp_Channel,
	REF_VOLTAGE 	= AN_ADC_RefVoltage_Channel,
	INTERNAL_VREF 	= AN_ADC_VRefInt_Channel,
	INTERNAL_TEMP 	= AN_ADC_IntTemp_Channel,
} AnalogChannels_t;

typedef enum {
	MUX_IRON1_IN_HOLDER = 0,
	MUX_IRON2_IN_HOLDER = 1,
	MUX_TOOL_CHANGE		= 2,
	MUX_IRON1_TOOLID	= 3,
	MUX_IRON2_TOOLID	= 4,
	MUX_SPARE			= 5,
	MUX_NOTUSED1		= 6,
	MUX_NOTUSED2		= 7,
} MuxChannels_t;


// IRQ
void boardDisableIRQ(void);
void boardEnableIRQ(void);

// SW-DP, JTAG config
void boardDPIntfConfig(void);

// System clock initialization
uint32_t boardRCCClockInit(void);
uint32_t boardRTCClockInit(void);

// DEBUG - USART
void boardUSARTDebugConfig(uint32_t bufferAddr);

// Monitoring
void boardMonitoringConfig(void);

// LCD
void boardLCDConfig(void);
void boardLCDDisable(void);

// Rotary encoder + switch
void boardRotaryConfig(void);

// IR Remote
void boardIRRemoteConfig(void);

// Keys (switches)
void boardKeysConfig(void);

// PWM
void boardPWMConfig(void);

// SPI Flash
void boardFLASHConfig(void);

// Backup
void boardBackupInit(void);
void boardBackupDeInit(void);

// Heating
void boardHeatingConfig(void);

// Detection
void boardDetectConfig(void);

// Control timer
void boardCTRLTimerConfig(void);

// Multiplexer
void boardMUXConfig(void);

// ADC
void boardADCConfig(void);

// Buzzer
void boardBuzzerChangeFreq(uint16_t freq);
void boardBuzzerConfig(void);

// EEPROM
void boardEEPROMConfig(void);

void boardRTOSTimerConfig(void);
void boardGPIOConfig(void);

void boardSysTickInit(uint32_t tickRate);


// Load bootlader
void boardLoadBootloader(void);

// System reset
void boardReset(void);

void _Error_Handler(char * file, int line);

#endif /* BOARD_H_INCLUDED */
