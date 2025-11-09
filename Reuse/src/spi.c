/**
 ********************************************************************************
 * @file    spi.c
 * @author  Mikolaj Pieklo
 * @date    29.11.2024
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "spi.h"

#include <stddef.h>

#include <stm32f1xx_ll_bus.h>

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
void SPI1_Init(void)
{
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

   LL_GPIO_SetPinMode(GPIOA, SPI1_CS1_Pin, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_CS1_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOA, SPI1_CS1_Pin, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(GPIOA, SPI1_CS1_Pin, LL_GPIO_PULL_UP);

   LL_GPIO_SetPinMode(GPIOA, SPI1_CS2_Pin, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_CS2_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOA, SPI1_CS2_Pin, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(GPIOA, SPI1_CS2_Pin, LL_GPIO_PULL_UP);

   LL_GPIO_SetOutputPin(GPIOA, SPI1_CS1_Pin);
   LL_GPIO_SetOutputPin(GPIOA, SPI1_CS2_Pin);

   LL_GPIO_SetPinMode(GPIOA, SPI1_SCK_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_SCK_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOA, SPI1_SCK_Pin, LL_GPIO_PULL_UP);

   LL_GPIO_SetPinMode(GPIOA, SPI1_MISO_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_MISO_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOA, SPI1_MISO_Pin, LL_GPIO_PULL_UP);

   LL_GPIO_SetPinMode(GPIOA, SPI1_MOSI_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_MOSI_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOA, SPI1_MOSI_Pin, LL_GPIO_PULL_UP);

   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

   // NVIC_SetPriority(SPI1_IRQn, 0);    /* Set priority for SPI1_IRQn */
   // NVIC_EnableIRQ(SPI1_IRQn);         /* Enable SPI1_IRQn           */

   /* Configure SPI1 communication */
   LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV64);
   LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
   LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
   LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
   /* Reset value is LL_SPI_MSB_FIRST */
   LL_SPI_SetTransferBitOrder(SPI1, LL_SPI_MSB_FIRST);
   LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
   LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT);
   LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);

   // LL_SPI_EnableIT_RXNE(SPI1);        /* Enable RXNE  Interrupt      */
   // LL_SPI_EnableIT_TXE(SPI1);         /* Enable TXE   Interrupt      */
   // LL_SPI_EnableIT_ERR(SPI1);         /* Enable Error Interrupt      */

   LL_SPI_Enable(SPI1);
}

void SPI2_Init(void)
{
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

   LL_GPIO_SetPinMode(GPIOB, SPI2_SCK_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOB, SPI2_SCK_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOB, SPI2_SCK_Pin, LL_GPIO_PULL_UP);

   LL_GPIO_SetPinMode(GPIOB, SPI2_MOSI_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOB, SPI2_MOSI_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOB, SPI2_MOSI_Pin, LL_GPIO_PULL_UP);

   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

   /* Configure SPI2 communication */
   LL_SPI_SetBaudRatePrescaler(SPI2, LL_SPI_BAUDRATEPRESCALER_DIV16);
   LL_SPI_SetTransferDirection(SPI2, LL_SPI_FULL_DUPLEX);
   LL_SPI_SetClockPhase(SPI2, LL_SPI_PHASE_2EDGE);
   LL_SPI_SetClockPolarity(SPI2, LL_SPI_POLARITY_LOW);
   /* Reset value is LL_SPI_MSB_FIRST */
   LL_SPI_SetTransferBitOrder(SPI2, LL_SPI_MSB_FIRST);
   LL_SPI_SetDataWidth(SPI2, LL_SPI_DATAWIDTH_8BIT);
   LL_SPI_SetNSSMode(SPI2, LL_SPI_NSS_SOFT);
   LL_SPI_SetMode(SPI2, LL_SPI_MODE_MASTER);

   LL_SPI_Enable(SPI2);
}

uint8_t SPI_Transfer(SPI_TypeDef *dev, uint32_t cs_pin, uint8_t *tx_data, uint8_t *rx_data,
                     uint8_t n)
{
   uint8_t i = 0;

   if (0 != cs_pin)
   {
      LL_GPIO_ResetOutputPin(GPIOA, cs_pin);
   }

   for (i = 0; i < n; i++)
   {
      LL_SPI_TransmitData8(dev, *(tx_data + i));
      while (LL_SPI_IsActiveFlag_BSY(dev))
      {
      }
      if (NULL != rx_data)
      {
         *(rx_data + i) = LL_SPI_ReceiveData8(dev);
      }
   }

   if (0 != cs_pin)
   {
      LL_GPIO_SetOutputPin(GPIOA, cs_pin);
   }

   return 0;
}
