/**
 ********************************************************************************
 * @file    task_monitor.c
 * @author  Mikolaj Pieklo
 * @date    28.01.2026
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "task_monitor.h"

#include <FreeRTOS.h>
#include <log.h>
#include <string.h>
#include <task.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device monitor_task = {
   .name = "MONITOR",
};

#define MAX_TASKS 10

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef struct TaskPrevTime_Typedef
{
   uint32_t id;
   uint32_t prevTaskTime;
} TaskPrevTime_t;

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static void    monitorTask(void *parameters) __attribute__((noreturn));
static uint8_t find_index(uint32_t task_id, TaskPrevTime_t *table);

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void monitorTask(void *parameters)
{
   /* Unused parameters. */
   (void) parameters;
   TaskStatus_t         *pxTaskStatusArray;
   volatile UBaseType_t  uxArraySize, x;
   uint32_t              total_run_time = 0;
   static TaskPrevTime_t prevTaskTime[MAX_TASKS];
   static uint32_t       prevTotalTime = 0;

   memset((void *) prevTaskTime, 0, sizeof(prevTaskTime));

   for (;;)
   {
      vTaskDelay(40000); /* delay 40s */

      uxArraySize = uxTaskGetNumberOfTasks();
      if (uxArraySize == MAX_TASKS)
      {
         log_err(&monitor_task, "Task Monitor: MAX_TASKS limit reached!\r\n");
      }

      pxTaskStatusArray = pvPortMalloc(uxArraySize * 2 * sizeof(TaskStatus_t));
      if (NULL != pxTaskStatusArray)
      {
         uint32_t uxArraySize =
             uxTaskGetSystemState(pxTaskStatusArray, uxArraySize * 2, &total_run_time);

         log_notice(&monitor_task,
                    "   Task Name   | ID | CPU%% |     Ticks    | Min Free words \r\n");
         log_notice(&monitor_task, "-------------------------------------------------------\r\n");

         uint32_t deltaTotal = total_run_time - prevTotalTime;
         for (x = 0; x < uxArraySize; x++)
         {
            uint8_t  index = find_index(pxTaskStatusArray[x].xTaskNumber, prevTaskTime);
            uint32_t deltaTask =
                pxTaskStatusArray[x].ulRunTimeCounter - prevTaskTime[index].prevTaskTime;
            uint32_t cpu_percent = (deltaTask * 100UL) / deltaTotal;

            log_notice(&monitor_task, "%-15s|%3u | %3u%% | %12lu | %8u\r\n",
                       pxTaskStatusArray[x].pcTaskName, pxTaskStatusArray[x].xTaskNumber,
                       cpu_percent, pxTaskStatusArray[x].ulRunTimeCounter,
                       pxTaskStatusArray[x].usStackHighWaterMark);

            prevTaskTime[index].prevTaskTime = pxTaskStatusArray[x].ulRunTimeCounter;
         }
         prevTotalTime = total_run_time;
         vPortFree(pxTaskStatusArray);
      }
   }
}

static uint8_t find_index(uint32_t task_id, TaskPrevTime_t *table)
{
   uint8_t i = 0;
   for (i = 0; i < MAX_TASKS; i++)
   {
      if (table[i].id == 0)
      {
         table[i].id = task_id;
         break;
      }
      if (table[i].id == task_id)
      {
         break;
      }
   }
   return i;
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
int32_t StartTaskMonitor(void)
{
   return xTaskCreate(monitorTask, "monitor", configMINIMAL_STACK_SIZE, (void *) NULL,
                      tskIDLE_PRIORITY, NULL);
}
