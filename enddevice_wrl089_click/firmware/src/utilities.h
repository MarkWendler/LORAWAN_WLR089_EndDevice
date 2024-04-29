/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _UTILITIES_H    /* Guard against multiple inclusion */
#define _UTILITIES_H

#include "mls.h"
#include "mls_app.h"
#include "lorawan_demo.h"

void DEMO_ProcessRestorePrompt(void);
void DEMO_PrintECCInfo(void);
void DEMO_JoinNotification(StackRetStatus_t xStatus);

#ifdef CONF_PMM_ENABLE
void DEMO_WakeupNotification(uint32_t sleptDuration);
#endif

#define LED_GREEN_ON    LED_GREEN_Set();
#define LED_GREEN_OFF   LED_GREEN_Clear();
#define LED_BLUE_ON     LED_BLUE_Set();
#define LED_BLUE_OFF    LED_BLUE_Clear();
#define LED_RED_ON      LED_RED_Set();
#define LED_RED_OFF     LED_RED_Clear();

#endif /* _EXAMPLE_FILE_NAME_H */
/* *****************************************************************************
 End of File
 */

