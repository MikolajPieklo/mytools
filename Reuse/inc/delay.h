/**
 ********************************************************************************
 * @file    Delay.h
 * @author  Mikolaj Pieklo
 * @date    12.11.2024
 * @brief
 ********************************************************************************
 */

#ifndef __DELAY_H__
#define __DELAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include "stdint.h"
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
void     TS_Delay_ms(uint32_t delay_ms);
void     TS_Delay_us_Init(void);
void     TS_Delay_us(uint32_t delay_us);
uint32_t TS_Get_ms(void);

#ifdef __cplusplus
}
#endif

#endif