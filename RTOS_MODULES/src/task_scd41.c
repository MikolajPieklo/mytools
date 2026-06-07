/**
 ********************************************************************************
 * @file    task_sdc41.c
 * @author  Mikolaj Pieklo
 * @date    11.02.2026
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "task_scd41.h"

#include "task_supervisor.h"
#include <FreeRTOS.h>
#include <log.h>
#include <scd41.h>
#include <task.h>

/************************************
 * EXTERN VARIABLES
 ************************************/
/* Dummy device */
static const struct device task_scd41_dev = {
   .name = "TASK_SCD41",
};

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/

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
static void task_scd41(void *parameters) __attribute__((noreturn));

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void task_scd41(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;

   EventGroupHandle_t wdEvent = Get_WD_Event_Handle();

   vTaskDelay(pdMS_TO_TICKS(1000)); /* delay 1000 ms */
   SCD41_Init(I2C1);
   // SCD41_PerformFactoryResetBlocking(I2C1);
   //  SCD41_PerformForcedRecalibration(I2C1, 400);

   for (;;)
   {
      if (NULL != wdEvent)
      {
         xEventGroupSetBits(wdEvent, WD_TASK_SCD41);
      }
      else
      {
         wdEvent = Get_WD_Event_Handle();
         log_err(&task_scd41_dev, "WD Event Group is NULL\r\n");
      }

      uint16_t rh = 0;
      uint16_t co2 = 0;
      int16_t  temperature = 0;
      int8_t   retval = SCD41_GetCO2RHTBlocking(I2C1, &co2, &rh, &temperature);
      if (0 != retval)
      {
         log_err(&task_scd41_dev, "SCD41 Get CO2RHT Blocking error Status %d\r\n", retval);
      }
      else
      {
         log_info(&task_scd41_dev, "SCD41 CO2: %d, RH: %d, Temperature: %d C\r\n", co2, rh,
                  temperature);
      }

      vTaskDelay(pdMS_TO_TICKS(1000)); /* delay 1000 ms */
   }
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void Task_SCD41_Create(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;

   /* Create the task, storing the handle. */
   xTaskCreate(task_scd41, "scd41", configMINIMAL_STACK_SIZE, (void *) NULL, tskIDLE_PRIORITY,
               NULL);
}