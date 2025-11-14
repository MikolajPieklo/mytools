/**
 ********************************************************************************
 * @file    uart.h
 * @author  Mikolaj Pieklo
 * @date    10.11.2025
 * @brief
 ********************************************************************************
 */

#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#ifdef STM32F103xB
#include <stm32f1xx_ll_usart.h>
#elif STM32F401xC
#include <stm32f4xx_ll_usart.h>
#else
#error Module not supported!
#endif

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

int8_t UART1_Init(void);

int8_t USART2_Init(void);

int8_t USARTx_Set_BaudRate(USART_TypeDef *USARTx, uint32_t baudRate);

int8_t USARTx_Tx(USART_TypeDef *USARTx, uint8_t *data, uint8_t lenght);

int8_t USARTx_Rx(USART_TypeDef *USARTx, uint8_t *data, uint8_t lenght);


#ifdef __cplusplus
}
#endif

#endif