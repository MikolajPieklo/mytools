/**
 ********************************************************************************
 * @file    tm1637.h
 * @author  Mikolaj Pieklo
 * @date    11.10.2025
 * @brief
 ********************************************************************************
 */

#ifndef __TM1637_H__
#define __TM1337_H__

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
void TM1637Init(void);
void TM1637DisplayDecimal(int v, int displaySeparator);
void TM1637SetBrightness(uint8_t brightness);
void TM1637ShowError(void);
void TM1637ShowStartMessage(void);

#ifdef __cplusplus
}
#endif

#endif