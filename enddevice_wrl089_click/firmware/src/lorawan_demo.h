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

#ifndef _LORAWAN_DEMO_H    /* Guard against multiple inclusion */
#define _LORAWAN_DEMO_H

#include "configuration.h"
#include "conf_regparams.h"
#include "conf_pmm.h"
#include "conf_sal.h"

#if (ENABLE_PDS == 1)
/* Period of the restorePromptTimer - default: 1s */
#define DEMOAPP_RESTOREPERIODTIMER_MS   (1000U)

/* Number of attempts for restoration */
#define DEMOAPP_RESTOREATTEMPT_MAX      (5U)
#endif /* #if (ENABLE_PDS == 1)*/

#define DEMOAPP_PROCESS_TASK()          \
    do { \
        DEMO_RunTask(); \
    } while(0);

/* Enumerate the application states */
typedef enum _AppTaskState_t
{
    APP_INIT_STATE,
    APP_SEND_STATE,
    APP_JOIN_STATE,
    APP_IDLE_STATE
#if (ENABLE_PDS == 1)
    ,RESTORE_PROMPT_STATE
#endif
}AppTaskState_t;

SYSTEM_TaskStatus_t APP_TaskHandler(void);
void DEMO_Init(void);

void DEMO_RunTask(void);

SYSTEM_TaskStatus_t DEMO_ProcessTask(void);
void DEMO_SendMessage(void);
void DEMO_MessageNotification(void *appHnd, appCbParams_t *data);



#endif /* _EXAMPLE_FILE_NAME_H */
