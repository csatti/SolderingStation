#ifndef UI_LOCALIZATION_H_
#define UI_LOCALIZATION_H_

#include "loc_eng.h"
#include "loc_hun.h"
#include "version.h"

typedef enum
{
// Empty text
	TEXT_EMPTY,

// Generic texts
	TEXT_ON,
	TEXT_OFF,
	TEXT_ENABLE,
	TEXT_DISABLE,
	TEXT_STATIONNAME,
	TEXT_AUTHOR,
	TEXT_24VACRAIL,
	TEXT_THROTTLE,
	TEXT_ADCTEMP,
	TEXT_ADCREF,
	TEXT_SUPPLYVOLTAGE,
	TEXT_USBFIRMWARE,
	TEXT_INTTEMP,

// Tool states
	TSTEXT_NONE,
	TSTEXT_DISABLED,
	TSTEXT_OFF,
	TSTEXT_SLEEP,
	TSTEXT_WARMUP,
	TSTEXT_READY,
	TSTEXT_FAULT,
	TSTEXT_CALIBRATION,
	TSTEXT_CHANGE,
	TSTEXT_SETUP,
	TSTEXT_COOLDOWN,
	TSTEXT_COLD,

// Units
	UNITTEXT_CELSIUS,
	UNITTEXT_FAHRENHEIT,
	UNITTEXT_MINUTE,
	UNITTEXT_VOLT,
	UNITTEXT_AMPER,
	UNITTEXT_WATT,
	UNITTEXT_MILIVOLT,
	UNITTEXT_DECIMALPOINT,
	UNITTEXT_CELSIUSPERMINUTE,
	UNITTEXT_FAHRENHEITPERMINUTE,

// Menu controls
	MENUCONTROL_BACK,
	MENUCONTROL_SELECT,
	MENUCONTROL_EDIT,
	MENUCONTROL_CANCEL,
	MENUCONTROL_ENTER,
	MENUCONTROL_ACK,	// Acknowledge
	MENUCONTROL_NEXT,
	MENUCONTROL_PREVIOUS,
	MENUCONTROL_OK,
	MENUCONTROL_MODE,

// Menu items
	MENUITEM_SETTINGS,
	MENUITEM_SYSTEM,
	MENUITEM_DUMMY,
	MENUITEM_SOUND_ON,
	MENUITEM_SOUND_OFF,
	MENUITEM_LANGUAGE,
	MENUITEM_IRONCALIBRATION,
	MENUITEM_LCDGAMMA,
	MENUITEM_FIRMWAREUPDATE,
	MENUITEM_SOUNDTEST,
	MENUITEM_RESTART,
	MENUITEM_REPORT,
	MENUITEM_SCOPE,
	MENUITEM_DEFAULTSETTINGS,

// Alarms, warnings and notifications and their descriptions
	NOTIFYTYPE_ALARM,
	NOTIFYTYPE_WARNING,
	NOTIFYTYPE_INFORMATION,
	WARNINGTEXT_BATTERY,
	WARNINGTEXT_BATTERY_DESC,
	WARNINGTEXT_DEBUGMODE,
	WARNINGTEXT_DEBUGMODE_DESC,
	ALARMTEXT_CALIBRATION,
	ALARMTEXT_CALIBRATION_DESC,
	INFOTEXT_IRONCALIBRATION,
	INFOTEXT_IRONCALIBRATION_DESC,
	INFOTEXT_CALIBRATIONDONE,
	INFOTEXT_CALIBRATIONDONE_DESC,
	WARNINGTEXT_UNCALIBRATEDIRON,
	WARNINGTEXT_UNCALIBRATEDIRON_DESC,
	WARNINGTEXT_THERMALTHROTTLING,
	WARNINGTEXT_THERMALTHROTTLING_DESC,
	ALARMTEXT_OVERVOLTAGE,
	ALARMTEXT_OVERVOLTAGE_DESC,
	ALARMTEXT_OVERCURRENT,
	ALARMTEXT_OVERCURRENT_DESC,
	ALARMTEXT_HIGHTIPTEMPERATURE,
	ALARMTEXT_HIGHTIPTEMPERATURE_DESC,
	ALARMTEXT_HIGHAMBIENTTEMPERATURE,
	ALARMTEXT_HIGHAMBIENTTEMPERATURE_DESC,
	WARNINGTEXT_REPORTFAILED,
	WARNINGTEXT_REPORTFAILED_DESC,
	WARNINGTEXT_SETTINGSLOST,
	WARNINGTEXT_SETTINGSLOST_DESC,
	INFOTEXT_SETTINGSRESET,
	INFOTEXT_SETTINGSRESET_DESC,

// Dialogs
	DIALOGTEXT_NOIRON,
	DIALOGTEXT_NOIRON_DESC,

// Settings and their descriptions
	SETTINGTEXT_SLEEPTEMP,
	SETTINGTEXT_SLEEPTEMP_DESC,
	SETTINGTEXT_DELAYSWITCHOFF,
	SETTINGTEXT_DELAYSWITCHOFF_DESC,
	SETTINGTEXT_TEMPERATURESTEP,
	SETTINGTEXT_TEMPERATURESTEP_DESC,
	SETTINGTEXT_MAXTEMPERATURE,
	SETTINGTEXT_MAXTEMPERATURE_DESC,
	SETTINGTEXT_UNITOFTEMPERATURE,
	SETTINGTEXT_UNITOFTEMPERATURE_DESC,
	SETTINGTEXT_TELEMETRY,
	SETTINGTEXT_TELEMETRY_DESC,
	SETTINGTEXT_DIAGNOSTICS,
	SETTINGTEXT_DIAGNOSTICS_DESC,
	SETTINGTEXT_SCREENOFF,
	SETTINGTEXT_SCREENOFF_DESC,
	SETTINGTEXT_POWERLIMIT,
	SETTINGTEXT_POWERLIMIT_DESC,
	SETTINGTEXT_TEMPERATUREDROP,
	SETTINGTEXT_TEMPERATUREDROP_DESC,

// Iron calibration
	CALIBTEXT_1STCALIBRATIONPOINT,
	CALIBTEXT_2NDCALIBRATIONPOINT,
	CALIBTEXT_3RDCALIBRATIONPOINT,
	CALIBTEXT_CALIBRATION_DESC,

// Last text item (program constant, always last)
	LAST_TEXT
} LOCTEXT;

static const char FTXT TEXT_VERSION[] = VERSION;
static const char FTXT TEXT_VERSIONSHORT[] = VERSION_SHORT;

#define GT(x) 		locGetText(x)

void locSetLanguage(uint8_t language);
uint8_t locGetLanguage(void);
char const * locGetText(int textNo);

#endif /* UI_LOCALIZATION_H_ */
