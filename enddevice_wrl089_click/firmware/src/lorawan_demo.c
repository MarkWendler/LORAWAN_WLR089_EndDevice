
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "mls.h"
#include "mls_app.h"
#include "lorawan_demo.h"
#include "utilities.h"

#ifdef CRYPTO_DEV_ENABLED
#include "cryptoauthlib.h"
#endif /* #ifdef CRYPTO_DEV_ENABLED */


// *****************************************************************************
// *****************************************************************************
// Section: External variables
// *****************************************************************************
// *****************************************************************************
extern bool mlsAppSerRxStartFlag;
extern uint8_t mlsAppSerialBuffer;
extern bool mlsAppPeriodicStartFlag;
extern uint8_t mlsAppPeriodicTimer;

extern IsmBand_t mlsAppISMBand;

#ifdef CONF_PMM_ENABLE
extern uint32_t ulSleptDuration;
#endif

const char * pcLorawanStatus[] = {
	"RADIO_SUCCESS",
	"RADIO_NO_DATA",
	"RADIO_DATA_SIZE",
	"RADIO_INVALID_REQ",
	"RADIO_BUSY",
	"RADIO_OUT_OF_RANGE",
	"RADIO_UNSUPPORTED_ATTR",
	"RADIO_CHANNEL_BUSY",
	"SUCCESS",
	"NWK_NOT_JOINED",
	"INVALID_PARAMETER",
	"KEYS_NOT_INITIALIZED",
	"SILENT_IMMEDIATELY_ACTIVE",
	"FCNTR_ERROR_REJOIN_NEEDED",
	"INVALID_BUFFER_LENGTH",
	"MAC_PAUSED",
	"NO_CHANNELS_FOUND",
	"BUSY",
	"NO_ACK",
	"NWK_JOIN_IN_PROGRESS",
	"RESOURCE_UNAVAILABLE",
	"INVALID_REQUEST",
	"UNSUPPORTED_BAND",
	"FCNTR_ERROR",
	"MIC_ERROR",
	"INVALID_MTYPE",
	"MCAST_HDR_INVALID",
	"TX_TIMEOUT",
	"RADIO_TX_TIMEOUT",
	"MAX_MCAST_GROUP_REACHED",
	"INVALID_PACKET",
	"RXPKT_ENCRYPTION_FAILED",
	"TXPKT_ENCRYPTION_FAILED",
	"SKEY_DERIVATION_FAILED",
	"MIC_CALCULATION_FAILED",
	"SKEY_READ_FAILED",
	"JOIN_NONCE_ERROR"
};


// *****************************************************************************
// *****************************************************************************
// Section: Global variables
// *****************************************************************************
// *****************************************************************************
volatile uint8_t ucAppTaskFlags = 0x00u;
volatile AppTaskState_t xAppTaskState = APP_INIT_STATE;




#if (ENABLE_PDS == 1)
uint8_t ucRestoreTimer;
uint8_t ucRestorePromptStopKey;
volatile uint8_t ucRestoreAttemptCnt = DEMOAPP_RESTOREATTEMPT_MAX;
#endif /* #if (ENABLE_PDS == 1) */

// *****************************************************************************
// *****************************************************************************
// Section: Function definitions
// *****************************************************************************
// *****************************************************************************
SYSTEM_TaskStatus_t APP_TaskHandler(void)
{
    SYSTEM_PostTask(APP_TASK_ID);
	/*if (ucAppTaskFlags) {
		for (uint8_t ucTaskId = 0; ucTaskId < APP_TASKS_COUNT; ucTaskId++) {
			if ((1 << ucTaskId) & (ucAppTaskFlags)) {
				ATOMIC_SECTION_ENTER
				ucAppTaskFlags &= ~(1 << ucTaskId);
				ATOMIC_SECTION_EXIT

				pxAppTaskHandlers[ucTaskId]();

				if (ucAppTaskFlags) {
					SYSTEM_PostTask(APP_TASK_ID);
				}

				break;
			}
		}
	}*/

	return SYSTEM_TASK_SUCCESS;
}
//------------------------------------------------------------------------------
void DEMO_RunTask(void)
{
	SYSTEM_PostTask(APP_TASK_ID);
}

//------------------------------------------------------------------------------

void DEMO_PrintStatus(StackRetStatus_t xStat)
{
	printf("Status : LORAWAN_%s\r\n", pcLorawanStatus[xStat]);
}

//------------------------------------------------------------------------------

void DEMO_Init(void) {
    LED_GREEN_OFF;
    LED_BLUE_OFF;
    LED_RED_OFF;

#ifdef CONF_PMM_ENABLE
    MlsAppSleepCallbackNotifySet(DEMO_WakeupNotification);
#endif
    MlsAppSendReqCallbackNotifySet(DEMO_MessageNotification);
    MlsAppJoinReqCallbackNotifySet(DEMO_JoinNotification);

    xAppTaskState = APP_INIT_STATE;

    printf("\r\nInit - Successful\r\n");

#ifdef CRYPTO_DEV_ENABLED
    DEMO_PrintECCInfo();
#endif /* #ifdef CRYPTO_DEV_ENABLED */

#if (ENABLE_PDS == 1)
    if (PDS_IsRestorable()) {
        PDS_RestoreAll();

        ucRestoreAttemptCnt = DEMOAPP_RESTOREATTEMPT_MAX;

        printf("Last configured regional band \r\n");

        if (SwTimerCreate(&ucRestoreTimer) != LORAWAN_SUCCESS) { //todo
            printf("Failed to create SW timer for Auto restore\r\n");
        }
    } else
#endif
    {
        StackRetStatus_t xSetStatus;

        LORAWAN_Reset(APP_ISMBAND);

        xSetStatus = MlsAppSet(APP_ISMBAND, mlsAppSerialBuffer);

        if ((xSetStatus == LORAWAN_SUCCESS)) {
            printf("\nJoin Request Sent");
        } else {
            DEMO_PrintStatus(xSetStatus);
        }

        xAppTaskState = APP_JOIN_STATE;
    }
    
    DEMO_RunTask();

}

//------------------------------------------------------------------------------


SYSTEM_TaskStatus_t DEMO_ProcessTask(void)
{
	printf("\r\n\r\n");

	switch (xAppTaskState) {
	case APP_INIT_STATE:
	{
		DEMO_Init();

		break;
	}

    case APP_JOIN_STATE:
	{
		MlsAppJoinReq(DEMO_APP_ACTIVATION_TYPE);
		break;
	}
    
    case APP_SEND_STATE:
	{
		DEMO_SendMessage();
		break;
	}   
    
	case APP_IDLE_STATE:
	{
		//DEMO_ProcessAppMenu();
		break;
	}
#if (ENABLE_PDS == 1)
	case RESTORE_PROMPT_STATE:
	{
		DEMO_ProcessRestorePrompt();

		break;
	}
#endif
	default:
	{
		printf("Warn - %s:%d - Invalid app_task_state\r\n",
			__FUNCTION__, __LINE__);

		break;
	}
	}

	return LORAWAN_SUCCESS;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void DEMO_SendMessage(void)
{
	StackRetStatus_t xStat;

	xStat = MlsAppSendReq();
	//DEMO_PrintStatus(xStat);

	switch (xStat) {
	case LORAWAN_SUCCESS:
        LED_RED_ON;
		break;

	case LORAWAN_NWK_NOT_JOINED:
		printf("Warn - Cannot uplink, device not joined\r\n");
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------------

void DEMO_MessageNotification(void *appHnd, appCbParams_t *data)
{
	(void) appHnd;

	EdClass_t xClass;

	LORAWAN_GetAttr(EDCLASS, NULL, &xClass);

    LED_RED_OFF;


	switch (data->evt) {
	default:
		break;

	case LORAWAN_EVT_TRANSACTION_COMPLETE:
	{
		DEMO_PrintStatus(data->param.transCmpl.status);
		break;
	}

	case LORAWAN_EVT_RX_DATA_AVAILABLE:
	{
		printf("RX packet arrived");
		break;
	}
	}
}
//------------------------------------------------------------------------------

