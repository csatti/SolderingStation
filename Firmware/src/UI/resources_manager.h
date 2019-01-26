#ifndef _RESOURCES_MANAGER_H
#define _RESOURCES_MANAGER_H

#include "gfx.h"
#include "main.h"

// Font indexes
//#define General 0
#define LargeTemp 1
#define SmallTemp 2
#define General 0
#define GeneralSmall 3
#define GeneralLarge 4
#define GeneralTiny 5

#ifdef __cplusplus
extern "C" {
#endif

	bool_t guiResourcesManagerInit(void);
	font_t gstudioGetFont(int fontIndex);

#ifdef __cplusplus
}
#endif

#endif // _RESOURCES_MANAGER_H
