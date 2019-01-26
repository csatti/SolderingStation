/* Copyright (C) 2017 - 2019, Attila Kovács
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

#ifndef TM_DEFINES_H
#define TM_DEFINES_H

// Build configurations
//#define THIN_BUILD												// Decreased flash requirements

// Debug
#define USARTDEBUG                     		                	// Enable messages via serial
#define USARTTELEMETRY											// Enable telemetry via USART
#define USART_DEBUG_BAUDRATE          	921600UL				// 921600UL

// ADC limits
#define TIP_VALID_LOW					200						// Low limit for valid ADC tip temperature value
#define TIP_VALID_HIGH					3600					// High limit for valid ADC tip temperature value
#define INPUT_LOW						1400					// Signal considered ON below this value
#define INPUT_HIGH						3000					// Signal considered OFF above this value

// Calibration points
//#define CALIB_T245						650, 800, 1300			// Calibration ADC values (above reference offset) for calibrating C245
//#define CALIB_T210						(180 + OFFSET_T210), (250 + OFFSET_T210), (340 + OFFSET_T210)
//																// Calibration ADC values (above reference offset) for calibrating C210
#define CALIB_T245						1300, 1800, 2500			// Calibration ADC values (above reference offset) for calibrating C245
#define CALIB_T210						(360 + OFFSET_T210), (500 + OFFSET_T210), (680 + OFFSET_T210)
																// Calibration ADC values (above reference offset) for calibrating C210

// Temperature limits
#define TEMP_ZERO						320						// in 0.1 °F   (0 °C)
#define TEMP_COLD						1220					// in 0.1 °F   (50 °C)

// Cold junction
#define USE_STARTING_TEMPERATURE								// Use starting ambient temperature for cold junction compensation (otherwise it uses the ambient sensor continuously)

// Thermal throttling
#define THERMAL_ADD_T245				12						// Add "heat" value to tool control if using C245 cartridge
#define THERMAL_ADD_T210				4						// Add "heat" value to tool control if using C210 cartridge
#define THERMAL_LIMIT					22000					// "Heat" limit (each tool control dissipates one "heat" value when not heating)
#define THERMAL_RETURNTONORMAL			(THERMAL_LIMIT - 1500)

// Protection limits
#define CURRENT_LIMIT					13000					// mA RMS
#define VOLTAGE_LIMIT					29000					// mV RMS
#define TIPTEMP_LIMIT					8960					// in 0.1 °F   (480 °C)
#define AMBIENTTEMP_LIMIT				1220					// in 0.1 °F   (50 °C)

// Pulse control
#define PULSE_MAX						10						// Maximum pulse before temperature measurements
#define PULSE_MAX_DIFF_10K				4						// Maximum pulse before temperature measurements (if temperature difference less than 10 Kelvin)
#define PULSE_MIN						2						// Minimum number of heating pulses
#define PULSE_WAIT						0						// Wait this number of cycles before temperature measurement (let the system settle)

// Power
#define NOMINAL_POWER					160						// Watts

// Power limiting
//#define POWER_LIMIT						(NOMINAL_POWER + 20)	// Watts
#define POWER_OVERLIMIT					10					// Watts

// Input offset compensation
// AD8226 (470 Ohm, gain resistance)
#define OFFSET_T245						31						// Bypassing digital input current offset on temperature measurement
#define OFFSET_T210						172						// Bypassing digital input current offset on temperature measurement

// Timers
#define TIME_FORGOTTEN					(6*60)					// minutes, Warning time system is on (play melody)
#define TIME_FORGOTTEN_REPEAT			(1*60)					// minutes, Repeat time after first warning

// Monitoring - debug
//#define MONITORING

#endif
