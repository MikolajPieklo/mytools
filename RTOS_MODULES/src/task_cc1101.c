/**
 ********************************************************************************
 * @file    task_cc1101.c
 * @author  Mikolaj Pieklo
 * @date    08.02.2026
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "task_cc1101.h"

#include "task_supervisor.h"
#include <FreeRTOS.h>
#include <cc1101.h>
#include <log.h>
#include <task.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device task_cc1101_dev = {
   .name = "TASK_CC1101",
};

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static void task_cc1101(void *parameters) __attribute__((noreturn));

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void task_cc1101(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;

   EventGroupHandle_t wdEvent = Get_WD_Event_Handle();

   // CC1101_Init(0);
   CC1101_Debug_Init();
   int8_t temperature = 0;

   for (;;)
   {
      if (NULL != wdEvent)
      {
         xEventGroupSetBits(wdEvent, WD_TASK_CC1101);
      }
      else
      {
         wdEvent = Get_WD_Event_Handle();
         log_err(&task_cc1101_dev, "WD Event Group is NULL\r\n");
      }

      /* Example Task Code */
      // CC1101_Check_State();
      CC1101_Debug_Rx();

      // LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      vTaskDelay(pdMS_TO_TICKS(1000)); /* delay 1000 ticks */
   }
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void Task_CC1101_Create(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;

   /* Create the task, storing the handle. */
   xTaskCreate(task_cc1101, "cc1101", configMINIMAL_STACK_SIZE, (void *) NULL, tskIDLE_PRIORITY,
               NULL);
}
