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

#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include "board.h"
#include "lcd.h"
#ifdef SEGGER_JLINK_RTT
#include "SEGGER_RTT.h"
#endif

#define CMD_CMDMODEON   0x00000001
#define CMD_CMDMODEOFF  0x00000002


// Size  must be dividable by 4 bytes, last 4 byte is CRC32
typedef union {
  uint8_t buffer[20];
  struct {
    uint32_t command;
    uint32_t device;   // Fixed
    uint32_t id;
    uint32_t dataSize; // Fixed
    uint32_t CRC32;
  } cmd;
  struct {
    uint32_t command;
    uint32_t device;   // Fixed
    uint32_t position;
    uint32_t dataSize; // Fixed
    uint32_t CRC32;
  } mem;
  struct {
    uint32_t command;
    uint32_t device;   // Fixed
    uint32_t resultOrId;
    uint32_t dataSize; // Fixed
    uint32_t CRC32;
  } rsp;
} DBG_Cmd_t;

// Callback function for debug command processing
typedef void (*DBG_ReceivedCommandProcessor_t)(volatile DBG_Cmd_t*);

// LCD printouts
#ifdef LCDOUT
void msgPrintC(const char *str, uint16_t color);
void msgPrint(const char *str);
void msgPrintLnC(const char *str, uint16_t color);
void msgPrintLn(const char *str);
void msgOk(void);
void msgFail(void);
#else
#define msgPrintC(str, color) DBG_PrintC(str, color)
#define msgPrint(str) DBG_Print(str)
#define msgPrintLnC(str, color) DBG_PrintLnC(str, color)
#define msgPrintLn(str) DBG_PrintLn(str)
#define msgOk() DBG_Ok()
#define msgFail() DBG_Fail()
#endif

#ifdef USARTTELEMETRY
void telemetryHeader(void);
void telemetryU16(uint16_t data);
void telemetryFloat(float data);
void telemetrySend();
void telemetryControl(uint8_t enable);
uint8_t telemetryIsEnabled(void);
#endif

#ifdef USARTDEBUG

// Initialization
void messageInit(void);

// Debug mode functions
void debugPrintC(const char *str, uint16_t color);
void debugPrint(const char *str);
void debugPrintLn(const char *str);
void debugPrintLnC(const char *str, uint16_t color);
void debugTimeRef(void);
void debugPrintChar(char c);
void debugBlockPrint(const char *str);
void debugOk(void);
void debugFail(void);
#else
#define debugSetCmdCallbackFunction(processor)   ((void) 0)
#define debugSendRAW(buff, len)                  ((void) 0)
#define debugWaitTillSending()                   ((void) 0)
#define debugInit()                              ((void) 0)
#define debugPrintC(str, color)                  ((void) 0)
#define debugPrintLnC(str, color)                ((void) 0)
#define debugPrint(str)                          ((void) 0)
#define debugPrintLn(str)                        ((void) 0)
#define debugTimeRef()                           ((void) 0)
#define debugBlockPrint(str)                     ((void) 0)
#define debugOk()                                ((void) 0)
#define debugFail()                              ((void) 0)
#endif
#define DBG_Lock()                ((void) 0)
#define DBG_Unlock()              ((void) 0)

#ifdef MONITOR
void debugMonitoringInit(void);
#else
#define debugMonitoringInit()         ((void) 0)
#endif

#endif /* MESSAGE_H_INCLUDED */
