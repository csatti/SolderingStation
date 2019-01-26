#ifndef NVIC_H_INCLUDED
#define NVIC_H_INCLUDED

#include "board.h"

#define NVIC_PRIORITYGROUPING    (7 - __NVIC_PRIO_BITS)

void nvicRTCConfiguration(void);
void nvicUSARTDebugDMAConfiguration(void);
void nvicUSARTDebugConfiguration(void);
void nvicTimeoutConfiguration(void);
void nvicIRRemoteConfiguration(void);
void nvicKeysEXTIConfiguration(void);
void nvicFlashDMAConfiguration(void);
void nvicAnalogConfiguration(void);
void nvicAnalogDMAConfiguration(void);
void nvicDetectConfiguration(void);
void nvicCTRLTimerConfiguration(void);
void nvicRTOSTimerConfiguration(void);
void nvicEEPROMi2cConfiguration(void);
void nvicEEPROMi2cDMAConfiguration(void);

#endif /* NVIC_H_INCLUDED */
