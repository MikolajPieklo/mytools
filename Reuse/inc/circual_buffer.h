/**
 ********************************************************************************
 * @file    circual_buffer.h
 * @author  Mikolaj Pieklo
 * @date    15.12.2023
 * @brief
 ********************************************************************************
 */

#ifndef __CIRCUAL_BUFFER_H__
#define __CIRCUAL_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include <stdint.h>

#include <stm32f1xx_ll_usart.h>

/************************************
 * MACROS AND DEFINES
 ************************************/
#define CIRCUAL_BUFFER_SIZE 1024

/************************************
 * TYPEDEFS
 ************************************/
typedef struct CirBuff_Typedef
{
   uint32_t       tail;
   uint32_t       head;
   uint8_t        data[CIRCUAL_BUFFER_SIZE];
   uint32_t       size;
   USART_TypeDef *USARTx;
} CirBuff_T;

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void CirBuff_Insert_Char(CirBuff_T *cb, uint8_t c);

void CirBuff_Insert_Text(CirBuff_T *cb, uint8_t *text, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif