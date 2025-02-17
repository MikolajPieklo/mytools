/**
 ********************************************************************************
 * @file    circual_buffer.c
 * @author  Mikolaj Pieklo
 * @date    15.12.2023
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "circual_buffer.h"

#include <stdint.h>

#include <string.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

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

/************************************
 * STATIC FUNCTIONS
 ************************************/

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void CirBuff_Insert_Char(CirBuff_T *cb, uint8_t c)
{
   if (cb->head >= (CIRCUAL_BUFFER_SIZE - 1))
   {
      cb->head = 0;
   }
   cb->data[cb->head] = c;
   cb->head++;

   if (USART1 == cb->USARTx)
   {
      LL_USART_EnableIT_TC(cb->USARTx);
   }
}

void CirBuff_Insert_Text(CirBuff_T *cb, uint8_t *text, uint32_t len)
{
   if ((cb->head + len) <= (CIRCUAL_BUFFER_SIZE - 1))
   {
      memcpy(cb->data + cb->head, text, len);
      cb->head += len;
   }
   else if (cb->head + len > (CIRCUAL_BUFFER_SIZE - 1))
   {
      uint32_t lenToEndBuff = CIRCUAL_BUFFER_SIZE - cb->head;
      memcpy(cb->data + cb->head, text, lenToEndBuff);
      memcpy(cb->data, (text + lenToEndBuff), (len - lenToEndBuff));
      cb->head = (len - lenToEndBuff);
   }
   else
   {
      // do nothing
   }

   if (USART1 == cb->USARTx)
   {
      LL_USART_EnableIT_TC(cb->USARTx);
   }
}