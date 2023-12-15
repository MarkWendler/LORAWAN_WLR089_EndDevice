/* **************************************************************************
 User interface application

 @Summary
 Handling buttons and LEDs on the board


*******   Section: Included Files   *********/
#include "config/default/peripheral/port/plib_port.h"
#include "config/default/MLS/services/sw_timer/sw_timer.h"


#define LED0_PIN      PORT_PIN_PA19
#define LED1_PIN      PORT_PIN_PA18

#define LED0_ON()     PORT_PinSet(LED0_PIN)
#define LED0_OFF()    PORT_PinClear(LED0_PIN)
#define LED0_TOGGLE() PORT_PinToggle(LED0_PIN)

#define LED1_ON()     PORT_PinSet(LED1_PIN)
#define LED1_OFF()    PORT_PinClear(LED1_PIN)
#define LED1_TOGGLE() PORT_PinToggle(LED1_PIN)

#define SW0_PIN PORT_PIN_PA28

#define SW0_STATE() PORT_PinRead(SW0_PIN)

typedef enum
{
    /* Application's state machine's initial state. */
    USERIF_STATE_INIT=0,
    USERIF_SERVICE_TASKS,

} USERIF_STATES;

USERIF_STATES userIfState = USERIF_STATE_INIT;
uint8_t ucMyTimerID;

/********   Section: Local Functions  *********/


/********   Section: Interface Functions  *********/

void APP_UserIf_PostTask(void){
    SYSTEM_PostTask(APP_USER_IF_ID);
    
        if (SwTimerStart(ucMyTimerID,
                MS_TO_US(1000),
                SW_TIMEOUT_RELATIVE,
                APP_UserIf_PostTask,
                NULL) != LORAWAN_SUCCESS) {
                printf("Failed to restart periodic timer for APP_User_If\r\n");
        }    
}


/* *****************************************************************************
 Summary:
 Initializes the user interface application.

 Description:
 This function initializes the user interface application and all its variables
 and modules.

 */
void APP_UserIfInit(void){
    // Initialize LED0 and LED1 PA19 and PA18
    PORT_PinGPIOConfig(LED0_PIN);
    PORT_PinGPIOConfig(LED1_PIN);
    PORT_PinOutputEnable(LED0_PIN);
    PORT_PinOutputEnable(LED1_PIN);

    // Initialize SW0 PA28
    PORT_PinGPIOConfig(SW0_PIN);
    PORT_PinInputEnable(SW0_PIN);
    PORT_PinWrite(SW0_PIN, true);//Enable pull-up
    //SW0 - input enable & pull enable
	PORT_REGS->GROUP[0].PORT_PINCFG[28]= 0b0111;
    
    if( SwTimerCreate(&ucMyTimerID)!= LORAWAN_SUCCESS) {
			printf("Failed to create SW timer for APP_User_If\r\n");
	}
    else{
    
        if (SwTimerStart(ucMyTimerID,
                MS_TO_US(1000),
                SW_TIMEOUT_RELATIVE,
                APP_UserIf_PostTask,
                NULL) != LORAWAN_SUCCESS) {
                printf("Failed to restart periodic timer for APP_User_If\r\n");
        }
    }

}

/* *****************************************************************************
 Summary:
 Tasks the user interface application. Handles button presses and LED toggles.

 Description:
 This has a state machine that handles the button presses and LED toggles.

 */
SYSTEM_TaskStatus_t APP_UserIfTask(void)
{
    switch(userIfState)
    {
        case APP_STATE_INIT:
        {
            APP_UserIfInit();
            userIfState = USERIF_SERVICE_TASKS;
            printf("APP_USER_IF INITIALIZED\r\n");
            break;
        }

        case USERIF_SERVICE_TASKS:
        {
            
            if(SW0_STATE() == 0)
            {
                LED0_ON();
                printf("Button ON\r\n");
            }
            else
            {
                LED0_OFF();
            }
            LED1_TOGGLE();
            break;
        }

        default:
        {
          //userIfState = USERIF_STATE_INIT;
            break;
        }
    }
    
    return SYSTEM_TASK_SUCCESS;

}


/* *****************************************************************************
 End of File
 */
