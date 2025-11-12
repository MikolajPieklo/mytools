/**
 ********************************************************************************
 * @file    uart.c
 * @author  Mikolaj Pieklo
 * @date    12.11.2025
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "uart.h"

#include <reuse.h>

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
int8_t USARTx_Set_BaudRate(USART_TypeDef *USARTx, uint32_t baudRate)
{
   UNUSED(USARTx);
   UNUSED(baudRate);
   return 0;
}

int8_t USARTx_Tx(USART_TypeDef *USARTx, uint8_t *data, uint8_t lenght)
{
   UNUSED(USARTx);
   UNUSED(data);
   UNUSED(lenght);
   return 0;
}

int8_t USARTx_Rx(USART_TypeDef *USARTx, uint8_t *data, uint8_t lenght)
{
   UNUSED(USARTx);
   UNUSED(data);
   UNUSED(lenght);
   return 0;
}

int8_t USART2_Init(void)
{
   return 0;
}
