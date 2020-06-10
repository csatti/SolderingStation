/* Copyright (C) 2018 - 2020, Attila Kovács
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

#include "control.h"
#include "process.h"
#include "timing.h"
#include "analog.h"
#include "message.h"
#include "config.h"
#include "notify.h"
#include "sound.h"
#include "utility.h"
#include <string.h>

#define MAX_DELAY			pdMS_TO_TICKS(40)
#define WAIT_TIME			pdMS_TO_TICKS(5)
#define SHORTWAIT_TIME		pdMS_TO_TICKS(2)

#define MUTEX_ENTER()			xSemaphoreTake(_shMutexMeasurements, portMAX_DELAY)
#define MUTEX_ENTER_NOWAIT()	xSemaphoreTake(_shMutexMeasurements, 0)
#define MUTEX_EXIT()			xSemaphoreGive(_shMutexMeasurements)

#define CONTROL_TASK_PRIORITY       ( configMAX_PRIORITIES - 1 )

static TaskHandle_t _thControlTask = NULL;

static volatile uint16_t _u16HeatPulses1 = 0;
static volatile uint16_t _u16HeatPulses2 = 0;
static volatile uint8_t _u8HeatTool1 = 0;
static volatile uint8_t _u8EnableStation = 0;
static volatile uint8_t _u8CurrentLimit = 0;
static volatile uint8_t _u8VoltageLimit = 0;
static volatile uint8_t _u8AmbientTempLimit = 0;


static volatile SemaphoreHandle_t _shMutexMeasurements = NULL;

static volatile DataExchange_t _externalData;
static volatile InternalData_t _internalData;

static InputState_t _determineInputState(uint16_t input)
{
	// Input is sampled as analog value
	// Pullup resistor present for all inputs, if no pulldown signal is OFF
	if (input > INPUT_HIGH)
		return INPUT_OFF;
	if (input < INPUT_LOW)
		return INPUT_ON;
	// Between state, signal is not valid
	return INPUT_UNDEFINED;
}

// Toggles between tools
static void _switchTool(void)
{
	_u8HeatTool1 = !_u8HeatTool1;
	if (_u8HeatTool1) {
		HEATING_SelTool_SET();
	} else {
		HEATING_SelTool_CLR();
	}
}

// Activates tool 1
static void _selectTool1(void)
{
	_u8HeatTool1 = 1;
	HEATING_SelTool_SET();
}

// Activates tool 2
static void _selectTool2(void)
{
	_u8HeatTool1 = 0;
	HEATING_SelTool_CLR();
}

// Measures the active tool/iron dependent analog inputs
static void _measureIronInputs(void)
{
	if (_u8HeatTool1) {
		// Tool 1 selected
		_internalData.internal.tool1.holderRaw = analogGetMuxChannel(MUX_IRON1_IN_HOLDER);
		_internalData.state.tool1.holder = _determineInputState(_internalData.internal.tool1.holderRaw);

		_internalData.internal.tool1.ironIDRaw = analogGetMuxChannel(MUX_IRON1_TOOLID);
		_internalData.state.tool1.ironID = _determineInputState(_internalData.internal.tool1.ironIDRaw);
		_internalData.state.tool1.toolTempRaw = analogGetChannel(IRON_TEMP);
	} else {
		// Tool 2 selected
		_internalData.internal.tool2.holderRaw = analogGetMuxChannel(MUX_IRON2_IN_HOLDER);
		_internalData.state.tool2.holder = _determineInputState(_internalData.internal.tool2.holderRaw);
		_internalData.internal.tool2.ironIDRaw = analogGetMuxChannel(MUX_IRON2_TOOLID);
		_internalData.state.tool2.ironID = _determineInputState(_internalData.internal.tool2.ironIDRaw);
		_internalData.state.tool2.toolTempRaw = analogGetChannel(IRON_TEMP);
	}
}

// Compensate temperature values
static void _compensateRawTemperatures(void)
{
	uint16_t cnum = 0;

	if (_internalData.state.tool1.holder == INPUT_ON) cnum++;
	if (_internalData.state.tool1.ironID == INPUT_ON) cnum++;
	if (_internalData.state.tool1.toolChange == INPUT_ON) cnum++;
	if (_internalData.state.tool2.ironID != INPUT_UNDEFINED) {
		if (_internalData.state.tool2.holder == INPUT_ON) cnum++;
		if (_internalData.state.tool2.ironID == INPUT_ON) cnum++;
	}

	if (_internalData.state.tool1.ironID == INPUT_OFF) {
		_internalData.state.tool1.toolTempRaw += OFFSET_T245 * cnum;
	} else {
		_internalData.state.tool1.toolTempRaw += OFFSET_T210 * cnum;
	}

	if (_internalData.state.tool2.ironID == INPUT_OFF) {
		_internalData.state.tool2.toolTempRaw += OFFSET_T245 * cnum;
	} else {
		_internalData.state.tool2.toolTempRaw += OFFSET_T210 * cnum;
	}
}

// Measures common analog inputs
static void _measureCommonInputs(void)
{
	// Reference voltage (offset voltage for thermocouple gain amp)
	_internalData.state.refVoltageRaw = analogGetChannel(REF_VOLTAGE);
	// Ambient temperature measured at front panel PCB
	_internalData.state.ambientTempInt = analogGetAmbientTemp();
	// MCU supply voltage
	_internalData.state.supplyVoltage = analogGetStoredSupplyVoltage();
	// Common tool change signal
	_internalData.internal.tool1.toolChangeRaw = analogGetMuxChannel(MUX_TOOL_CHANGE);
	_internalData.state.tool1.toolChange = _determineInputState(_internalData.internal.tool1.toolChangeRaw);
	_internalData.internal.tool2.toolChangeRaw = _internalData.internal.tool1.toolChangeRaw;
	_internalData.state.tool2.toolChange = _internalData.state.tool1.toolChange;

	// Store starting temperature for cold junction compensation
	if (!_internalData.state.startTempInt) {
		if ((_internalData.state.ambientTempInt < AMBIENTTEMP_LIMIT) && (_internalData.state.ambientTempInt > TEMP_ZERO))
			_internalData.state.startTempInt = _internalData.state.ambientTempInt;
	}


	// Check for high ambient temperature
	if ((_internalData.state.ambientTempInt >= AMBIENTTEMP_LIMIT) && (_u8EnableStation)) {
		_u8AmbientTempLimit++;
		if (_u8AmbientTempLimit > 100) {
			// Disable station
			_u8EnableStation = 0;
			// Send alarm to user
			notifySend(ALARM_HIGHAMBIENTTEMPERATURE);
		}
	} else {
		_u8AmbientTempLimit = 0;
	}

	// Mutex for process data
	if (processLockDataNow() == pdTRUE) {

		// Check for overcurrent or short circuit
		if ((u32ProcessRMSCurrent >= CURRENT_LIMIT) && (_u8EnableStation)) {
			_u8CurrentLimit++;
			if (_u8CurrentLimit > 2) {
				// Disable station
				_u8EnableStation = 0;
				// Send alarm to user
				notifySend(ALARM_OVERCURRENT);
			}
		} else {
			_u8CurrentLimit = 0;
		}

		// Check for overvoltage
		if ((u32ProcessRMSVoltage >= VOLTAGE_LIMIT) && (_u8EnableStation)) {
			_u8VoltageLimit++;
			if (_u8VoltageLimit > 2) {
				// Disable station
				_u8EnableStation = 0;
				// Send alarm to user
				notifySend(ALARM_OVERVOLTAGE);
			}
		} else {
			_u8VoltageLimit = 0;
		}

		// Release mutex
		processUnlockData();
	}

}

// Raw ADC tip temperature conversion to internal temperature format (0.1 °F)
static uint16_t convertRawTempToInternal(uint16_t rawTemp, const uint16_t* calibPoints, ADCtoTemp_t* calib)
{
	int32_t calibDiff, tempDiff, ratio, temp;

	// If not calibrated return 0
	if (!calib->calibrated)
		return 0;

	// ADC value below gain amp reference voltage, return ambient temperature
	if (rawTemp < _internalData.state.refVoltageRaw)
		return _internalData.state.ambientTempInt;

	// Remove offset voltage from signal
	temp = (int32_t) ((int32_t)rawTemp - (int32_t)_internalData.state.refVoltageRaw);

	// Scaling signal with calibration data
	calibDiff = (int32_t) (*(calibPoints + 2)) - (int32_t) (*calibPoints);
	tempDiff = (int32_t) (calib->calibrationPointTemp[2]) - (int32_t) (calib->calibrationPointTemp[0]);
	temp -= (int32_t) (*calibPoints);
	// Faulty calibration data, return 0
	if (tempDiff <= 0)
		return 0;
	calibDiff *= (int32_t) 1000;
	ratio = calibDiff / tempDiff;
	temp *= (int32_t) 1000;
	temp /= ratio;
	temp += (int32_t) (calib->calibrationPointTemp[0]);
	// Offset measured temperature with ambient temperature (cold junction compensation)
#ifdef USE_STARTING_TEMPERATURE
	temp += (int32_t) _internalData.state.startTempInt;
#else
	temp += (int32_t) _internalData.state.ambientTempInt;
#endif

	/*if (temp < (int32_t)_internalData.state.ambientTempInt) return _internalData.state.ambientTempInt;*/

	return (uint16_t) temp;
}

// Determine tool state
static void _calculateState(volatile ToolData_t* tool, uint8_t* controlOn, uint8_t calibration)
{
	CalibOffline_t* CalibOffline = configGetCalibOfflineNow(); // Mutex
	const uint16_t* calibPoints;
	ADCtoTemp_t* calib;

	// Structure is taken, let's try next cycle
	if (CalibOffline == NULL)
		return;

	// Power board is configured by using option resistors, middle value (undefined) means that the tool option is disabled
	if (tool->ironID == INPUT_UNDEFINED) {
		tool->toolType = TOOL_NOTYPE;
		tool->toolState = TOOL_DISABLED;
		tool->toolTempInt = TEMP_ZERO;
		configReturnCalibOffline();
		return;
	}

	// If tip temperature is outside valid range, tip is not present
	if ((tool->toolTempRaw < TIP_VALID_LOW) || (tool->toolTempRaw > TIP_VALID_HIGH)) {
		tool->toolType = TOOL_NOTYPE;
		// Reset measured top power output for tip as it is no longer present
		tool->internal->maxRailPowerForTool = 0;
		// Reset temperature valid counter
		tool->internal->tempValid = 0;
	} else {
		if (tool->toolType == TOOL_NOTYPE) {
			tool->internal->tempValid++;
		}
		if (tool->internal->tempValid >= VALUE_GOOD) {
			switch (tool->ironID) {
				// If ironID present, cartridge type is C210
				case INPUT_ON:
					tool->toolType = TOOL_T210;
					calibPoints = CalibPointsT210;
					calib = &(CalibOffline->toolT210);
					break;
				// If ironID not present, cartridge type is C245
				case INPUT_OFF:
					tool->toolType = TOOL_T245;
					calibPoints = CalibPointsT245;
					calib = &(CalibOffline->toolT245);
					break;
				default:;
			}
		}
	}


	if (tool->toolType != TOOL_NOTYPE) {
		// Tool present, calculate tip temperature
		tool->toolTempInt = convertRawTempToInternal(tool->toolTempRaw, calibPoints, calib);

		// Overtemperature detection
		if ((tool->toolTempInt >= TIPTEMP_LIMIT) && (_u8EnableStation)) {
			tool->internal->overTemp++;
			if (tool->internal->overTemp >= VALUE_GOOD) {
				// Disable station
				_u8EnableStation = 0;
				// Send alarm to user
				notifySend(ALARM_HIGHTIPTEMPERATURE);
			}
		} else {
			tool->internal->overTemp = 0;
		}

		if (calibration) {
			// Calibration mode
			tool->toolState = TOOL_CALIBRATION;
			// Tip temperature is not valid
			tool->toolTempInt = TEMP_ZERO;
		} else if ((!tool->toolTempInt) || (!_u8EnableStation)) {
			// Fault
			tool->toolState = TOOL_FAULT;
			tool->toolTempInt = TEMP_ZERO;
			// Switch off
			*controlOn = 0;
		} else if (!*controlOn) {
			// Tool switched off
			if (tool->toolTempInt > (tool->toolState == TOOL_OFF? TEMP_COLD + 180 : TEMP_COLD)) {
				// Tip is cooling down
				tool->toolState = TOOL_COOLDOWN;
			} else {
				// Off
				tool->toolState = TOOL_OFF;
			}
		} else if (tool->holder == INPUT_ON) {
			// Tool is in sleep mode
			tool->toolState = TOOL_SLEEP;

			// If sleep timer is not running, start it
			if (xTimerIsTimerActive(tool->internal->sleepTimer) == pdFALSE) {
				tool->internal->setpointSleepTempAct = tool->toolTempInt;
				if (tool->internal->setpointSleepTempAct > _internalData.control.setpointTemp) tool->internal->setpointSleepTempAct = _internalData.control.setpointTemp;
				if (tool->internal->setpointSleepTempAct < _internalData.control.setpointSleepTemp) tool->internal->setpointSleepTempAct = _internalData.control.setpointSleepTemp;

				uint32_t period = ((uint32_t)_internalData.control.delayOff * 60 * 1000) / portTICK_PERIOD_MS;

				xTimerChangePeriod(tool->internal->sleepTimer, period, 0);
				xTimerStart(tool->internal->sleepTimer, 0);

				period = ((uint32_t)_internalData.control.dropTemp * 10) / portTICK_PERIOD_MS;
				xTimerChangePeriod(tool->internal->tempDropTimer, period, 0);
				xTimerStart(tool->internal->tempDropTimer, 0);
			}
		} else {

			// If sleep timer is running, stop it
			if (xTimerIsTimerActive(tool->internal->sleepTimer) == pdTRUE)
				xTimerStop(tool->internal->sleepTimer, 0);

			// If temperature drop timer is running, stop it
			if (xTimerIsTimerActive(tool->internal->tempDropTimer) == pdTRUE)
				xTimerStop(tool->internal->tempDropTimer, 0);

			if (tool->toolChange == INPUT_ON) {
				// Tool change detected
				tool->toolState = TOOL_CHANGE;
			} else if (tool->toolTempInt + 180 < _internalData.control.setpointTemp) {
				// Tip is warming up
				tool->toolState = TOOL_WARMUP;
				tool->internal->tempReached = 0;
			} else {
				if ((!tool->internal->tempReached) && (tool->toolTempInt >= _internalData.control.setpointTemp)) {
					// Tip temperature reached setpoint
					tool->internal->tempReached = 1;
					// Audible signal to user
					soundNotice();
				}
				// State signal with hysteresis
				tool->toolState = (tool->internal->tempReached ? TOOL_READY : TOOL_WARMUP);
			}
		}
	} else {
		// Tool not present
		tool->toolTempInt = TEMP_ZERO;
		tool->toolState = TOOL_NONE;
		*controlOn = 0;
	}

	// Release mutex
	configReturnCalibOffline();
}

// Transfers data between internal and external data structure
static void _synchornizeData(void)
{
	// Mutex for external data structure access
	if (MUTEX_ENTER_NOWAIT() == pdTRUE) {
		// Control signals
		// Tool 1/2 control signals are on/off triggers
		if (_externalData.control.tool1) {
			_internalData.control.tool1 = !_internalData.control.tool1;
			_externalData.control.tool1 = 0;
		}
		if (_externalData.control.tool2) {
			_internalData.control.tool2 = !_internalData.control.tool2;
			_externalData.control.tool2 = 0;
		}
		_internalData.control.calibPointTool1 = _externalData.control.calibPointTool1;
		_internalData.control.setpointTemp = _externalData.control.setpointTemp;
		_internalData.control.setpointSleepTemp = _externalData.control.setpointSleepTemp;
		_internalData.control.delayOff = _externalData.control.delayOff;
		_internalData.control.dropTemp = _externalData.control.dropTemp;
		_internalData.control.powerLimit = _externalData.control.powerLimit;

		// State signals
		memcpy((void*) &_externalData.state, (void*) &_internalData.state, sizeof(_internalData.state));

		// Release mutex
		MUTEX_EXIT();
	}
}

// Calibration control
static uint8_t _calibration(void)
{
	uint16_t target, step;

	if (_internalData.state.tool1.toolState == TOOL_CALIBRATION) {
		// Calibration mode

		// Checks if calibration point selection is valid
		if ((_internalData.control.calibPointTool1 > 0)
				&& (_internalData.control.calibPointTool1 < 4)) {

			step = _internalData.control.calibPointTool1 - 1;
			target = (_internalData.state.tool1.toolType == TOOL_T245 ?
							CalibPointsT245[step] : CalibPointsT210[step]);
			target += _internalData.state.refVoltageRaw;

			// Simple pulsing control, if target is above current ADC value, heat for one cycle
			if (target > _internalData.state.tool1.toolTempRaw)
				_u16HeatPulses1 = 2;

			// Signal that calibration mode is selected
			return 1;
		}
	}

	// Signal that calibration mode is not selected
	return 0;
}

// Temperature control
static uint16_t _controlTool(volatile ToolData_t* tool)
{
	uint16_t tempDiff, maxPulse, target;

	// If filtered power output reaches limiting value, skip a cycle
	if (_internalData.state.railPower > ((_internalData.control.powerLimit + POWER_OVERLIMIT) * 10)) {
		tool->internal->toolPulse = 0;
		tool->internal->toolPrevTempInt = tool->toolTempInt;
		return (uint16_t) 0;
	}

	// If throttling value is above limit, warn user and switch on throttling signal
	if ((tool->thermalThrottle >= THERMAL_LIMIT) && (!tool->throttlingOn)) {
		notifySend(WARNING_THERMALTHROTTLING);
		tool->throttlingOn = 1;
	} else if (tool->thermalThrottle <= THERMAL_RETURNTONORMAL)
		tool->throttlingOn = 0;

	// Checks for tool off states and throttle limit
	if (((tool->toolState != TOOL_WARMUP) && (tool->toolState != TOOL_READY)
			&& (tool->toolState != TOOL_SLEEP)) || (tool->thermalThrottle >= THERMAL_LIMIT) ) {
		tool->internal->toolPulse = 0;
		tool->internal->toolPrevTempInt = tool->toolTempInt;
		return (uint16_t) 0;
	}

	// Determine target temperature (normal or sleep)
	target = (tool->toolState != TOOL_SLEEP ? _internalData.control.setpointTemp : tool->internal->setpointSleepTempAct);

	// If temperature is above setpoint, no heating required
	if (tool->toolTempInt >= target) {
		tool->internal->toolPulse = 0;
		tool->internal->toolPrevTempInt = tool->toolTempInt;
		return (uint16_t) 0;
	}

	// Calculate temperature difference between setpoint and current values
	tempDiff = target - tool->toolTempInt;

	// If temperature difference higher then 100 Kelvins, max power
	if (tempDiff > 1800)
		tool->internal->toolPulse = PULSE_MAX;

	// If possible try to calculate how many heating cycles is required to reach target temperature
	if ((tool->internal->toolPrevTempInt < tool->toolTempInt) && (tool->internal->toolPulse > 0)) {
		uint16_t heatVal = (tool->toolTempInt - tool->internal->toolPrevTempInt) / tool->internal->toolPulse;
		if (heatVal) tool->internal->toolPulse = tempDiff / heatVal;
	}

	// Determine maximum allowed pulse number (if close to target, less is allowed)
	maxPulse = (tempDiff > 180 ? PULSE_MAX : PULSE_MAX_DIFF_10K);  // 180 => 10 Kelvin
	// If filtered power output reaches nominal value, stop heating
	maxPulse = (_internalData.state.railPower > (_internalData.control.powerLimit * 10) ? 0 : maxPulse);
	
	if (tool->internal->maxRailPowerForTool > _internalData.state.railPower) {
		uint16_t maxPulseBeforeOver = (AVERAGING_SAMPLENUM * ((_internalData.control.powerLimit * 10) - _internalData.state.railPower)) / (tool->internal->maxRailPowerForTool - _internalData.state.railPower);
		if (maxPulse > maxPulseBeforeOver) maxPulse = maxPulseBeforeOver;
		if ((maxPulse < PULSE_MIN) && (maxPulse)) maxPulse = PULSE_MIN;
	}

	// Limit maximum pulse in sleep mode
	if (tool->toolState == TOOL_SLEEP) maxPulse = PULSE_MIN;

	// Increase previous heating cycle number by two
	tool->internal->toolPulse += 2;

	// Check if pulse number is bellow minimum allowed (removes toroid noise)
	if (tool->internal->toolPulse < PULSE_MIN) tool->internal->toolPulse = PULSE_MIN;
	// Limits pulse number
	if (tool->internal->toolPulse > maxPulse) tool->internal->toolPulse = maxPulse;

	if (tool->internal->toolPulse % 2) tool->internal->toolPulse++;

	// Saves previous temperature
	tool->internal->toolPrevTempInt = tool->toolTempInt;
	// Returns calculated heating pulse number
	return tool->internal->toolPulse;
}

// Convert internal temperature value (0.1 °F) for telemetry use (0.1 °C)
static uint16_t _internalTempToTelemetry(uint16_t intTemp)
{
	int32_t conv = (int32_t)intTemp;

	conv -= 320;
	conv *= 10;
	conv /= 18;
	if (conv < 0) conv = 0;
	return (uint16_t)conv;
}

// Send telemetry data
static void _telemetryData(void)
{
	telemetryHeader();
	// Tool 1 data
	telemetryU16(_internalData.state.tool1.toolTempRaw);
	telemetryU16(_internalTempToTelemetry(_internalData.state.tool1.toolTempInt));
	telemetryU16(_internalData.state.tool1.thermalThrottle);
	telemetryU16(_internalData.internal.tool1.toolPulse);
	telemetryU16(_internalData.state.tool2.toolTempRaw);
	// Tool 2 data
	telemetryU16(_internalTempToTelemetry(_internalData.state.tool2.toolTempInt));
	telemetryU16(_internalData.state.tool2.thermalThrottle);
	telemetryU16(_internalData.internal.tool2.toolPulse);
	telemetryU16(_internalTempToTelemetry(_internalData.state.ambientTempInt));
	// Common data
	telemetryU16(_internalData.state.supplyVoltage);
	telemetryU16(_internalTempToTelemetry(analogGetStoredMCUTemperature()));
	telemetryFloat((float)u32ProcessRMSPower);
	telemetryFloat((float)u32ProcessRMSVoltage);
	telemetryFloat((float)u32ProcessRMSCurrent);
	telemetryU16(_internalTempToTelemetry(_internalData.control.setpointTemp));
	telemetryFloat((float)u32ProcessMaxVoltage);
	telemetryFloat((float)u32ProcessMaxCurrent);
	// "Digital" inputs
	telemetryU16(_internalData.state.tool1.holder);
	telemetryU16(_internalData.state.tool2.holder);
	telemetryU16(_internalData.state.tool1.toolChange);
	telemetryU16(_internalData.state.tool1.ironID);
	telemetryU16(_internalData.state.tool2.ironID);
	// Filtered data
	telemetryU16(_internalData.state.railVoltage);
	telemetryU16(_internalData.state.railCurrent);
	telemetryU16(_internalData.state.railPower);
	// Raw "digital" inputs
	telemetryU16(_internalData.internal.tool1.holderRaw);
	telemetryU16(_internalData.internal.tool2.holderRaw);
	telemetryU16(_internalData.internal.tool1.toolChangeRaw);
	telemetryU16(_internalData.internal.tool1.ironIDRaw);
	telemetryU16(_internalData.internal.tool2.ironIDRaw);
	telemetrySend();
}

// Send debug data in readable format
static void _debugData(void)
{
	const char inputTexts[][10] = { "OFF", "ON", "UNDEFINED" };
	char str[20];

	debugTimeRef();
	ltoa(_internalData.state.tool1.toolTempRaw, str, 10);
	debugPrint(TXT("T1, ADC temp: "));
	debugPrint(str);
	ltoa(_internalTempToTelemetry(_internalData.state.tool1.toolTempInt), str, 10);
	debugPrint(TXT(" Temp: "));
	debugPrint(str);
	ltoa(_internalData.state.refVoltageRaw, str, 10);
	debugPrint(TXT(" ZeroRef: "));
	debugPrintLn(str);

	debugTimeRef();
	ltoa(_internalData.state.tool2.toolTempRaw, str, 10);
	debugPrint(TXT("T2, ADC temp: "));
	debugPrint(str);
	ltoa(_internalTempToTelemetry(_internalData.state.tool2.toolTempInt), str, 10);
	debugPrint(TXT(" Temp: "));
	debugPrintLn(str);
	debugTimeRef();
	ltoa(u32ProcessRMSPower, str, 10);
	debugPrint(TXT("Power: "));
	debugPrint(str);
	ltoa(u32ProcessRMSVoltage, str, 10);
	debugPrint(TXT(" Voltage: "));
	debugPrint(str);
	ltoa(u32ProcessRMSCurrent, str, 10);
	debugPrint(TXT(" Current: "));
	debugPrintLn(str);

	debugTimeRef();
	debugPrint(TXT("Holder 1: "));
	debugPrint(inputTexts[_internalData.state.tool1.holder]);
	debugPrint(TXT(" Holder 2: "));
	debugPrint(inputTexts[_internalData.state.tool2.holder]);
	debugPrint(TXT(" Tool change: "));
	debugPrint(inputTexts[_internalData.state.tool1.toolChange]);
	debugPrint(TXT(" ID 1: "));
	debugPrint(inputTexts[_internalData.state.tool1.ironID]);
	debugPrint(TXT(" ID 2: "));
	debugPrintLn(inputTexts[_internalData.state.tool2.ironID]);
}

// Controller task
static void _controlTask(void *pvParameters)
{
	uint32_t notifyVal;
	uint8_t toggleEnable = 0, heatingRunning = 0;
	uint16_t halfWaveCount;
	int dbgCycle = 0;
	char str[20];

	// Set this task for notification for timing interrupt handler
	timingSetControlTask(_thControlTask);

	// Wait until timings are adjusted
	while ((!analogIsFrequencyDetermined()) || (!timingIsFrequencyDetermined())) {
		vTaskDelay(WAIT_TIME);
	}
	debugTimeRef();
	debugPrintLn(TXT("Frequency measured, ADC readjusted."));

	// Wait until voltage and current mid levels are determined
	while (!processIsTuned()) {
		vTaskDelay(WAIT_TIME);
	}
	debugTimeRef();
	debugPrintLn(TXT("Voltage and current level tuned."));

	// Wait until configuration data is loaded
	while (!configIsLoaded()) {
		vTaskDelay(WAIT_TIME);
	}
	timingUpdateTimings();
	debugTimeRef();
	debugPrintLn(TXT("Timings adjusted. Ready to rock'n'roll."));

	// Return measured half wave time (one cycle)
	halfWaveCount = timingGetHalfWaveCount();
	debugTimeRef();
	ltoa(halfWaveCount, str, 10);
	debugPrint(TXT("Half wave length: "));
	debugPrint(str);
	debugPrintLn(TXT(" us"));

	while (1) {
		// Wait until the timing interrupt handler sends us an event
		notifyVal = waitForAnyEvent(
				NOTIFY_MOSFETSTART | NOTIFY_LOWSWITCHOFF | NOTIFY_PEAK | NOTIFY_ZEROCROSS, MAX_DELAY);

		if (!notifyVal) {
			// Invalid event, or no event in allocated time
			debugTimeRef();
			debugPrintLnC(TXT("No control event or other failure!"), LCD_RED);
		} else {
			// Event handling

			if (notifyVal & NOTIFY_MOSFETSTART) {
				// Mosfet switch on/off time (lowest internal heating at this time)
				if (_u16HeatPulses1) {
					// Heating for tool 1
					_selectTool1();
					_u16HeatPulses1--;
					HEATING_Heating_ON();
					heatingRunning = 1 + PULSE_WAIT;
					if (_internalData.state.tool1.toolType == TOOL_T210) heatingRunning++;
					// Increase thermal throttle value
					_internalData.state.tool1.thermalThrottle += (_internalData.state.tool1.toolType == TOOL_T210 ?
												THERMAL_ADD_T210 : THERMAL_ADD_T245);
				} else if (_u16HeatPulses2) {
					// Heating for tool 2
					_selectTool2();
					_u16HeatPulses2--;
					HEATING_Heating_ON();
					heatingRunning = 1 + PULSE_WAIT;
					if (_internalData.state.tool2.toolType == TOOL_T210) heatingRunning++;
					// Increase thermal throttle value
					_internalData.state.tool2.thermalThrottle += (_internalData.state.tool2.toolType == TOOL_T210 ?
												THERMAL_ADD_T210 : THERMAL_ADD_T245);
				} else {
					// No heating at this time
					HEATING_Heating_OFF();
					// Decrease thermal throttle values
					if (_internalData.state.tool1.thermalThrottle)
						_internalData.state.tool1.thermalThrottle--;

					if (_internalData.state.tool2.thermalThrottle)
						_internalData.state.tool2.thermalThrottle--;
					// Decrease no heating signal (can be used as a delay for temperature measurement)
					if (heatingRunning)
						heatingRunning--;
				}

			}

			if (notifyVal & NOTIFY_LOWSWITCHOFF) {
				// Switch off time for low power tips (C210) which can not handle full power

				if (heatingRunning) {
					if (((_u8HeatTool1) && (_internalData.state.tool1.toolType != TOOL_T245))
							|| ((!_u8HeatTool1) && (_internalData.state.tool2.toolType != TOOL_T245))) {
						// If selected tip is C210, switch off heating
						HEATING_Heating_OFF();
					}
				}

			}

			if (notifyVal & NOTIFY_ZEROCROSS) {
				// Zero cross time, used for debug and telemetry data send

				dbgCycle++;
				dbgCycle %= 20;

				// Lock process data mutex
				if (processLockDataNow() == pdTRUE) {

					// Filter RMS rail signals
					/*filterSignalFact(&filteredVoltage, u32ProcessRMSVoltage, 4);
					filterSignalFact(&filteredCurrent, u32ProcessRMSCurrent, 4);
					filterSignalFact(&filteredPower, u32ProcessRMSPower, 4);*/

					// Convert filtered signals to usable format
					/*_internalData.state.railVoltage = (uint16_t)(filteredSignalValueFact(filteredVoltage, 4) / 100UL);
					_internalData.state.railCurrent = (uint16_t)(filteredSignalValueFact(filteredCurrent, 4) / 100UL);
					_internalData.state.railPower = (uint16_t)(filteredSignalValueFact(filteredPower, 4) / 100UL);*/
					_internalData.state.railVoltage = (uint16_t)(u32FilteredVoltage / 100UL);
					_internalData.state.railCurrent = (uint16_t)(u32FilteredCurrent / 100UL);
					_internalData.state.railPower = (uint16_t)(u32FilteredPower / 100UL);

					// Measure top power output for current tip
					if (heatingRunning) {
						uint16_t crmsPower =  (uint16_t)(u32ProcessRMSPower / 100UL);
						if (_u8HeatTool1) {
							if (crmsPower > _internalData.internal.tool1.maxRailPowerForTool)
								_internalData.internal.tool1.maxRailPowerForTool = crmsPower;
						} else {
							if (crmsPower > _internalData.internal.tool2.maxRailPowerForTool)
								_internalData.internal.tool2.maxRailPowerForTool = crmsPower;
						}
					}

					if (telemetryIsEnabled())
						// Send telemetry data
						_telemetryData();
					else if (!dbgCycle)
						// Send debug data
						_debugData();

					// Release process data mutex
					processUnlockData();
				}

			}

			if (notifyVal & NOTIFY_PEAK) {
				// Voltage peak reached

				if (_u8EnableStation) {
					// If station is enabled, toggle heating enabled signal

					// This signal has to be toggled otherwise it expires and the heating shuts down (HW safety for SW crash)
					if (toggleEnable)
						HEATING_Enable_SET();
					else
						HEATING_Enable_CLR();

					toggleEnable = !toggleEnable;
				}

				if (!heatingRunning) {
					// If no heating is running currently, measure inputs
					// High currents during heating makes measurements impossible for these values

					// Measure currently selected tool values
					_measureIronInputs();
					// Switch to other tool
					_switchTool();
					// Measure common input values
					_measureCommonInputs();
					// Measure now the other tool values
					_measureIronInputs();

					// Adjust raw temperatures by input current loss
					_compensateRawTemperatures();

					// Calculate tool states
					_calculateState(&_internalData.state.tool1, (uint8_t*)&_internalData.control.tool1,
							_internalData.control.calibPointTool1);
					_calculateState(&_internalData.state.tool2, (uint8_t*)&_internalData.control.tool2, 0);

					// Check for calibration mode
					if (_calibration()) {
						// Calibration
						_u16HeatPulses2 = 0;
						_internalData.control.tool1 = 0;
						_internalData.control.tool2 = 0;
					} else {
						// Normal control
						_u16HeatPulses1 = _controlTool(&_internalData.state.tool1);
						_u16HeatPulses2 = _controlTool(&_internalData.state.tool2);
					}

					// Synchronize data between internal and external data structure
					_synchornizeData();
				}
			}
		}
	}
}

// Return external data structure (if mutex lock is successful)
DataExchange_t* controlGetDataExchangeMaxWait(uint32_t maxwait)
{
	if (xSemaphoreTake(_shMutexMeasurements, maxwait) == pdPASS) {
		return (DataExchange_t*) &_externalData;
	}
	return NULL;
}

// Release mutex lock
void controlReturnDataExchange(void)
{
	xSemaphoreGive(_shMutexMeasurements);
}

// Drop sleeping temperature
static void _tempDropTimerCallback(TimerHandle_t xTimer)
{
	uint32_t id = (uint32_t)pvTimerGetTimerID(xTimer);
	switch (id) {
		case 1:
			_internalData.internal.tool1.setpointSleepTempAct -= 18;
			if (_internalData.internal.tool1.setpointSleepTempAct <= _internalData.control.setpointSleepTemp) {
				_internalData.internal.tool1.setpointSleepTempAct = _internalData.control.setpointSleepTemp;
				xTimerStop(xTimer, 0);
			}
			break;
		case 2:
			_internalData.internal.tool2.setpointSleepTempAct -= 18;
			if (_internalData.internal.tool2.setpointSleepTempAct <= _internalData.control.setpointSleepTemp) {
				_internalData.internal.tool2.setpointSleepTempAct = _internalData.control.setpointSleepTemp;
				xTimerStop(xTimer, 0);
			}
			break;
		default:;
	}
}

// Sleep timer callback
static void _sleepTimerCallback(TimerHandle_t xTimer)
{
	uint32_t id = (uint32_t)pvTimerGetTimerID(xTimer);
	switch (id) {
		case 1:
			if (_internalData.control.tool1) {
				_internalData.control.tool1 = 0;
				soundEvent();
			}
			break;
		case 2:
			if (_internalData.control.tool2) {
				_internalData.control.tool2 = 0;
				soundEvent();
			}
			break;
		default:;
	}

}

// Initialize control task
void controlInit(void)
{
	// Initialize process (voltage and current measurement and processing)
	processInit();

	// Create control task
	xTaskCreate(_controlTask, (char *) "Control", 200, NULL,
			CONTROL_TASK_PRIORITY, &_thControlTask);

	// Create mutex for data exchange control
	_shMutexMeasurements = xSemaphoreCreateMutex();

	vQueueAddToRegistry(_shMutexMeasurements, TXT("CtrlMx"));

	// Initialize internal data state
	_internalData.control.tool1 = 0;
	_internalData.control.tool2 = 0;
	_internalData.control.calibPointTool1 = 0;
	_internalData.control.delayOff = 1;
	_internalData.control.dropTemp = 10;

	_internalData.state.tool1.holder = INPUT_OFF;
	_internalData.state.tool1.ironID = INPUT_OFF;
	_internalData.state.tool1.toolChange = INPUT_OFF;
	_internalData.state.tool1.toolTempInt = TEMP_ZERO;

	_internalData.state.tool1.toolState = TOOL_SETUP;
	_internalData.state.tool1.toolType = TOOL_NOTYPE;
	_internalData.state.tool1.thermalThrottle = 0;
	_internalData.state.tool1.throttlingOn = 0;

	memcpy((void*) &_internalData.state.tool2, (void*) &_internalData.state.tool1, sizeof(_internalData.state.tool1));
	_internalData.state.tool1.internal = (ToolDataInt_t*)&_internalData.internal.tool1;
	_internalData.state.tool2.internal = (ToolDataInt_t*)&_internalData.internal.tool2;

	_internalData.internal.tool1.toolPrevTempInt = TEMP_ZERO;
	_internalData.internal.tool1.toolPulse = 0;
	_internalData.internal.tool1.tempReached = 0;
	_internalData.internal.tool1.overTemp = 0;
	_internalData.internal.tool1.tempValid = 0;
	_internalData.internal.tool1.maxRailPowerForTool = 0;
	memcpy((void*) &_internalData.internal.tool2, (void*) &_internalData.internal.tool1, sizeof(_internalData.internal.tool1));

	_internalData.state.ambientTempInt = TEMP_ZERO;
	_internalData.state.supplyVoltage = 0;
	_internalData.state.refVoltageRaw = 0;
	_internalData.state.railVoltage = 0;
	_internalData.state.railCurrent = 0;
	_internalData.state.railPower = 0;
	_internalData.state.startTempInt = 0;

	// Create timers for sleep mode switch off
	_internalData.internal.tool1.sleepTimer = xTimerCreate( TXT("Tool1OffTimer"), 1, pdFALSE, (void *) 1, _sleepTimerCallback);
	_internalData.internal.tool2.sleepTimer = xTimerCreate( TXT("Tool2OffTimer"), 1, pdFALSE, (void *) 2, _sleepTimerCallback);

	// Create timers for slow temperature dropping during sleep mode
	_internalData.internal.tool1.tempDropTimer = xTimerCreate( TXT("Tool1TempTimer"), 1, pdTRUE, (void *) 1, _tempDropTimerCallback);
	_internalData.internal.tool2.tempDropTimer = xTimerCreate( TXT("Tool2TempTimer"), 1, pdTRUE, (void *) 2, _tempDropTimerCallback);

	// Copy initialized data from internal to external data structure
	memcpy((void*) &_externalData.control, (void*) &_internalData.control, sizeof(Control_t));
	memcpy((void*) &_externalData.state, (void*) &_internalData.state, sizeof(State_t));

	// Enable station
	_u8EnableStation = 1;

}

