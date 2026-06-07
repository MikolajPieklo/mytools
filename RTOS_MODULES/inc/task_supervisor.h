/**
 ********************************************************************************
 * @file    task_supervisor.h
 * @author  Mikolaj Pieklo
 * @date    08.02.2026
 * @brief
 ********************************************************************************
 */

#ifndef __TASK_SUPERVISOR_H__
#define __TASK_SUPERVISOR_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include <FreeRTOS.h>
#include <event_groups.h>

/************************************
 * MACROS AND DEFINES
 ************************************/
#define WD_TASK_CC1101 (1 << 0)
#define WD_TASK_SCD41  (1 << 1)
#define WD_TASK_CTRL   (1 << 2)

#define WD_ALL_TASKS (WD_TASK_CC1101 | WD_TASK_SCD41 | WD_TASK_CTRL)
/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void Task_Supervisor_Create(void *parameters);

EventGroupHandle_t Get_WD_Event_Handle(void);

#ifdef __cplusplus
}
#endif

#endif