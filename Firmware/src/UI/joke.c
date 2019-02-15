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

#include "joke.h"
#include "sound.h"

// Original source code from: http://processors.wiki.ti.com/index.php/Playing_The_Imperial_March
// Original author's copyright notice is not retrievable (link is dead)

#define c 261
#define d 294
#define e 329
#define f 349
#define g 391
#define gS 415
#define a 440
#define aS 455
#define b 466
#define cH 523
#define cSH 554
#define dH 587
#define dSH 622
#define eH 659
#define fH 698
#define fSH 740
#define gH 784
#define gSH 830
#define aH 880


void jokePlayImperialMarch(void)
{
    soundBeep(a, 500);
    soundBeep(a, 500);
    soundBeep(a, 500);
    soundBeep(f, 350);
    soundBeep(cH, 150);
    soundBeep(a, 500);
    soundBeep(f, 350);
    soundBeep(cH, 150);
    soundBeep(a, 650);

    soundBeep(0, 150);
    //end of first bit

    soundBeep(eH, 500);
    soundBeep(eH, 500);
    soundBeep(eH, 500);
    soundBeep(fH, 350);
    soundBeep(cH, 150);
    soundBeep(gS, 500);
    soundBeep(f, 350);
    soundBeep(cH, 150);
    soundBeep(a, 650);

    soundBeep(0, 150);
    //end of second bit...

    soundBeep(aH, 500);
    soundBeep(a, 300);
    soundBeep(a, 150);
    soundBeep(aH, 400);
    soundBeep(gSH, 200);
    soundBeep(gH, 200);
    soundBeep(fSH, 125);
    soundBeep(fH, 125);
    soundBeep(fSH, 250);

    soundBeep(0, 250);

    soundBeep(aS, 250);
    soundBeep(dSH, 400);
    soundBeep(dH, 200);
    soundBeep(cSH, 200);
    soundBeep(cH, 125);
    soundBeep(b, 125);
    soundBeep(cH, 250);

    soundBeep(0, 250);

    soundBeep(f, 125);
    soundBeep(gS, 500);
    soundBeep(f, 375);
    soundBeep(a, 125);
    soundBeep(cH, 500);
    soundBeep(a, 375);
    soundBeep(cH, 125);
    soundBeep(eH, 650);

    //end of third bit... (Though it doesn't play well)
    //let's repeat it

    soundBeep(aH, 500);
    soundBeep(a, 300);
    soundBeep(a, 150);
    soundBeep(aH, 400);
    soundBeep(gSH, 200);
    soundBeep(gH, 200);
    soundBeep(fSH, 125);
    soundBeep(fH, 125);
    soundBeep(fSH, 250);

    soundBeep(0, 250);

    soundBeep(aS, 250);
    soundBeep(dSH, 400);
    soundBeep(dH, 200);
    soundBeep(cSH, 200);
    soundBeep(cH, 125);
    soundBeep(b, 125);
    soundBeep(cH, 250);

    soundBeep(0, 250);

    soundBeep(f, 250);
    soundBeep(gS, 500);
    soundBeep(f, 375);
    soundBeep(cH, 125);
    soundBeep(a, 500);
    soundBeep(f, 375);
    soundBeep(cH, 125);
    soundBeep(a, 650);
    //end of the song

}


