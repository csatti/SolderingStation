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

#ifndef SRC_DRIVER_WIN32_BOARD_ARCH_H_
#define SRC_DRIVER_WIN32_BOARD_ARCH_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define STRCONC(str1,str2) str1 ## str2
#define TOKENPASTE(x, y) STRCONC(x, y)

// Sections (most of these resources changes rarely, keeping them apart from main code helps with uploading the software during development)
#define FONT                              __attribute__((section(".font")))
#define IMAGE                             __attribute__((section(".image")))
#define LIB                               __attribute__((section(".lib")))
#define CCMRAM                            __attribute__((section(".ccmram")))
#define CCMBSS                            __attribute__((section(".ccmbss")))
#define TXT(s)                            s
#define FTXT

// Attributes
#define PACKED                            __attribute__((__packed__))
#define USED                              __attribute__((used))
#define UNUSEDFUNC                        __attribute__((unused))
#define ALWAYSINLINE                      __attribute__((always_inline)) __STATIC_INLINE
#define __weak   							__attribute__((weak))

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

// Types
typedef uint8_t                           u8;
typedef uint16_t                          u16;
typedef uint32_t                          u32;
typedef uint64_t                          u64;
typedef int8_t                            s8;
typedef int16_t                           s16;
typedef int32_t                           s32;
typedef int64_t                           s64;
typedef uint_least8_t                     ul8;
typedef uint_least16_t                    ul16;
typedef uint_least32_t                    ul32;
typedef int_least8_t                      sl8;
typedef int_least16_t                     sl16;
typedef int_least32_t                     sl32;
typedef uint_fast8_t                      uf8;
typedef uint_fast16_t                     uf16;
typedef uint_fast32_t                     uf32;
typedef uint_fast64_t                     uf64;
typedef int_fast8_t                       sf8;
typedef int_fast16_t                      sf16;
typedef int_fast32_t                      sf32;
typedef int_fast64_t                      sf64;

#endif /* SRC_DRIVER_WIN32_BOARD_ARCH_H_ */
