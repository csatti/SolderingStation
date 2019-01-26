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

/* This file is part of SolderingStation.
 */


#include "localization.h"

#define LANG_NUM 2

#define STRCONC(str1,str2) str1 ## str2
#define TXT_EL(x)	{ 						\
						STRCONC(ENG_,x) , 	\
						STRCONC(HUN_,x)		\
					}

typedef char const * const text[LANG_NUM];

static const text FTXT texts[LAST_TEXT] =
{
// Empty
	TXT_EL(TEXT00001),

// Generic texts
	TXT_EL(TEXT00002),
	TXT_EL(TEXT00003),
	TXT_EL(TEXT00004),
	TXT_EL(TEXT00005),
	TXT_EL(TEXT00006),
	TXT_EL(TEXT00007),
	TXT_EL(TEXT00008),
	TXT_EL(TEXT00009),
	TXT_EL(TEXT00010),
	TXT_EL(TEXT00011),
	TXT_EL(TEXT00012),
	TXT_EL(TEXT00013),
	TXT_EL(TEXT00014),

// Tool states
	TXT_EL(TEXT01001),
	TXT_EL(TEXT01002),
	TXT_EL(TEXT01003),
	TXT_EL(TEXT01004),
	TXT_EL(TEXT01005),
	TXT_EL(TEXT01006),
	TXT_EL(TEXT01007),
	TXT_EL(TEXT01008),
	TXT_EL(TEXT01009),
	TXT_EL(TEXT01010),
	TXT_EL(TEXT01011),
	TXT_EL(TEXT01101),

// Units
	TXT_EL(TEXT00101),
	TXT_EL(TEXT00102),
	TXT_EL(TEXT00103),
	TXT_EL(TEXT00104),
	TXT_EL(TEXT00105),
	TXT_EL(TEXT00106),
	TXT_EL(TEXT00107),
	TXT_EL(TEXT00108),

// Menu controls
	TXT_EL(TEXT00201),
	TXT_EL(TEXT00202),
	TXT_EL(TEXT00203),
	TXT_EL(TEXT00204),
	TXT_EL(TEXT00205),
	TXT_EL(TEXT00206),
	TXT_EL(TEXT00207),
	TXT_EL(TEXT00208),
	TXT_EL(TEXT00209),
	TXT_EL(TEXT00210),

// Menu items
	TXT_EL(TEXT00301),
	TXT_EL(TEXT00302),
	TXT_EL(TEXT00303),
	TXT_EL(TEXT00304),
	TXT_EL(TEXT00305),
	TXT_EL(TEXT00306),
	TXT_EL(TEXT00307),
	TXT_EL(TEXT00308),
	TXT_EL(TEXT00309),
	TXT_EL(TEXT00310),
	TXT_EL(TEXT00311),
	TXT_EL(TEXT00312),
	TXT_EL(TEXT00313),

// Alarms, warnings and notifications and their descriptions
	TXT_EL(TEXT02001),
	TXT_EL(TEXT02002),
	TXT_EL(TEXT02003),
	TXT_EL(TEXT02011),
	TXT_EL(TEXT02012),
	TXT_EL(TEXT02021),
	TXT_EL(TEXT02022),
	TXT_EL(TEXT02031),
	TXT_EL(TEXT02032),
	TXT_EL(TEXT02041),
	TXT_EL(TEXT02042),
	TXT_EL(TEXT02051),
	TXT_EL(TEXT02052),
	TXT_EL(TEXT02061),
	TXT_EL(TEXT02062),
	TXT_EL(TEXT02071),
	TXT_EL(TEXT02072),
	TXT_EL(TEXT02081),
	TXT_EL(TEXT02082),
	TXT_EL(TEXT02091),
	TXT_EL(TEXT02092),
	TXT_EL(TEXT02101),
	TXT_EL(TEXT02102),
	TXT_EL(TEXT02111),
	TXT_EL(TEXT02112),
	TXT_EL(TEXT02121),
	TXT_EL(TEXT02122),
	TXT_EL(TEXT02131),
	TXT_EL(TEXT02132),

// Dialogs
	TXT_EL(TEXT05011),
	TXT_EL(TEXT05012),

// Settings and their descriptions
	TXT_EL(TEXT03011),
	TXT_EL(TEXT03012),
	TXT_EL(TEXT03021),
	TXT_EL(TEXT03022),
	TXT_EL(TEXT03031),
	TXT_EL(TEXT03032),
	TXT_EL(TEXT03041),
	TXT_EL(TEXT03042),
	TXT_EL(TEXT03051),
	TXT_EL(TEXT03052),
	TXT_EL(TEXT03061),
	TXT_EL(TEXT03062),
	TXT_EL(TEXT03071),
	TXT_EL(TEXT03072),
	TXT_EL(TEXT03081),
	TXT_EL(TEXT03082),
	TXT_EL(TEXT03091),
	TXT_EL(TEXT03092),

// Iron calibration
	TXT_EL(TEXT04001),
	TXT_EL(TEXT04002),
	TXT_EL(TEXT04003),
	TXT_EL(TEXT04004),

};

static uint8_t lang = 0;

void locSetLanguage(uint8_t language)
{
	if (language >= LANG_NUM) {
		lang = 0;
	}
	else
	{
		lang = language;
	}
}

uint8_t locGetLanguage(void)
{
	return lang;
}

char const * GT(int textNo)
{
    return texts[textNo][lang];
}
