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

#ifndef UI_LOC_ENG_H_
#define UI_LOC_ENG_H_

#include "arch.h"

// Empty
#define ENG_TEXT00001 ""

// Generic texts
#define ENG_TEXT00002 "On"
#define ENG_TEXT00003 "Off"
#define ENG_TEXT00004 "Enabled"
#define ENG_TEXT00005 "Disabled"
#define ENG_TEXT00006 "Soldering Station"
#define ENG_TEXT00007 "©2019 Attila Kovács"
#define ENG_TEXT00008 "24 VAC rail:"
#define ENG_TEXT00009 "Thermal throttle:"
#define ENG_TEXT00010 "ADC temperature:"
#define ENG_TEXT00011 "ADC TC reference:"
#define ENG_TEXT00012 "Supply voltage:"
#define ENG_TEXT00013 "Connect the USB cable and install the new firmware. Good Luck!"
#define ENG_TEXT00014 "Temperature .1F:"

// Tool states
#define ENG_TEXT01001 "Empty"
#define ENG_TEXT01002 ""
#define ENG_TEXT01003 "Off"
#define ENG_TEXT01004 "Sleep"
#define ENG_TEXT01005 "Heating"
#define ENG_TEXT01006 "Ready"
#define ENG_TEXT01007 "Fault"
#define ENG_TEXT01008 "Calibrate"
#define ENG_TEXT01009 "Change"
#define ENG_TEXT01010 "Set up"
#define ENG_TEXT01011 "Cooldn."
#define ENG_TEXT01101 "Cold"

// Units
#define ENG_TEXT00101 "°C"
#define ENG_TEXT00102 "°F"
#define ENG_TEXT00103 "min."
#define ENG_TEXT00104 "V"
#define ENG_TEXT00105 "A"
#define ENG_TEXT00106 "W"
#define ENG_TEXT00107 "mV"
#define ENG_TEXT00108 "."		// Decimal point

// Menu controls
#define ENG_TEXT00201 "Back"
#define ENG_TEXT00202 "Select"
#define ENG_TEXT00203 "Edit"
#define ENG_TEXT00204 "Cancel"
#define ENG_TEXT00205 "Enter"
#define ENG_TEXT00206 "Ack."	// Acknowledge
#define ENG_TEXT00207 "Next"
#define ENG_TEXT00208 "Previous"
#define ENG_TEXT00209 "OK"
#define ENG_TEXT00210 "Mode"

// Menu items
#define ENG_TEXT00301 "Settings"
#define ENG_TEXT00302 "System"
#define ENG_TEXT00303 "Dummy"
#define ENG_TEXT00304 "Sound: On"
#define ENG_TEXT00305 "Sound: Off"
#define ENG_TEXT00306 "Language: English"			// Write name of current language here
#define ENG_TEXT00307 "Soldering iron calibration"
#define ENG_TEXT00308 "LCD gamma adjustments"
#define ENG_TEXT00309 "Firmware update"
#define ENG_TEXT00310 "Sound test"
#define ENG_TEXT00311 "Restart"
#define ENG_TEXT00312 "Report"
#define ENG_TEXT00313 "Scope"
#define ENG_TEXT00314 "Default settings"

// Alarms, warnings and notifications and their descriptions
#define ENG_TEXT02001 "Alarm!"
#define ENG_TEXT02002 "Warning!"
#define ENG_TEXT02003 "Information"
#define ENG_TEXT02011 "Short term settings are lost!"
#define ENG_TEXT02012 "Low battery voltage (CR1220) or incompatible SW version"
#define ENG_TEXT02021 "Debug mode active!"
#define ENG_TEXT02022 "Debug firmware is less efficient"
#define ENG_TEXT02031 "Calibration settings are lost!"
#define ENG_TEXT02032 "The soldering station has to be calibrated before use"
#define ENG_TEXT02041 "Soldering iron calibration"
#define ENG_TEXT02042 "Connect the iron under calibration to the first input"
#define ENG_TEXT02051 "Calibration done"
#define ENG_TEXT02052 "The calibration is finished successfully"
#define ENG_TEXT02061 "Uncalibrated iron!"
#define ENG_TEXT02062 "The connected soldering iron is not calibrated"
#define ENG_TEXT02071 "Thermal throttling!"
#define ENG_TEXT02072 "The thermal power has reached the safety limit"
#define ENG_TEXT02081 "Overvoltage!"
#define ENG_TEXT02082 "Overvoltage detected on the 24VAC rail, station is disabled"
#define ENG_TEXT02091 "Overcurrent or short circuit!"
#define ENG_TEXT02092 "Overcurrent detected on the 24VAC rail, station is disabled"
#define ENG_TEXT02101 "High tip temperature!"
#define ENG_TEXT02102 "The tip temperature reached the maximum safety limit, station is disabled"
#define ENG_TEXT02111 "High internal temperature!"
#define ENG_TEXT02112 "The internal temperature reached the maximum safety limit, station is disabled"
#define ENG_TEXT02121 "Reporting is unsuccessful!"
#define ENG_TEXT02122 "Disable telemetry"
#define ENG_TEXT02131 "User settings are lost!"
#define ENG_TEXT02132 "Updated from incompatible SW version"
#define ENG_TEXT02141 "Default settings"
#define ENG_TEXT02142 "The user settings have been reset to their default values"

// Dialogs
#define ENG_TEXT05011 "No iron detected!"
#define ENG_TEXT05012 "There is no iron connected to the first input"

// Settings and their descriptions
#define ENG_TEXT03011 "Sleep temperature:"
#define ENG_TEXT03012 "Tip temperature set point when mounted on holder"
#define ENG_TEXT03021 "Delayed switch off:"
#define ENG_TEXT03022 "Delay time for complete heating switch off when sleeping"
#define ENG_TEXT03031 "Temperature step:"
#define ENG_TEXT03032 "Smallest temperature adjustment for manual temperature setting"
#define ENG_TEXT03041 "Maximum temperature limit:"
#define ENG_TEXT03042 "Highest allowed temperature set point"
#define ENG_TEXT03051 "Unit of temperature:"
#define ENG_TEXT03052 "Display unit of temperature"
#define ENG_TEXT03061 "Telemetry:"
#define ENG_TEXT03062 "Telemetry data output via USB CDC (virtual serial port)"
#define ENG_TEXT03071 "Diagnostics:"
#define ENG_TEXT03072 "Measured parameters are displayed on the main screen"
#define ENG_TEXT03081 "Screen saver:"
#define ENG_TEXT03082 "Delay time for screen backlight switch off"
#define ENG_TEXT03091 "Output power limit:"
#define ENG_TEXT03092 "The soldering station limits its output power at this value"

// Iron calibration
#define ENG_TEXT04001 "First calibration point:"
#define ENG_TEXT04002 "Second calibration point:"
#define ENG_TEXT04003 "Third calibration point:"
#define ENG_TEXT04004 "Set the measured temperature of the tip after it stabilized"

#endif /* UI_LOC_ENG_H_ */
