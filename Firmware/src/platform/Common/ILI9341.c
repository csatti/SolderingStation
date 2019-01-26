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

#include "ILI9341.h"

const ILI9341_GammaCurve_t ILI9341_originalGammaCurve = {
	.VP63 = 0x0f, 	// 4 bit
	.VP62 = 0x31, 	// 6 bit
	.VP61 = 0x2b, 	// 6 bit
	.VP59 = 0x0c, 	// 4 bit
	.VP57 = 0x0e, 	// 5 bit
	.VP50 = 0x08, 	// 4 bit
	.VP43 = 0x4e, 	// 7 bit
	.VP36 = 0x01, 	// 4 bit
	.VP27 = 0x0f, 	// 4 bit
	.VP20 = 0x37, 	// 7 bit
	.VP13 = 0x07, 	// 4 bit
	.VP6  = 0x10, 	// 5 bit
	.VP4  = 0x03, 	// 4 bit
	.VP2  = 0x0e, 	// 6 bit
	.VP1  = 0x09, 	// 6 bit
	.VP0  = 0x00, 	// 4 bit

	.VN63 = 0x00, 	// 4 bit
	.VN62 = 0x0e, 	// 6 bit
	.VN61 = 0x14, 	// 6 bit
	.VN59 = 0x03, 	// 4 bit
	.VN57 = 0x11, 	// 5 bit
	.VN50 = 0x07, 	// 4 bit
	.VN43 = 0x31, 	// 7 bit
	.VN36 = 0x0c, 	// 4 bit
	.VN27 = 0x01, 	// 4 bit
	.VN20 = 0x48, 	// 7 bit
	.VN13 = 0x08, 	// 4 bit
	.VN6  = 0x0f, 	// 5 bit
	.VN4  = 0x0c, 	// 4 bit
	.VN2  = 0x31, 	// 6 bit
	.VN1  = 0x36, 	// 6 bit
	.VN0  = 0x0f, 	// 4 bit
};

const ILI9341_GammaCurve_t ILI9341_defaultGammaCurve = {
	.VP63 = 0x0f, 	// 4 bit
	.VP62 = 0x2d, 	// 6 bit
	.VP61 = 0x21, 	// 6 bit
	.VP59 = 0x0c, 	// 4 bit
	.VP57 = 0x0e, 	// 5 bit
	.VP50 = 0x02, 	// 4 bit
	.VP43 = 0x40, 	// 7 bit
	.VP36 = 0x01, 	// 4 bit
	.VP27 = 0x0f, 	// 4 bit
	.VP20 = 0x26, 	// 7 bit
	.VP13 = 0x07, 	// 4 bit
	.VP6  = 0x10, 	// 5 bit
	.VP4  = 0x03, 	// 4 bit
	.VP2  = 0x2e, 	// 6 bit
	.VP1  = 0x09, 	// 6 bit
	.VP0  = 0x00, 	// 4 bit

	.VN63 = 0x00, 	// 4 bit
	.VN62 = 0x3b, 	// 6 bit
	.VN61 = 0x36, 	// 6 bit
	.VN59 = 0x03, 	// 4 bit
	.VN57 = 0x11, 	// 5 bit
	.VN50 = 0x07, 	// 4 bit
	.VN43 = 0x38, 	// 7 bit
	.VN36 = 0x0c, 	// 4 bit
	.VN27 = 0x01, 	// 4 bit
	.VN20 = 0x48, 	// 7 bit
	.VN13 = 0x0a, 	// 4 bit
	.VN6  = 0x0f, 	// 5 bit
	.VN4  = 0x0c, 	// 4 bit
	.VN2  = 0x30, 	// 6 bit
	.VN1  = 0x36, 	// 6 bit
	.VN0  = 0x0f, 	// 4 bit
};

const ILI9341_GammaCurve_t ILI9341_gammaCurveLimits = {
	.VP63 = 0x0f, 	// 4 bit
	.VP62 = 0x3f, 	// 6 bit
	.VP61 = 0x3f, 	// 6 bit
	.VP59 = 0x0f, 	// 4 bit
	.VP57 = 0x1f, 	// 5 bit
	.VP50 = 0x0f, 	// 4 bit
	.VP43 = 0x7f, 	// 7 bit
	.VP36 = 0x0f, 	// 4 bit
	.VP27 = 0x0f, 	// 4 bit
	.VP20 = 0x7f, 	// 7 bit
	.VP13 = 0x0f, 	// 4 bit
	.VP6  = 0x1f, 	// 5 bit
	.VP4  = 0x0f, 	// 4 bit
	.VP2  = 0x3f, 	// 6 bit
	.VP1  = 0x3f, 	// 6 bit
	.VP0  = 0x0f, 	// 4 bit

	.VN63 = 0x0f, 	// 4 bit
	.VN62 = 0x3f, 	// 6 bit
	.VN61 = 0x3f, 	// 6 bit
	.VN59 = 0x0f, 	// 4 bit
	.VN57 = 0x1f, 	// 5 bit
	.VN50 = 0x0f, 	// 4 bit
	.VN43 = 0x7f, 	// 7 bit
	.VN36 = 0x0f, 	// 4 bit
	.VN27 = 0x0f, 	// 4 bit
	.VN20 = 0x7f, 	// 7 bit
	.VN13 = 0x0f, 	// 4 bit
	.VN6  = 0x1f, 	// 5 bit
	.VN4  = 0x0f, 	// 4 bit
	.VN2  = 0x3f, 	// 6 bit
	.VN1  = 0x3f, 	// 6 bit
	.VN0  = 0x0f, 	// 4 bit
};

void ILI9341_LimitGammaCurve(ILI9341_GammaCurve_t* gammacurve)
{
	uint8_t* curve = (uint8_t*)gammacurve;
	uint8_t* curvelimits = (uint8_t*)&ILI9341_gammaCurveLimits;

	for (int i = 0; i < sizeof(ILI9341_GammaCurve_t); i++)
	{
		*curve++ &= *curvelimits++;
	}
}
