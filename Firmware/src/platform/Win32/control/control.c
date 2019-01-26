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

#include "control.h"
#include "analog.h"
#include <math.h>

static DataExchange_t _externalData;

uint8_t* pu8VoltageScope;
uint8_t* pu8CurrentScope;

uint16_t u16Marker[MARKER_NUM];

static TaskHandle_t _thScopeTask = NULL;

DataExchange_t* controlGetDataExchangeMaxWait(uint32_t maxwait)
{

	_externalData.state.ambientTempInt = analogGetAmbientTemp();
	_externalData.state.supplyVoltage = analogGetStoredSupplyVoltage();

	_externalData.state.tool1.toolType = TOOL_T245;
	_externalData.state.tool2.toolType = TOOL_T210;

	_externalData.state.tool1.toolState = TOOL_WARMUP;
	_externalData.state.tool2.toolState = TOOL_READY;

	_externalData.state.tool1.toolTempInt = 5550;
	_externalData.state.tool2.toolTempInt = 4780;

	_externalData.state.railPower = 1600;
	_externalData.state.railVoltage = 240;
	_externalData.state.railCurrent = 68;
	_externalData.state.refVoltageRaw = 550;
	_externalData.state.tool1.thermalThrottle = 3000;
	_externalData.state.tool2.thermalThrottle = 5000;
	_externalData.state.tool1.toolTempRaw = 1111;
	_externalData.state.tool2.toolTempRaw = 2222;

	return (DataExchange_t*)&_externalData;
}

void controlReturnDataExchange(void)
{
	;
}

void processLockScope(void)
{
	uint8_t *u = pu8VoltageScope;
	uint8_t *i = pu8CurrentScope;
	static uint16_t offs = 0;

	offs++;
	for (uint16_t j = 0; j < SCOPE_LEN; j++)
	{
		double f;

		f = abs(sin(3.1415 * (j + offs) / 256) * 128);
		*u++ = (uint8_t)f;
		f = abs(cos(3.1415 * (j + offs) / 256) * 128);
		*i++ = (uint8_t)f;
	}

	u16Marker[0] = offs & 0x00FF;
}

void controlInit(void)
{
	pu8VoltageScope = (uint8_t*)pvPortMalloc(SCOPE_LEN);
	pu8CurrentScope = (uint8_t*)pvPortMalloc(SCOPE_LEN);
	memset(u16Marker, 0, sizeof(u16Marker));
}

void processSetScopeTask(TaskHandle_t task)
{
	_thScopeTask = task;
}

void processSetScopeDataIsUsed(void)
{
	if (_thScopeTask != NULL) {
		xTaskNotify( _thScopeTask, NOTIFY_NEWDATATRIGGER | NOTIFY_HEATTRIGGER, eSetBits);
	}
}

