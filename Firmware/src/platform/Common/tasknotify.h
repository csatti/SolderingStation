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

#ifndef PLATFORM_COMMON_TASKNOTIFY_H_
#define PLATFORM_COMMON_TASKNOTIFY_H_

/* Public task notification signals */

// For process task
#define NOTIFY_ADCVOLTAGE		0x01000000UL
#define NOTIFY_ADCCURRENT		0x02000000UL
#define NOTIFY_ADCFAULT			0x04000000UL // TODO: implement transaction error for DMA

// For control task
#define NOTIFY_MOSFETSTART		0x01000000UL
#define NOTIFY_LOWSWITCHOFF		0x02000000UL
#define NOTIFY_PEAK				0x04000000UL
#define NOTIFY_ZEROCROSS		0x08000000UL

// For scope task
#define NOTIFY_NEWDATATRIGGER	0x01000000UL
#define NOTIFY_HEATTRIGGER		0x02000000UL
#define NOTIFY_EXITSCOPE		0x00010000UL


#endif /* PLATFORM_COMMON_TASKNOTIFY_H_ */
