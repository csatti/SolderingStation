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

#ifndef TOOL_H_
#define TOOL_H_

#include "main.h"

typedef enum
{
	TOOL_NOTYPE,							//			No tool connected
    TOOL_T210,								//			Tip type is C245
	TOOL_T245,								//			Tip type is C210
} ToolType_t;

typedef enum
{
	TOOL_NONE,								//			Empty tool
	TOOL_DISABLED,							//			Tool is disabled (circuit is missing)
    TOOL_OFF,								//			Tool is switched off and cold
	TOOL_SLEEP,								//			Tool is sleeping (inside holder)
	TOOL_WARMUP,							//			Tip is warming up towards setpoint temperature
	TOOL_READY,								//			Tool is ready to use
	TOOL_FAULT,								//			Tool fault (incorrect signals or station is disabled)
	TOOL_CALIBRATION,						//			Tool is being calibrated
	TOOL_CHANGE,							//			Tool change mode
	TOOL_SETUP,								//			Startup state
	TOOL_COOLDOWN,							//			Tip is cooling down
} ToolState_t;

typedef enum
{
	INPUT_OFF,								//			OFF state
	INPUT_ON,								//			ON state
	INPUT_UNDEFINED,						//			Undetermined state
} InputState_t;

typedef volatile struct
{
	uint16_t 		toolPrevTempInt;    	// 0.1 °F,	Store pervious temperature (internal use only)
	uint16_t 		toolPulse;				//			Current heating pulse number
	uint8_t			tempReached;			//			Setpoint temperature reached (internal use only)
	uint8_t			overTemp;				//			Overtemperature counter (internal use only)
	uint8_t			tempValid;				//			Temperature valid counter (internal use only)
	uint16_t		holderRaw;
	uint16_t		ironIDRaw;
	uint16_t		toolChangeRaw;
	uint16_t		maxRailPowerForTool;
	uint16_t 		setpointSleepTempAct;	// 0.1 °F,	Active setpoint temperature for sleeping
	TimerHandle_t	sleepTimer;				//			Delay switch off timer from sleep -> off
	TimerHandle_t	tempDropTimer;			//			Temperature drop timer tick during sleep
} ToolDataInt_t;

typedef struct
{
	uint16_t 		toolTempRaw;			//			Raw ADC temperature value
	uint16_t 		toolTempInt;    		// 0.1 °F	Tip temperature
	uint8_t			throttlingOn;			//			Thermal throttling on

	InputState_t	holder;					//			Tool in holder
	InputState_t	toolChange;				//			Tool change detected
	InputState_t	ironID;					//			Tip ID signal

	ToolType_t 		toolType;				//			Tool type
	ToolState_t 	toolState;				//			Tool state
	uint16_t		thermalThrottle;		//			Thermal throttle value
	ToolDataInt_t*	internal;				//			Pointer to internal structure

} ToolData_t;



typedef struct {
	uint8_t			tool1;					//			Tool 1 on/off
	uint8_t			tool2;					//			Tool 2 on/off
	uint8_t			calibPointTool1; 		// 			0 = No calibration, 1-3 = Selected calibration point
	uint16_t 		setpointTemp;			// 0.1 °F,	Setpoint temperature for soldering
	uint16_t 		setpointSleepTemp;		// 0.1 °F,	Setpoint temperature for sleeping
	uint16_t		delayOff;				// min,		Maximum sleep time, auto off afterwards
	uint16_t 		powerLimit;				// W,		Limit maximum output power
	uint16_t		dropTemp;				// s/1.8 °F,	Temperature drop rate during sleep mode
} Control_t;

typedef struct {
	uint16_t 		ambientTempInt;  		// 0.1 °F, 	Ambient temperature measured at front panel PCB
	uint16_t		startTempInt;			// 0.1 °F, 	Ambient temperature during switch on
	uint16_t 		refVoltageRaw;	 		// 			Reference voltage (offset voltage for thermocouple gain amp)
	uint16_t 		supplyVoltage;	 		// mV, 		MCU supply voltage
	uint16_t		railVoltage; 	 		// 0.1 V,	24VAC RMS voltage (filtered)
	uint16_t		railCurrent;	 		// 0.1 A,	24VAC RMS current (filtered)
	uint16_t		railPower;		 		// 0.1 W,	24VAC RMS power (filtered)

	ToolData_t		tool1;					//			Tool 1 state signals
	ToolData_t		tool2;					//			Tool 2 state signals
} State_t;

typedef struct {
	ToolDataInt_t	tool1;					//			Tool 1 state signals
	ToolDataInt_t	tool2;					//			Tool 2 state signals
} Internal_t;

typedef struct{
	Control_t control;
	State_t state;
} DataExchange_t;

typedef struct{
	Control_t control;
	State_t state;
	Internal_t internal;
} InternalData_t;

#endif /* TOOL_H_ */
