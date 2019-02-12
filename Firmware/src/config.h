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

#ifndef CONFIG_H_
#define CONFIG_H_

#include "ILI9341.h"
#include "board.h"

// I really don't like to calculate in Fahrenheit, but it is easier to convert from °F to °C without going to
// fraction hell

// Data is stored in 32bit wide registers (bitfield and volatile), max 80 bytes
typedef struct  {
	volatile uint32_t presetTemp1  		: 16;	// in 0.1 °F
	volatile uint32_t presetTemp2  		: 16;	// in 0.1 °F
	volatile uint32_t presetTemp3  		: 16;	// in 0.1 °F
	volatile uint32_t setpointTemp 		: 16;	// in 0.1 °F
	volatile uint32_t language	   		: 16;
	volatile uint32_t sound				: 16;
	volatile uint32_t tempUnitC			: 16;
	volatile uint32_t telemetry			: 16;
	volatile uint32_t diagnostics		: 16;
	volatile uint32_t loadBootloader;
	volatile uint32_t canary;
} ConfigOnline_t;

typedef struct {
	uint16_t offsetMosfetSwitchStart;
	uint16_t offsetLowVoltageSwitchOff;
	uint16_t offsetZeroCross;
} TimingOffsets_t;

typedef struct {
	uint8_t		calibrated;
	uint16_t 	calibrationPointTemp[3];	// in 0.1 °F	(without cold junction temperature)
} ADCtoTemp_t;

// Offline data is stored in 2kB EEPROM (read at start up, written by request)
typedef struct  {
	ILI9341_GammaCurve_t gammaCurve;
	TimingOffsets_t timings;
	ADCtoTemp_t toolT210;
	ADCtoTemp_t toolT245;
	uint32_t canary;
} CalibOffline_t;

static const uint16_t CalibPointsT245[] = { CALIB_T245 };
static const uint16_t CalibPointsT210[] = { CALIB_T210 };

static const uint16_t SleepTempTableSize = 5;
static const uint16_t SleepTempTable[] = { 1760, 2120, 2480, 3020, 3380};  // in 0.1 °F
//										   80,   100,  120,  150,  170     °C

static const uint16_t DelayOffTableSize = 7;
static const uint16_t DelayOffTable[] = { 1, 2, 5, 10, 15, 30, 60}; // in minutes

static const uint16_t PowerLimitTableSize = 8;
static const uint16_t PowerLimitTable[] = { 25, 50, 75, 100, 125, 150, 160, 180}; // in W

static const uint16_t ScreenOffTableSize = 5;
static const uint16_t ScreenOffTable[] = { 5, 15, 30, 60, 120}; // in minutes

static const uint16_t TempStepTableSize = 5;
static const uint16_t TempStepTable[] = { 1, 2, 5, 10, 20};  // step unit is same as selected temperature unit

static const uint16_t MaxTempTableSize = 4;
static const uint16_t MaxTempTable[] = { 5720, 6620, 7520, 8420};   // in 0.1 °F
//										 300,  350,  400,  450		°C

typedef enum {
	TEMP_UNIT,
	SLEEP_TEMPERATURE,
	DELAY_SWITCHOFF,
	TEMPERATURE_STEP,
	MAX_TEMP,
	POWER_LIMIT,
	DELAY_SCREENOFF,
	TELEMETRY,
	DIAGNOSTICS,
	// Constant for number of configuration settings
	LAST_CONFIG
} Configs_t;

typedef struct  {
	uint16_t sleepTemp;
	uint16_t delayOff;
	uint16_t tempStep;
	uint16_t maxTemp;
	uint16_t screenOff;
	uint16_t powerLimit;
	uint32_t canary;
} ConfigOffline_t;

void configInit(void);
void configLoad(void);
void configReset(void);
uint8_t configIsLoaded(void);

// Access to online configuration (battery backed up)
ConfigOnline_t* configGetConfigOnlineMaxWait(uint32_t maxwait);
ConfigOnline_t* configGetConfigOnlineNoWrite(void);
#define configGetConfigOnlineNow() 		configGetConfigOnlineMaxWait(0)
#define configGetConfigOnline() 		configGetConfigOnlineMaxWait(portMAX_DELAY)
void configReturnConfigOnline(void);

// Access to offline configuration (stored in external EEPROM)
ConfigOffline_t* configGetConfigOfflineMaxWait(uint32_t maxwait);
#define configGetConfigOfflineNow() 	configGetConfigOfflineMaxWait(0)
#define configGetConfigOffline() 		configGetConfigOfflineMaxWait(portMAX_DELAY)
void configReturnConfigOffline(void);
void configStoreConfigOffline(void);


// Access to offline calibration (stored in external EEPROM)
CalibOffline_t* configGetCalibOfflineMaxWait(uint32_t maxwait);
#define configGetCalibOfflineNow() 		configGetCalibOfflineMaxWait(0)
#define configGetCalibOffline() 		configGetCalibOfflineMaxWait(portMAX_DELAY)
void configReturnCalibOffline(void);
void configStoreCalibOffline(void);

void configOutputToDebugConsole(void);

#endif /* CONFIG_H_ */
