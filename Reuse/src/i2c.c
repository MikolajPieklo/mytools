/**
 ********************************************************************************
 * @file    i2c.c
 * @author  Mikolaj Pieklo
 * @date    12.10.2023
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "i2c.h"

#include "delay.h"
#include "log.h"

#ifdef STM32F103xB
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_i2c.h>
#include <stm32f1xx_ll_rcc.h>
#elif STM32F401xC
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_i2c.h>
#include <stm32f4xx_ll_rcc.h>
#else
#error Module not supported!
#endif

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device i2c_dev = {
   .name = "I2C",
};
#define I2C2_SCL_PIN    LL_GPIO_PIN_10
#define I2C2_SDA_PIN    LL_GPIO_PIN_11
#define I2C2_SPEEDCLOCK 100000
#define I2C2_DUTYCYCLE  LL_I2C_DUTYCYCLE_2

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/
uint32_t start_time_ms = 0U;

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
bool    is_TimeOut();
uint8_t nibble_to_hex(uint8_t nibble);

/************************************
 * STATIC FUNCTIONS
 ************************************/
bool is_TimeOut()
{
   if ((TS_Get_ms() - start_time_ms) > 2)
   {
      return true;
   }
   return false;
}

uint8_t nibble_to_hex(uint8_t nibble)
{
   return (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
I2c_Drv_Status_T I2C_Init(I2C_TypeDef *dev)
{
   I2c_Drv_Status_T     status = I2C_DRV_STATUS_SUCCESS;
   LL_RCC_ClocksTypeDef rcc_clocks;

   do
   {
      if (I2C2 == dev)
      {
         LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
         LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

         LL_GPIO_SetPinMode(GPIOB, I2C2_SCL_PIN, LL_GPIO_MODE_ALTERNATE);
         LL_GPIO_SetPinSpeed(GPIOB, I2C2_SCL_PIN, LL_GPIO_SPEED_FREQ_HIGH);
         LL_GPIO_SetPinOutputType(GPIOB, I2C2_SCL_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
         LL_GPIO_SetPinPull(GPIOB, I2C2_SCL_PIN, LL_GPIO_PULL_UP);

         LL_GPIO_SetPinMode(GPIOB, I2C2_SDA_PIN, LL_GPIO_MODE_ALTERNATE);
         LL_GPIO_SetPinSpeed(GPIOB, I2C2_SDA_PIN, LL_GPIO_SPEED_FREQ_HIGH);
         LL_GPIO_SetPinOutputType(GPIOB, I2C2_SDA_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
         LL_GPIO_SetPinPull(GPIOB, I2C2_SDA_PIN, LL_GPIO_PULL_UP);
      }

      LL_I2C_Disable(dev);
      LL_RCC_GetSystemClocksFreq(&rcc_clocks);
      LL_I2C_ConfigSpeed(dev, rcc_clocks.PCLK1_Frequency, I2C2_SPEEDCLOCK, I2C2_DUTYCYCLE);
      LL_I2C_SetClockSpeedMode(dev, LL_I2C_CLOCK_SPEED_STANDARD_MODE);
      LL_I2C_SetMode(dev, LL_I2C_MODE_I2C);
      LL_I2C_Enable(dev);
   }
   while (0);
   I2C_Master_Scan(dev, I2C_DRV_TIMEOUT_MS);

   return status;
}

bool I2C_Master_Scan(I2C_TypeDef *dev, uint32_t timeout)
{
   uint8_t       i, base = 0x00U;
   const uint8_t addr_min = 0x03;
   const uint8_t addr_max = 0x77;

   log_info(&i2c_dev, "    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
   for (base = 0U; base < 0x80U; base += 0x10)
   {
      uint8_t text[] = "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --";
      for (i = 0U; i < 0x10U; i++)
      {
         start_time_ms = TS_Get_ms();
         uint8_t addr = (base + i);
         if (addr < addr_min || addr > addr_max)
         {
            continue;
         }
         // start
         LL_I2C_GenerateStartCondition(dev);
         while (!LL_I2C_IsActiveFlag_SB(dev))
         {
         }

         // address
         LL_I2C_TransmitData8(dev, (addr << 1) | 0);
         while ((!LL_I2C_IsActiveFlag_ADDR(dev)) && (!is_TimeOut()))
         {
         }
         LL_I2C_ClearFlag_ADDR(dev);
         if (!is_TimeOut())
         {
            text[i * 3U] = nibble_to_hex((addr >> 4) & 0x0F);
            text[i * 3U + 1U] = nibble_to_hex(addr & 0x0F);
         }
      }
      log_info(&i2c_dev, "%x: %s\r\n", base, text);
   }

   return true;
}

bool I2C_Master_Write(I2C_TypeDef *dev, uint8_t address, uint8_t data, uint32_t timeout)
{
   bool status = true;

   // start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // address
   LL_I2C_TransmitData8(dev, address | 0);
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // data
   LL_I2C_TransmitData8(dev, data);
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }

   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   return status;
}

bool I2C_Master_Read(I2C_TypeDef *dev, uint8_t address, uint8_t *data, uint8_t len,
                     uint32_t timeout)
{
   bool    status = true;
   uint8_t idx = 0;

   LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);

   // start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // address
   LL_I2C_TransmitData8(dev, address | 1);
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // data
   for (idx = 0; idx < len; idx++)
   {
      if (idx == len - 1)
      {
         LL_I2C_AcknowledgeNextData(dev, LL_I2C_NACK);
      }
      else
      {
         LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);
      }

      while (!LL_I2C_IsActiveFlag_RXNE(dev))
      {
      }
      *(data + idx) = LL_I2C_ReceiveData8(dev);
   }

   // Stop
   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   return status;
}

bool I2C_Master_Reg8_Transmit_Byte(I2C_TypeDef *dev, uint8_t address, uint8_t reg, uint8_t data,
                                   uint32_t timeout)
{
   bool status = true;

   // start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // address
   LL_I2C_TransmitData8(dev, address | 0);
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // reg
   LL_I2C_TransmitData8(dev, reg);
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }

   // data
   LL_I2C_TransmitData8(dev, data);
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }

   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   return status;
}

bool I2C_Master_Reg16_Transmit_Byte(I2C_TypeDef *dev, uint8_t address, uint16_t reg, uint8_t data,
                                    uint32_t timeout)
{
   bool status = true;

   // start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // address
   LL_I2C_TransmitData8(dev, address | 0);
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // reg
   LL_I2C_TransmitData8(dev, (uint8_t) (reg >> 8));
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }
   LL_I2C_TransmitData8(dev, (uint8_t) (reg & 0xFF));
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }

   // data
   LL_I2C_TransmitData8(dev, data);
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }

   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   return status;
}

bool I2C_Master_Reg8_Transmit_Bytes(I2C_TypeDef *dev, uint8_t address, uint8_t reg, uint8_t *data,
                                    uint8_t len, uint32_t timeout)
{
   bool    status = true;
   uint8_t idx = 0;

   // start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // address
   LL_I2C_TransmitData8(dev, address | 0);
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // reg
   LL_I2C_TransmitData8(dev, reg);
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }

   // data
   for (idx = 0; idx < len; idx++)
   {
      LL_I2C_TransmitData8(dev, *(data + idx));
      while (!LL_I2C_IsActiveFlag_TXE(dev))
      {
      }
   }

   // Stop
   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   return status;
}

bool I2C_Master_Reg16_Transmit_Bytes(I2C_TypeDef *dev, uint8_t address, uint16_t reg, uint8_t *data,
                                     uint8_t len, uint32_t timeout)
{
   bool    status = true;
   uint8_t idx = 0;

   // start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // address
   LL_I2C_TransmitData8(dev, address | 0);
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // reg
   LL_I2C_TransmitData8(dev, (uint8_t) (reg >> 8));
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }
   LL_I2C_TransmitData8(dev, (uint8_t) (reg & 0xFF));
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }

   // data
   for (idx = 0; idx < len; idx++)
   {
      LL_I2C_TransmitData8(dev, *(data + idx));
      while (!LL_I2C_IsActiveFlag_TXE(dev))
      {
      }
   }

   // Stop
   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   return status;
}

bool I2C_Master_Reg8_Recessive_Bytes(I2C_TypeDef *dev, uint8_t address, uint8_t reg, uint8_t *data,
                                     uint8_t len, uint32_t timeout)
{
   bool    status = true;
   uint8_t idx = 0;

   LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);

   // start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // address
   LL_I2C_TransmitData8(dev, address | 0);
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // reg
   LL_I2C_TransmitData8(dev, reg);
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }

   // Stop
   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   // second start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // address
   LL_I2C_TransmitData8(dev, address | 2); // Read
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);

   // data
   for (idx = 0; idx < len; idx++)
   {
      if (idx == len - 1)
      {
         LL_I2C_AcknowledgeNextData(dev, LL_I2C_NACK);
      }
      else
      {
         LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);
      }

      while (!LL_I2C_IsActiveFlag_RXNE(dev))
      {
      }
      *(data + idx) = LL_I2C_ReceiveData8(dev);
   }

   // Stop
   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   return status;
}

bool I2C_Master_Reg16_Recessive_Bytes(I2C_TypeDef *dev, uint8_t address, uint16_t reg,
                                      uint8_t *data, uint8_t len, uint32_t timeout)
{
   bool    status = true;
   uint8_t idx = 0;

   LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);

   // start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // address
   LL_I2C_TransmitData8(dev, address | 0);
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // reg
   LL_I2C_TransmitData8(dev, (uint8_t) (reg >> 8));
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }
   LL_I2C_TransmitData8(dev, (uint8_t) (reg & 0xFF));
   while (!LL_I2C_IsActiveFlag_TXE(dev))
   {
   }

   // Stop
   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   // Second start
   LL_I2C_GenerateStartCondition(dev);
   while (!LL_I2C_IsActiveFlag_SB(dev))
   {
   }

   // Address
   LL_I2C_TransmitData8(dev, address | 1); // Read
   while (!LL_I2C_IsActiveFlag_ADDR(dev))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);

   // LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);

   // data
   for (idx = 0; idx < len; idx++)
   {
      if (idx == len - 1)
      {
         LL_I2C_AcknowledgeNextData(dev, LL_I2C_NACK);
      }
      else
      {
         LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);
      }

      while (!LL_I2C_IsActiveFlag_RXNE(dev))
      {
      }
      *(data + idx) = LL_I2C_ReceiveData8(dev);
   }

   // Stop
   LL_I2C_GenerateStopCondition(dev);
   while (LL_I2C_IsActiveFlag_STOP(dev))
   {
   }

   return status;
}
