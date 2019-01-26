/* Copyright (C) 2017, Attila Kovács
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

/* This file is part of CameraTrigger.
 */

#include "ext.h"
#include <string.h>
#include <ctype.h>

void delay_ms(uint16_t ms)
{
    uint16_t i,j;
    for( i = 0; i < ms; i++ )
    {
        for( j = 0; j < 2000; j++ ) __NOP();
    }
}

static char *long_to_string_with_divisor(char *p,
        long num,
        unsigned radix,
        long divisor)
{
    int i;
    char *q;
    long l, ll;

    l = num;
    if (divisor == 0)
    {
        ll = num;
    }
    else
    {
        ll = divisor;
    }

    q = p + 11;
    do
    {
        i = (int)(l % radix);
        i += '0';
        if (i > '9')
            i += 'A' - '0' - 10;
        *--q = i;
        l /= radix;
    }
    while ((ll /= radix) != 0);

    i = (int)(p + 11 - q);
    do
        *p++ = *q++;
    while (--i);
    *p++ = 0;

    return p;
}

char *ltoa(long num, char *p, unsigned radix)
{

    return long_to_string_with_divisor(p, num, radix, 0);
}

/** \brief Left pads string with characters until size reached
 *
 * \param p char* String to be padded
 * \param c char  Padding character
 * \param num int Final number of characters
 * \return char*  Pointer to string
 *
 */
char *strpadleft(char *p, char c, int num)
{
    int len = strlen(p);
    int i;
    char *d = p + len;
    char *t = p + num;

    if (len >= num) return p;

    for (i = 0; i < len + 1; i++)
    {
        *t-- = *d--;
    }

    t = p;
    for (i = len; i < num; i++)
        *t++ = c;

    return p;
}

/** \brief Right pads string with characters until size reached
 *
 * \param p char* String to be padded
 * \param c char  Padding character
 * \param num int Final number of characters
 * \return char*  Pointer to string
 *
 */
char *strpadright(char *p, char c, int num)
{
    int len = strlen(p);
    int i;
    char *t = p + len;

    if (len >= num) return p;

    for (i = len; i < num; i++)
        *t++ = c;
    *t = 0;

    return p;
}

/** \brief Adds leading zeros to string until size reached
 *
 * \param p char* String to be padded
 * \param num int Final number of characters
 * \return char*  Pointer to string
 *
 */
char *strleadingzeros(char *p, int num)
{
    return strpadleft(p, '0', num);
}

char *strpadspaces(char *p, int num)
{
    return strpadright(p, ' ', num);
}

/*int strncmpi (const char *a, const char *b, int len)
{
    for (; len && toupper(*a) == toupper(*b); --len, ++a, ++b);
    return (toupper(*a) - toupper(*b));
}*/
