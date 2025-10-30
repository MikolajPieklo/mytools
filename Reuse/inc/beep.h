/**
 ********************************************************************************
 * @file    beep.h
 * @author  Mikolaj Pieklo
 * @date    26.10.2025
 * @brief
 ********************************************************************************
 */

#ifndef __NAME_H__
#define __NAME_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include <stdint.h>
/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void     Beep_Init(void);
void     Beep_Pulse(uint8_t ms);
void     Beep_Set_Warning(void);
void     Beep_Clear_Warning(void);
uint16_t Beep_Get_Warning_Status(void);
void     Beep_Task(void);


#ifdef __cplusplus
}
#endif

#endif