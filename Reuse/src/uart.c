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

#ifdef STM32F103xB
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_rcc.h>
#elif STM32F401xC
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_rcc.h>
#else
#error Module not supported!
#endif

#include <circual_buffer.h>
#include <reuse.h>

/************************************
 * EXTERN VARIABLES
 ************************************/
extern volatile CirBuff_T cb_uart1_tx;
extern volatile CirBuff_T cb_uart1_rx;

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define USART1_RX_PIN LL_GPIO_PIN_10
#define USART1_TX_PIN LL_GPIO_PIN_9

#define USART2_TX_RX_PIN LL_GPIO_PIN_2

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
int8_t UART1_Init(void)
{
   LL_USART_InitTypeDef USART_InitStruct = {0};
   LL_GPIO_InitTypeDef  GPIO_InitStruct = {0};

   /* Peripheral clock enable */
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
#elif STM32F401xC
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
#endif

   /*USART1 GPIO Configuration */
   GPIO_InitStruct.Pin = USART1_TX_PIN;
   GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
   GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
   GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
   GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
#ifdef STM32F401xC
   GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
#endif
   LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   GPIO_InitStruct.Pin = USART1_RX_PIN;
   GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
   GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
   GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
   GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
#ifdef STM32F401xC
   GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
#endif
   LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   /* USART1 interrupt Init */
   NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
   NVIC_EnableIRQ(USART1_IRQn);

   LL_USART_Disable(USART1);
   USART_InitStruct.BaudRate = 115200;
   USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
   USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
   USART_InitStruct.Parity = LL_USART_PARITY_NONE;
   USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
   USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
   USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
   LL_USART_Init(USART1, &USART_InitStruct);
   LL_USART_ConfigAsyncMode(USART1);
   LL_USART_Enable(USART1);

   // LL_USART_EnableIT_TXE(USART1);
   LL_USART_EnableIT_RXNE(USART1);
   LL_USART_EnableIT_ERROR(USART1);
   // LL_USART_EnableIT_TC(USART1);

   return 0;
}

int8_t USART2_Init(void)
{
   LL_USART_InitTypeDef USART_InitStruct = {0};
   LL_GPIO_InitTypeDef  GPIO_InitStruct = {0};

   /* Peripheral clock enable */
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
#elif STM32F401xC
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
#endif

   GPIO_InitStruct.Pin = USART2_TX_RX_PIN;
   GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
   GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
   GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
   GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;

   LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   LL_USART_Disable(USART2);
   USART_InitStruct.BaudRate = 115200;
   USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
   USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
   USART_InitStruct.Parity = LL_USART_PARITY_NONE;
   USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
   USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
   USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
   LL_USART_Init(USART2, &USART_InitStruct);
   LL_USART_ConfigAsyncMode(USART2);
   LL_USART_EnableHalfDuplex(USART2);
   LL_USART_Enable(USART2);

   return 0;
}

int8_t USARTx_Set_BaudRate(USART_TypeDef *USARTx, uint32_t baudRate)
{
   LL_RCC_ClocksTypeDef all_clk;
   uint32_t             clock = 0U;

   LL_RCC_GetSystemClocksFreq(&all_clk);
   if (USARTx == USART1)
   {
      clock = all_clk.PCLK1_Frequency;
   }
   else if (USARTx == USART2
            || USARTx ==
#ifdef STM32F103xB
                USART3)
#elif STM32F401xC
                USART6)
#endif
   {
      clock = all_clk.PCLK2_Frequency;
   }

#ifdef STM32F103xB
   LL_USART_SetBaudRate(USARTx, clock, baudRate);
#elif STM32F401xC
   LL_USART_SetBaudRate(USARTx, clock, LL_USART_OVERSAMPLING_16, baudRate);
#endif

   return 0;
}

int8_t USARTx_Tx(USART_TypeDef *USARTx, uint8_t *data, uint8_t lenght)
{
   uint32_t i = 0U;
   for (i = 0U; i < lenght; i++)
   {
      LL_USART_TransmitData8(USARTx, data[i]);
      while (!LL_USART_IsActiveFlag_TC(USARTx))
      {
      }
   }

   return 0;
}

int8_t USARTx_Rx(USART_TypeDef *USARTx, uint8_t *data, uint8_t lenght)
{
   uint32_t i = 0U;
   for (i = 0; i < lenght; i++)
   {
      while (!LL_USART_IsActiveFlag_RXNE(USARTx))
      {
      }
      data[i] = LL_USART_ReceiveData8(USARTx);
   }
   return (int8_t) LL_USART_IsActiveFlag_NE(USARTx);
}

void USART1_IRQHandler(void)
{
   if (LL_USART_IsEnabledIT_RXNE(USART1) && LL_USART_IsActiveFlag_RXNE(USART1))
   {
      if (cb_uart1_rx.head == cb_uart1_rx.size)
      {
         cb_uart1_rx.head = 0;
      }
      cb_uart1_rx.data[cb_uart1_rx.head] = LL_USART_ReceiveData8(USART1);
      cb_uart1_rx.head++;
   }

   if (LL_USART_IsEnabledIT_TXE(USART1) && LL_USART_IsActiveFlag_TXE(USART1))
   {
      /* TXE flag will be automatically cleared when writing new data in DR register */
   }

   if (LL_USART_IsEnabledIT_TC(USART1) && LL_USART_IsActiveFlag_TC(USART1))
   {
      LL_USART_ClearFlag_TC(USART1);
      if (cb_uart1_tx.tail != cb_uart1_tx.head)
      {
         LL_USART_TransmitData8(USART1, cb_uart1_tx.data[cb_uart1_tx.tail]);
         cb_uart1_tx.tail++;
      }

      if (cb_uart1_tx.tail == CIRCUAL_BUFFER_SIZE)
      {
         cb_uart1_tx.tail = 0;
      }

      if (cb_uart1_tx.tail == cb_uart1_tx.head)
      {
         LL_USART_DisableIT_TC(USART1);
      }
   }
   if (LL_USART_IsEnabledIT_ERROR(USART1) && LL_USART_IsActiveFlag_NE(USART1))
   {
      /* Call Error function */
   }
}