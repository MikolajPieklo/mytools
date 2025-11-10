/**
 ********************************************************************************
 * @file    lcd12864.h
 * @author  Mikolaj Pieklo
 * @date    20.11.2024
 * @brief
 ********************************************************************************
 */

#ifndef __LCD12864_H__
#define __LCD12864_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include <stdbool.h>
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
void LCD12864_Init(void);

void LCD12864_Set_Char(void);

void LCD12864_Graphic_Mode(bool enable);

void LCD12864_Fill_Screen(void);

void LCD12864_Clear_Screen(void);

int32_t LCD12864_Show_Value(uint8_t x_grid, uint8_t y_grid, int32_t value);

#ifdef __cplusplus
}
#endif

#endif