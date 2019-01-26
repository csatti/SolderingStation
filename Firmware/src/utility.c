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

#include "utility.h"
#include "main.h"

// Wait for event notification from interrupt handlers
uint32_t waitForAnyEvent(uint32_t notify, uint32_t maxDelay)
{
	BaseType_t result;
	uint32_t notifyVal = 0, prevVal = 0;

	// Wait until notification
	do {
		// Store accumulated events
		prevVal = notifyVal;
		// Wait for notification
		result = xTaskNotifyWait( pdTRUE, notify, &notifyVal, maxDelay);
		// Combine new event with previous events
		notifyVal |= prevVal;
		// If no event in allocated time, generate fault
		if (result != pdPASS) {
			return 0;
		}
		// Wait until any of the required events is collected
	} while ((notifyVal & notify) == 0);

	return notifyVal;
}

void filterSignalFact(uint32_t* filteredVal, uint32_t rawVal, uint16_t fact)
{
	if (!*filteredVal) {
		*filteredVal = rawVal << fact;
		return;
	}

	*filteredVal += rawVal - (*filteredVal >> fact);
}

uint32_t filteredSignalValueFact(uint32_t val, uint16_t fact)
{
	return val >> fact;
}
