/**
 ********************************************************************************
 * @file    task_supervisor.c
 * @author  Mikolaj Pieklo
 * @date    08.02.2026
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "task_supervisor.h"

#include <FreeRTOS.h>
#include <log.h>
#include <task.h>
#ifdef STM32F103xB
#include <stm32f1xx_ll_iwdg.h>
#elif STM32F401xC
#include <stm32f4xx_ll_iwdg.h>
#endif

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device task_supervisor_dev = {
   .name = "TASK_SUPERVISOR",
};

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/
static EventGroupHandle_t wdEvent;

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static void task_supervisor(void *parameters) __attribute__((noreturn));

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void task_supervisor(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;

   wdEvent = xEventGroupCreate();

   LL_IWDG_EnableWriteAccess(IWDG);
   /* T_WDG = (Reload + 1) × Prescaler / f_LSI*/
   LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_64);
   LL_IWDG_SetReloadCounter(IWDG, 1250); /* 1250 * 64 / 40000 = 2s timeout */
   LL_IWDG_DisableWriteAccess(IWDG);
   LL_IWDG_Enable(IWDG);

   while (1)
   {
      EventBits_t bits = xEventGroupWaitBits(wdEvent, WD_TASK_CC1101, // na co czekamy
                                             pdTRUE,                  // clear bits on exit
                                             pdTRUE,                  // wait for ALL bits
                                             pdMS_TO_TICKS(1000));

      if ((bits & WD_TASK_CC1101) == WD_TASK_CC1101)
      {
         LL_IWDG_ReloadCounter(IWDG);
      }
      LL_IWDG_ReloadCounter(IWDG);
   }
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void Task_Supervisor_Create(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;
   /* Create the task, storing the handle. */
   xTaskCreate(task_supervisor, "supervisor", configMINIMAL_STACK_SIZE, (void *) NULL,
               tskIDLE_PRIORITY, NULL);
}

EventGroupHandle_t Get_WD_Event_Handle(void)
{
   return wdEvent;
}
