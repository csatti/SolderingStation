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

#ifndef __ILI9341_H
#define __ILI9341_H

#include "arch.h"

#define GDISP_CONTROL_GAMMACURVE	(GDISP_CONTROL_LLD + 10)

#define ILI9341_GAMMACURVEPOINTS	16

typedef struct {
	// Positive gamma voltage
	uint8_t VP63; 	// 4 bit
	uint8_t VP62; 	// 6 bit
	uint8_t VP61; 	// 6 bit
	uint8_t VP59; 	// 4 bit
	uint8_t VP57; 	// 5 bit
	uint8_t VP50; 	// 4 bit
	uint8_t VP43; 	// 7 bit
	uint8_t VP36; 	// 4 bit
	uint8_t VP27; 	// 4 bit
	uint8_t VP20; 	// 7 bit
	uint8_t VP13; 	// 4 bit
	uint8_t VP6; 	// 5 bit
	uint8_t VP4; 	// 4 bit
	uint8_t VP2; 	// 6 bit
	uint8_t VP1; 	// 6 bit
	uint8_t VP0; 	// 4 bit
	// Negative gamma voltage
	uint8_t VN63; 	// 4 bit
	uint8_t VN62; 	// 6 bit
	uint8_t VN61; 	// 6 bit
	uint8_t VN59; 	// 4 bit
	uint8_t VN57; 	// 5 bit
	uint8_t VN50; 	// 4 bit
	uint8_t VN43; 	// 7 bit
	uint8_t VN36; 	// 4 bit
	uint8_t VN27; 	// 4 bit
	uint8_t VN20; 	// 7 bit
	uint8_t VN13; 	// 4 bit
	uint8_t VN6; 	// 5 bit
	uint8_t VN4; 	// 4 bit
	uint8_t VN2; 	// 6 bit
	uint8_t VN1; 	// 6 bit
	uint8_t VN0; 	// 4 bit
} ILI9341_GammaCurve_t;

extern const ILI9341_GammaCurve_t ILI9341_defaultGammaCurve;
extern const ILI9341_GammaCurve_t ILI9341_gammaCurveLimits;

#define LCD_VERTICAL_MAX        		320
#define LCD_HORIZONTAL_MAX      		240

#define ILI9341_DEVICE_CODE_READ_REG    0x00
#define ILI9341_SOFT_RESET_REG			0x01
#define ILI9341_IDENTINFO_R_REG			0x04
#define ILI9341_STATUS_R_REG			0x09
#define ILI9341_POWERMODE_R_REG			0x0A
#define ILI9341_MADCTL_R_REG			0x0B
#define ILI9341_PIXFORMAT_R_REG			0x0C
#define ILI9341_IMGFORMAT_R_REG			0x0D
#define ILI9341_SIGMODE_R_REG			0x0E
#define ILI9341_SD_RESULT_R_REG			0x0F
#define ILI9341_SLEEP_ENTER_REG			0x10
#define ILI9341_SLEEP_OUT_REG			0x11
#define ILI9341_PARTIALMODE_REG			0x12
#define ILI9341_NORDISPMODE_REG			0x13
#define ILI9341_INVERSIONOFF_REG		0x20
#define ILI9341_INVERSIONON_REG			0x21
#define ILI9341_GAMMASET_REG			0x26
#define ILI9341_DISPLAYOFF_REG			0x28
#define ILI9341_DISPLAYON_REG			0x29
#define ILI9341_COLADDRSET_REG			0x2A
#define ILI9341_PAGEADDRSET_REG			0x2B
#define ILI9341_MEMORYWRITE_REG			0x2C
#define ILI9341_COLORSET_REG			0x2D
#define ILI9341_MEMORYREAD_REG			0x2E
#define ILI9341_PARTIALAREA_REG			0x30
#define ILI9341_VERTSCROLL_REG			0x33
#define ILI9341_TEAREFFECTLINEOFF_REG	0x34
#define ILI9341_TEAREFFECTLINEON_REG	0x35
#define ILI9341_MEMACCESS_REG			0x36
#define ILI9341_VERSCRSRART_REG			0x37
#define ILI9341_IDLEMODEOFF_REG			0x38
#define ILI9341_IDLEMODEON_REG			0x39
#define ILI9341_PIXFORMATSET_REG		0x3A
#define ILI9341_WRITEMEMCONTINUE_REG	0x3C
#define ILI9341_READMEMCONTINUE_REG		0x3E
#define ILI9341_SETTEATSCAN_REG			0x44
#define ILI9341_GETSCANLINE_REG			0x45
#define ILI9341_WRITEBRIGHT_REG			0x51
#define ILI9341_READBRIGHT_REG			0x52
#define ILI9341_WRITECTRL_REG			0x53
#define ILI9341_READCTRL_REG			0x54
#define ILI9341_WRITECABC_REG			0x55
#define ILI9341_READCABC_REG			0x56
#define ILI9341_WRITECABCMB_REG			0x5E
#define ILI9341_READCABCMB_REG			0x5F
#define ILI9341_RGB_ISCTL_REG			0xB0
#define ILI9341_FRAMECTL_NOR_REG		0xB1
#define ILI9341_FRAMECTL_IDLE_REG		0xB2
#define ILI9341_FRAMECTL_PARTIAL_REG	0xB3
#define ILI9341_INVERCTL_REG			0xB4
#define ILI9341_BLANKPORCTL_REG			0xB5
#define ILI9341_FUNCTONCTL_REG			0xB6
#define ILI9341_ENTRYMODE_REG			0xB7
#define ILI9341_BLIGHTCTL1_REG			0xB8
#define ILI9341_BLIGHTCTL2_REG			0xB9
#define ILI9341_BLIGHTCTL3_REG			0xBA
#define ILI9341_BLIGHTCTL4_REG			0xBB
#define ILI9341_BLIGHTCTL5_REG			0xBC
#define ILI9341_BLIGHTCTL7_REG			0xBE
#define ILI9341_BLIGHTCTL8_REG			0xBF
#define ILI9341_POWERCTL1_REG			0xC0
#define ILI9341_POWERCTL2_REG			0xC1
#define ILI9341_VCOMCTL1_REG			0xC5
#define ILI9341_VCOMCTL2_REG			0xC7
#define ILI9341_POWERCTLA_REG			0xCB
#define ILI9341_POWERCTLB_REG			0xCF
#define ILI9341_NVMEMWRITE_REG			0xD0
#define ILI9341_NVMEMPROTECTKEY_REG		0xD1
#define ILI9341_NVMEMSTATUS_REG			0xD2
#define ILI9341_READID4_REG				0xD3
#define ILI9341_READID1_REG				0xDA
#define ILI9341_READID2_REG				0xDB
#define ILI9341_READID3_REG				0xDC
#define ILI9341_POSGAMMACORRECTION_REG	0xE0
#define ILI9341_NEGGAMMACORRECTION_REG	0xE1
#define ILI9341_DIGGAMCTL1_REG			0xE2
#define ILI9341_DIGGAMCTL2_REG			0xE3
#define ILI9341_DIVTIMCTL_A_REG			0xE8
#define ILI9341_DIVTIMCTL_B_REG			0xEA
#define ILI9341_POWONSEQCTL_REG			0xED
#define ILI9341_ENABLE_3G_REG			0xF2
#define ILI9341_INTERFCTL_REG			0xF6
#define ILI9341_PUMPRATIOCTL_REG		0xF7


#define ILI9341_MADCTL_MY       0x80
#define ILI9341_MADCTL_MX       0x40
#define ILI9341_MADCTL_MV       0x20
#define ILI9341_MADCTL_ML       0x10
#define ILI9341_MADCTL_RGB      0x00
#define ILI9341_MADCTL_BGR      0x08
#define ILI9341_MADCTL_MH       0x04


void ILI9341_LimitGammaCurve(ILI9341_GammaCurve_t* gammacurve);

#endif
