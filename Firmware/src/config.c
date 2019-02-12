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

#include "config.h"
#include "main.h"
#include <string.h>
#include "eeprom.h"
#include "notify.h"
#include "message.h"

#define CONFIGONLINE_CANARY		0xDEADBEEF
#define CALIBOFFLINE_CANARY		0xBEE00D1F
#define CONFIGOFFLINE_CANARY	0x124578AC

#define CALIBOFFSET				0x0400
#define CONFIGOFFSET			0x0000

#define WAIT_TIME			pdMS_TO_TICKS(15)

#ifndef BACKUPBYBATTERY
	static ConfigOnline_t _configOnline;
	ConfigOnline_t* configOnline = &_configOnline;
#else
	#define configOnline ((ConfigOnline_t*) BACKUPBYBATTERY )
#endif

static ConfigOffline_t configOffline;
static CalibOffline_t calibOffline;

#define STRCONC(str1,str2) str1 ## str2
#define TOKENPASTE(x, y) STRCONC(x, y)
#define MUTEX_ENTER(mutex)	xSemaphoreTake(mutex, portMAX_DELAY)
#define MUTEX_EXIT(mutex)	xSemaphoreGive(mutex)
#define OUTPUT_PAR(x)		debugTimeRef(); \
							debugPrint(TXT(" " #x " = ")); \
							ltoa(x , str, 10); \
							debugPrintLnC(str, LCD_GREEN)

static uint8_t configLoaded = 0;

static SemaphoreHandle_t _shMutexConfigOnline, _shMutexConfigOffline, _shMutexCalibOffline;

void configOutputToDebugConsole(void)
{
	char str[20];
	ILI9341_GammaCurve_t* gammaCurve = &calibOffline.gammaCurve;
	TimingOffsets_t* timings = &calibOffline.timings;
	ADCtoTemp_t* toolT210 = &calibOffline.toolT210;
	ADCtoTemp_t* toolT245 = &calibOffline.toolT245;


	vTaskDelay(WAIT_TIME);
	debugTimeRef();
	debugPrintLn(TXT("EEPROM calibration data: "));
	OUTPUT_PAR(gammaCurve->VP63);
	OUTPUT_PAR(gammaCurve->VP62);
	OUTPUT_PAR(gammaCurve->VP61);
	vTaskDelay(WAIT_TIME);
	OUTPUT_PAR(gammaCurve->VP59);
	OUTPUT_PAR(gammaCurve->VP57);
	OUTPUT_PAR(gammaCurve->VP50);
	OUTPUT_PAR(gammaCurve->VP43);
	OUTPUT_PAR(gammaCurve->VP36);
	OUTPUT_PAR(gammaCurve->VP27);
	vTaskDelay(WAIT_TIME);
	OUTPUT_PAR(gammaCurve->VP20);
	OUTPUT_PAR(gammaCurve->VP13);
	OUTPUT_PAR(gammaCurve->VP6);
	OUTPUT_PAR(gammaCurve->VP4);
	OUTPUT_PAR(gammaCurve->VP2);
	OUTPUT_PAR(gammaCurve->VP1);
	vTaskDelay(WAIT_TIME);
	OUTPUT_PAR(gammaCurve->VP0);
	OUTPUT_PAR(gammaCurve->VN63);
	OUTPUT_PAR(gammaCurve->VN62);
	OUTPUT_PAR(gammaCurve->VN61);
	OUTPUT_PAR(gammaCurve->VN59);
	OUTPUT_PAR(gammaCurve->VN57);
	OUTPUT_PAR(gammaCurve->VN50);
	vTaskDelay(WAIT_TIME);
	OUTPUT_PAR(gammaCurve->VN43);
	OUTPUT_PAR(gammaCurve->VN36);
	OUTPUT_PAR(gammaCurve->VN27);
	OUTPUT_PAR(gammaCurve->VN20);
	OUTPUT_PAR(gammaCurve->VN13);
	OUTPUT_PAR(gammaCurve->VN6);
	OUTPUT_PAR(gammaCurve->VN4);
	vTaskDelay(WAIT_TIME);
	OUTPUT_PAR(gammaCurve->VN2);
	OUTPUT_PAR(gammaCurve->VN1);
	OUTPUT_PAR(gammaCurve->VN0);
	OUTPUT_PAR(timings->offsetLowVoltageSwitchOff);
	OUTPUT_PAR(timings->offsetMosfetSwitchStart);
	OUTPUT_PAR(timings->offsetZeroCross);
	OUTPUT_PAR(toolT210->calibrated);
	vTaskDelay(WAIT_TIME);
	OUTPUT_PAR(toolT210->calibrationPointTemp[0]);
	OUTPUT_PAR(toolT210->calibrationPointTemp[1]);
	OUTPUT_PAR(toolT210->calibrationPointTemp[2]);
	OUTPUT_PAR(toolT245->calibrated);
	OUTPUT_PAR(toolT245->calibrationPointTemp[0]);
	OUTPUT_PAR(toolT245->calibrationPointTemp[1]);
	OUTPUT_PAR(toolT245->calibrationPointTemp[2]);
}

void configInit(void)
{
	boardBackupInit();
	eepromInit();

	// Initialize mutexes
	_shMutexConfigOnline = xSemaphoreCreateMutex();
	_shMutexConfigOffline = xSemaphoreCreateMutex();
	_shMutexCalibOffline = xSemaphoreCreateMutex();

	vQueueAddToRegistry(_shMutexConfigOnline, TXT("ConfOnMx"));
	vQueueAddToRegistry(_shMutexConfigOffline, TXT("ConfOffMx"));
	vQueueAddToRegistry(_shMutexCalibOffline, TXT("CalOffMx"));
}

static void _resetConfigOnline(void)
{
	configOnline->presetTemp1 = 4640;	// 240 °C, 464 °F
	configOnline->presetTemp2 = 5720;	// 300 °C, 572 °F
	configOnline->presetTemp3 = 6620;	// 350 °C, 662 °F
	configOnline->setpointTemp = 4640;	// 240 °C, 464 °F
	configOnline->language = 0;
	configOnline->sound = 1;
	configOnline->tempUnitC = 1;
	configOnline->telemetry = 0;
	configOnline->diagnostics = 0;
	configOnline->loadBootloader = 0;
	configOnline->canary = CONFIGONLINE_CANARY;
}

static void _resetConfigOffline(void)
{
	configOffline.sleepTemp = SleepTempTable[2];
	configOffline.delayOff = DelayOffTable[4];
	configOffline.tempStep = TempStepTable[3];
	configOffline.maxTemp = MaxTempTable[1];
	configOffline.screenOff = ScreenOffTable[3];
	configOffline.powerLimit = PowerLimitTable[6];
	configOffline.canary = CONFIGOFFLINE_CANARY;
	configStoreConfigOffline();
}

static void _resetCalibOffline(void)
{
	memcpy(&calibOffline.gammaCurve, &ILI9341_defaultGammaCurve, sizeof(ILI9341_GammaCurve_t));
	calibOffline.timings.offsetLowVoltageSwitchOff = 3300;
	calibOffline.timings.offsetMosfetSwitchStart = 380;
	calibOffline.timings.offsetZeroCross = 570;
	calibOffline.toolT210.calibrated = 0;
	calibOffline.toolT210.calibrationPointTemp[0] = 4640;
	calibOffline.toolT210.calibrationPointTemp[1] = 5720;
	calibOffline.toolT210.calibrationPointTemp[2] = 6620;
	calibOffline.toolT245.calibrated = 0;
	calibOffline.toolT245.calibrationPointTemp[0] = 4640;
	calibOffline.toolT245.calibrationPointTemp[1] = 5720;
	calibOffline.toolT245.calibrationPointTemp[2] = 6620;

	calibOffline.canary = CALIBOFFLINE_CANARY;
	configStoreCalibOffline();
}

void configReset(void)
{
	_resetConfigOnline();
	_resetConfigOffline();
}

void configLoad(void)
{
	eepromReadBlock(CONFIGOFFSET, (uint8_t*)&configOffline, sizeof(ConfigOffline_t));
	eepromReadBlock(CALIBOFFSET, (uint8_t*)&calibOffline, sizeof(CalibOffline_t));

	if (configOnline->canary != CONFIGONLINE_CANARY) {
		notifySend(WARNING_BATTERY);
		_resetConfigOnline();
	}

	if (configOffline.canary != CONFIGOFFLINE_CANARY) {
		notifySend(WARNING_SETTINGSLOST);
		_resetConfigOffline();
	}

	if (calibOffline.canary != CALIBOFFLINE_CANARY) {
		notifySend(ALARM_CALIBRATION);
		_resetCalibOffline();
	}

	configLoaded = 1;
}

ConfigOnline_t* configGetConfigOnlineMaxWait(uint32_t maxwait)
{
	if (xSemaphoreTake(_shMutexConfigOnline, maxwait) == pdPASS) {
		return configOnline;
	}
	return NULL;
}

ConfigOnline_t* configGetConfigOnlineNoWrite(void)
{
	return configOnline;
}

void configReturnConfigOnline(void)
{
	xSemaphoreGive(_shMutexConfigOnline);
}

ConfigOffline_t* configGetConfigOfflineMaxWait(uint32_t maxwait)
{
	if (xSemaphoreTake(_shMutexConfigOffline, maxwait) == pdPASS) {
		return &configOffline;
	}
	return NULL;
}

void configReturnConfigOffline(void)
{
	xSemaphoreGive(_shMutexConfigOffline);
}

void configStoreConfigOffline(void)
{
	eepromWriteBlock(CONFIGOFFSET, (uint8_t*)&configOffline, sizeof(ConfigOffline_t));
}

CalibOffline_t* configGetCalibOfflineMaxWait(uint32_t maxwait)
{
	if (xSemaphoreTake(_shMutexCalibOffline, maxwait) == pdPASS) {
		return &calibOffline;
	}
	return NULL;
}

void configReturnCalibOffline(void)
{
	xSemaphoreGive(_shMutexCalibOffline);
}

void configStoreCalibOffline(void)
{
	eepromWriteBlock(CALIBOFFSET, (uint8_t*)&calibOffline, sizeof(CalibOffline_t));
}

uint8_t configIsLoaded(void)
{
	return configLoaded;
}


