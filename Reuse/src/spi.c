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

#include <delay.h>
#include <errno.h>
#include <log.h>

#ifdef STM32F103xB
#include <stm32f1xx_ll_bus.h>
#elif STM32F401xC
#include <stm32f4xx_ll_bus.h>
#else
#error Module not supported!
#endif

/************************************
 * EXTERN VARIABLES
 ************************************/
/* Dummy device */
static const struct device spi_dev = {
   .name = "SPI",
};
/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define SPI1_SCK_Pin  LL_GPIO_PIN_5
#define SPI1_MISO_Pin LL_GPIO_PIN_6
#define SPI1_MOSI_Pin LL_GPIO_PIN_7
#define SPI2_SCK_Pin  LL_GPIO_PIN_13
#define SPI2_MOSI_Pin LL_GPIO_PIN_15

#define SPI_TIMEOUT_MS             10U
#define SPI_TIMEOUT_EXPIRED(start) ((TS_GET_MS() - (start)) > SPI_TIMEOUT_MS)

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
#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
#elif STM32F401xC
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
#endif

   LL_GPIO_SetPinMode(GPIOA, SPI1_CS1_Pin, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_CS1_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOA, SPI1_CS1_Pin, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(GPIOA, SPI1_CS1_Pin, LL_GPIO_PULL_NO);

   LL_GPIO_SetPinMode(GPIOA, SPI1_CS2_Pin, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_CS2_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOA, SPI1_CS2_Pin, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(GPIOA, SPI1_CS2_Pin, LL_GPIO_PULL_NO);

   LL_GPIO_SetOutputPin(GPIOA, SPI1_CS1_Pin);
   LL_GPIO_SetOutputPin(GPIOA, SPI1_CS2_Pin);

   LL_GPIO_SetPinMode(GPIOA, SPI1_SCK_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_SCK_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOA, SPI1_SCK_Pin, LL_GPIO_PULL_NO);
#ifdef STM32F401xC
   LL_GPIO_SetAFPin_0_7(GPIOA, SPI1_SCK_Pin, LL_GPIO_AF_5);
#endif

   LL_GPIO_SetPinMode(GPIOA, SPI1_MISO_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_MISO_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOA, SPI1_MISO_Pin, LL_GPIO_PULL_NO);
#ifdef STM32F401xC
   LL_GPIO_SetAFPin_0_7(GPIOA, SPI1_MISO_Pin, LL_GPIO_AF_5);
#endif

   LL_GPIO_SetPinMode(GPIOA, SPI1_MOSI_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOA, SPI1_MOSI_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOA, SPI1_MOSI_Pin, LL_GPIO_PULL_NO);
#ifdef STM32F401xC
   LL_GPIO_SetAFPin_0_7(GPIOA, SPI1_MOSI_Pin, LL_GPIO_AF_5);
#endif

   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
   LL_SPI_Disable(SPI1); /* Disable SPI1 before configuration */

   // NVIC_SetPriority(SPI1_IRQn, 0);    /* Set priority for SPI1_IRQn */
   // NVIC_EnableIRQ(SPI1_IRQn);         /* Enable SPI1_IRQn           */

   /* Configure SPI1 communication */
   LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV64);
   LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
   LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);     /* CPHA 0 */
   LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW); /* CPOL 0 */
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
#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
#elif STM32F401xC
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#endif

   LL_GPIO_SetPinMode(GPIOB, SPI2_SCK_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOB, SPI2_SCK_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOB, SPI2_SCK_Pin, LL_GPIO_PULL_UP);

   LL_GPIO_SetPinMode(GPIOB, SPI2_MOSI_Pin, LL_GPIO_MODE_ALTERNATE);
   LL_GPIO_SetPinSpeed(GPIOB, SPI2_MOSI_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(GPIOB, SPI2_MOSI_Pin, LL_GPIO_PULL_UP);

   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);
   LL_SPI_Disable(SPI2); /* Disable SPI2 before configuration */

   /* Configure SPI2 communication */
   LL_SPI_SetBaudRatePrescaler(SPI2, LL_SPI_BAUDRATEPRESCALER_DIV16);
   LL_SPI_SetTransferDirection(SPI2, LL_SPI_FULL_DUPLEX);
   LL_SPI_SetClockPhase(SPI2, LL_SPI_PHASE_2EDGE);     /* CPHA 1*/
   LL_SPI_SetClockPolarity(SPI2, LL_SPI_POLARITY_LOW); /* CPOL 0 */
   /* Reset value is LL_SPI_MSB_FIRST */
   LL_SPI_SetTransferBitOrder(SPI2, LL_SPI_MSB_FIRST);
   LL_SPI_SetDataWidth(SPI2, LL_SPI_DATAWIDTH_8BIT);
   LL_SPI_SetNSSMode(SPI2, LL_SPI_NSS_SOFT);
   LL_SPI_SetMode(SPI2, LL_SPI_MODE_MASTER);

   LL_SPI_Enable(SPI2);
}

int8_t SPI_Transfer(SPI_TypeDef *dev, uint32_t cs_pin, uint8_t *tx_data, uint8_t *rx_data,
                    uint8_t n)
{
   int8_t   retVal = 0;
   uint32_t t0;

   if (cs_pin)
   {
      LL_GPIO_ResetOutputPin(GPIOA, cs_pin);
   }

   do
   {
      /* --- CLEAR OVR IF SET --- */
      if (LL_SPI_IsActiveFlag_OVR(dev))
      {
         log_info(&spi_dev, "SPI OVR flag set! Clearing...\r\n");
         LL_SPI_ClearFlag_OVR(dev);
      }

      for (uint8_t i = 0; i < n; i++)
      {
         uint8_t tx_byte = 0xFF;
         if (tx_data)
         {
            tx_byte = tx_data[i];
         }

         t0 = TS_Get_ms();
         while (!LL_SPI_IsActiveFlag_TXE(dev))
         {
            if ((TS_Get_ms() - t0) > SPI_TIMEOUT_MS)
            {
               retVal = -ETIMEDOUT;
               goto spi_exit;
            }
         }
         LL_SPI_TransmitData8(dev, tx_byte);

         t0 = TS_Get_ms();
         while (!LL_SPI_IsActiveFlag_RXNE(dev))
         {
            if ((TS_Get_ms() - t0) > SPI_TIMEOUT_MS)
            {
               retVal = -ETIMEDOUT;
               goto spi_exit;
            }
         }

         uint8_t dummy = LL_SPI_ReceiveData8(dev);
         if (rx_data)
         {
            rx_data[i] = dummy;
         }
      }

      t0 = TS_Get_ms();
      while (LL_SPI_IsActiveFlag_BSY(dev))
      {
         if ((TS_Get_ms() - t0) > SPI_TIMEOUT_MS)
         {
            retVal = -EBUSY;
            goto spi_exit;
         }
      }

      if (LL_SPI_IsActiveFlag_MODF(dev))
      {
         log_info(&spi_dev, "SPI MODF flag set! Clearing...\r\n");
         LL_SPI_ClearFlag_MODF(dev);
         retVal = -EPROTO;
      }
   }
   while (0);

spi_exit:
   if (cs_pin)
   {
      LL_GPIO_SetOutputPin(GPIOA, cs_pin);
   }

   return retVal;
}

int8_t SPI_TransferByte(SPI_TypeDef *dev, uint32_t cs_pin, uint8_t tx_data, uint8_t *rx_data)
{
   return SPI_Transfer(dev, cs_pin, &tx_data, rx_data, 1);
}