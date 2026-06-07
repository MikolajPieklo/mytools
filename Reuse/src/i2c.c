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
#include <errno.h>

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
#ifdef STM32F103xB
#define I2C2_SCL_PIN    LL_GPIO_PIN_10
#define I2C2_SDA_PIN    LL_GPIO_PIN_11
#define I2C2_SPEEDCLOCK 100000U
#define I2C2_DUTYCYCLE  LL_I2C_DUTYCYCLE_2
#elif STM32F401xC
#define I2C1_SCL_PIN    LL_GPIO_PIN_8
#define I2C1_SDA_PIN    LL_GPIO_PIN_9
#define I2C1_SPEEDCLOCK 100000U
#define I2C1_DUTYCYCLE  LL_I2C_DUTYCYCLE_2

#define I2C2_SCL_PIN    LL_GPIO_PIN_10
#define I2C2_SDA_PIN    LL_GPIO_PIN_11
#define I2C2_SPEEDCLOCK 100000U
#define I2C2_DUTYCYCLE  LL_I2C_DUTYCYCLE_2
#endif

#define I2C_SCAN_TIMEOUT_MS 2U

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef enum
{
   I2C_WRITE_MODE = 0,
   I2C_READ_MODE = 1,
} i2c_frame_mode_t;

/************************************
 * STATIC VARIABLES
 ************************************/
uint32_t i2c_start_time_ms = 0U;

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static inline int8_t i2c_start(I2C_TypeDef *dev, uint8_t timeout);
static inline int8_t i2c_stop(I2C_TypeDef *dev, uint8_t timeout);
static inline int8_t i2c_send_address(I2C_TypeDef *dev, uint8_t dev_address, i2c_frame_mode_t mode,
                                      uint8_t timeout);
static inline int8_t i2c_send_data(I2C_TypeDef *dev, uint8_t *data, uint8_t timeout);
static inline int8_t i2c_receive_data(I2C_TypeDef *dev, uint8_t *data, uint8_t timeout);
static inline bool   is_TimeOut(uint8_t timeout);
uint8_t              nibble_to_hex(uint8_t nibble);

/************************************
 * STATIC FUNCTIONS
 ************************************/
static inline int8_t i2c_start(I2C_TypeDef *dev, uint8_t timeout)
{
   int8_t status = 0;
   LL_I2C_GenerateStartCondition(dev);
   while ((!LL_I2C_IsActiveFlag_SB(dev)) && (!is_TimeOut(timeout)))
   {
   }
   if (is_TimeOut(I2C_SCAN_TIMEOUT_MS))
   {
      status = -ETIMEDOUT;
   }
   return status;
}

static inline int8_t i2c_stop(I2C_TypeDef *dev, uint8_t timeout)
{
   int8_t status = 0;
   LL_I2C_GenerateStopCondition(dev);
   while ((LL_I2C_IsActiveFlag_STOP(dev)) && (!is_TimeOut(timeout)))
   {
   }
   if (is_TimeOut(timeout))
   {
      status = -ETIMEDOUT;
   }
   return status;
}

static inline int8_t i2c_send_address(I2C_TypeDef *dev, uint8_t addr, i2c_frame_mode_t mode,
                                      uint8_t timeout)
{
   int8_t status = 0;
   LL_I2C_TransmitData8(dev, (addr | mode));
   while ((!LL_I2C_IsActiveFlag_ADDR(dev)) && (!is_TimeOut(timeout)))
   {
   }
   LL_I2C_ClearFlag_ADDR(dev);
   if (is_TimeOut(I2C_SCAN_TIMEOUT_MS))
   {
      status = -ETIMEDOUT;
   }
   return status;
}

static inline int8_t i2c_send_data(I2C_TypeDef *dev, uint8_t *data, uint8_t timeout)
{
   int8_t status = 0;
   LL_I2C_TransmitData8(dev, *data);
   while ((!LL_I2C_IsActiveFlag_TXE(dev)) && (!is_TimeOut(timeout)))
   {
   }
   if (is_TimeOut(timeout))
   {
      status = -ETIMEDOUT;
   }
   return status;
}

static inline int8_t i2c_receive_data(I2C_TypeDef *dev, uint8_t *data, uint8_t timeout)
{
   int8_t status = 0;
   while ((!LL_I2C_IsActiveFlag_RXNE(dev)) && (!is_TimeOut(timeout)))
   {
   }
   *data = LL_I2C_ReceiveData8(dev);
   if (is_TimeOut(timeout))
   {
      status = -ETIMEDOUT;
   }
   return status;
}

static inline bool is_TimeOut(uint8_t timeout)
{
   if ((TS_Get_ms() - i2c_start_time_ms) > timeout)
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
#ifdef STM32F103xB
         LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
#elif STM32F401xC
         LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#endif
         LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

         LL_GPIO_SetPinMode(GPIOB, I2C2_SCL_PIN, LL_GPIO_MODE_ALTERNATE);
         LL_GPIO_SetPinSpeed(GPIOB, I2C2_SCL_PIN, LL_GPIO_SPEED_FREQ_HIGH);
         LL_GPIO_SetPinOutputType(GPIOB, I2C2_SCL_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
         LL_GPIO_SetPinPull(GPIOB, I2C2_SCL_PIN, LL_GPIO_PULL_NO);

         LL_GPIO_SetPinMode(GPIOB, I2C2_SDA_PIN, LL_GPIO_MODE_ALTERNATE);
         LL_GPIO_SetPinSpeed(GPIOB, I2C2_SDA_PIN, LL_GPIO_SPEED_FREQ_HIGH);
         LL_GPIO_SetPinOutputType(GPIOB, I2C2_SDA_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
         LL_GPIO_SetPinPull(GPIOB, I2C2_SDA_PIN, LL_GPIO_PULL_NO);
      }
      else if (I2C1 == dev)
      {
#ifdef STM32F103xB
         LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
#elif STM32F401xC
         LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#endif
         LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

         LL_GPIO_SetPinMode(GPIOB, I2C1_SCL_PIN, LL_GPIO_MODE_ALTERNATE);
         LL_GPIO_SetPinSpeed(GPIOB, I2C1_SCL_PIN, LL_GPIO_SPEED_FREQ_HIGH);
         LL_GPIO_SetPinOutputType(GPIOB, I2C1_SCL_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
         LL_GPIO_SetPinPull(GPIOB, I2C1_SCL_PIN, LL_GPIO_PULL_UP);
#ifdef STM32F401xC
         LL_GPIO_SetAFPin_8_15(GPIOB, I2C1_SCL_PIN, LL_GPIO_AF_4);
#endif

         LL_GPIO_SetPinMode(GPIOB, I2C1_SDA_PIN, LL_GPIO_MODE_ALTERNATE);
         LL_GPIO_SetPinSpeed(GPIOB, I2C1_SDA_PIN, LL_GPIO_SPEED_FREQ_HIGH);
         LL_GPIO_SetPinOutputType(GPIOB, I2C1_SDA_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
         LL_GPIO_SetPinPull(GPIOB, I2C1_SDA_PIN, LL_GPIO_PULL_UP);
#ifdef STM32F401xC
         LL_GPIO_SetAFPin_8_15(GPIOB, I2C1_SDA_PIN, LL_GPIO_AF_4);
#endif
      }


      LL_I2C_Disable(dev);
      LL_RCC_GetSystemClocksFreq(&rcc_clocks);
      LL_I2C_ConfigSpeed(dev, rcc_clocks.PCLK1_Frequency, I2C2_SPEEDCLOCK, I2C2_DUTYCYCLE);
      // LL_I2C_SetClockSpeedMode(dev, LL_I2C_CLOCK_SPEED_STANDARD_MODE);
      LL_I2C_SetMode(dev, LL_I2C_MODE_I2C);
      LL_I2C_Enable(dev);
   }
   while (0);
   I2C_Master_Scan(dev, I2C_SCAN_TIMEOUT_MS);

   return status;
}

int8_t I2C_Master_Scan(I2C_TypeDef *dev, uint32_t timeout)
{
   int8_t        status = 0;
   uint8_t       i, base = 0x00U;
   const uint8_t addr_min = 0x03;
   const uint8_t addr_max = 0x77;

   log_info(&i2c_dev, "    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
   for (base = 0U; base < 0x80U; base += 0x10)
   {
      uint8_t text[] = "-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --";
      for (i = 0U; i < 0x10U; i++)
      {
         i2c_start_time_ms = TS_Get_ms();
         uint8_t addr = (base + i);
         if (addr < addr_min || addr > addr_max)
         {
            continue;
         }
         // start
         status = i2c_start(dev, timeout);
         if (status < 0)
         {
            continue;
         }

         // address
         status = i2c_send_address(dev, addr << 1, I2C_WRITE_MODE, timeout);
         if (status == 0)
         {
            text[i * 3U] = nibble_to_hex((addr >> 4) & 0x0F);
            text[i * 3U + 1U] = nibble_to_hex(addr & 0x0F);
         }
      }
      log_info(&i2c_dev, "%2x: %s\r\n", base, text);
   }

   return 0;
}

int8_t I2C_Master_Write(I2C_TypeDef *dev, uint8_t dev_address, uint8_t *data, uint8_t size,
                        uint32_t timeout)
{
   int8_t  status = 0;
   uint8_t idx = 0u;

   do
   {
      i2c_start_time_ms = TS_Get_ms();

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         log_err(&i2c_dev, "start%x\r\n", dev_address);
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_WRITE_MODE, timeout);
      if (status < 0)
      {
         log_err(&i2c_dev, "addres %x\r\n", dev_address);
         break;
      }

      if (0 < size)
      {
         for (idx = 0; idx < size; idx++)
         {
            status = i2c_send_data(dev, data + idx, timeout);
            if (status < 0)
            {
               break;
               log_err(&i2c_dev, "data %x\r\n", dev_address);
            }
         }
      }

      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         log_err(&i2c_dev, "stop %x\r\n", dev_address);
         break;
      }
   }
   while (0);
   return status;
}

int8_t I2C_Master_Read(I2C_TypeDef *dev, uint8_t dev_address, uint8_t *data, uint8_t len,
                       uint32_t timeout)
{
   int8_t  status = 0;
   uint8_t idx = 0;

   do
   {
      i2c_start_time_ms = TS_Get_ms();

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         log_err(&i2c_dev, "start %x\r\n", dev_address);
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_READ_MODE, timeout);
      if (status < 0)
      {
         log_err(&i2c_dev, "addres %x\r\n", dev_address);
         break;
      }

      for (idx = 0; idx < len; idx++)
      {
         if (idx > len - 1)
         {
            LL_I2C_AcknowledgeNextData(dev, LL_I2C_NACK);
#ifdef STM32F103xB
            status = i2c_stop(dev, timeout);
            if (status < 0)
            {
               break;
            }
#endif
         }
         else
         {
            LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);
         }

         status = i2c_receive_data(dev, (data + idx), timeout);
         if (status < 0)
         {
            log_err(&i2c_dev, "read %x\r\n", dev_address);
            break;
         }
      }

#ifdef STM32F401xC
      // Stop
      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         log_err(&i2c_dev, "stop %x\r\n", dev_address);
         break;
      }
#endif
   }
   while (0);
   return status;
}

int8_t I2C_Master_Reg8_Transmit_Byte(I2C_TypeDef *dev, uint8_t dev_address, uint8_t reg,
                                     uint8_t data, uint32_t timeout)
{
   int8_t status = 0;

   do
   {
      i2c_start_time_ms = TS_Get_ms();

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_WRITE_MODE, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &reg, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &data, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         break;
      }
   }
   while (0);
   return status;
}

int8_t I2C_Master_Reg16_Transmit_Byte(I2C_TypeDef *dev, uint8_t dev_address, uint16_t reg,
                                      uint8_t data, uint32_t timeout)
{
   int8_t  status = 0;
   uint8_t reg_high = (reg >> 8) & 0xFF;
   uint8_t reg_low = reg & 0xFF;

   do
   {
      i2c_start_time_ms = TS_Get_ms();

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_WRITE_MODE, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &reg_high, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &reg_low, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &data, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         break;
      }
   }
   while (0);
   return status;
}

int8_t I2C_Master_Reg8_Transmit_Bytes(I2C_TypeDef *dev, uint8_t dev_address, uint8_t reg,
                                      uint8_t *data, uint8_t len, uint32_t timeout)
{
   int8_t  status = 0;
   uint8_t idx = 0;

   do
   {
      i2c_start_time_ms = TS_Get_ms();

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_WRITE_MODE, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &reg, timeout);
      if (status < 0)
      {
         break;
      }

      for (idx = 0; idx < len - 1; idx++)
      {
         status = i2c_send_data(dev, (data + idx), timeout);
         if (status < 0)
         {
            break;
         }
      }

      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         break;
      }
   }
   while (0);
   return status;
}

int8_t I2C_Master_Reg16_Transmit_Bytes(I2C_TypeDef *dev, uint8_t dev_address, uint16_t reg,
                                       uint8_t *data, uint8_t len, uint32_t timeout)
{
   int8_t  status = 0;
   uint8_t idx = 0;
   uint8_t reg_high = (reg >> 8) & 0xFF;
   uint8_t reg_low = reg & 0xFF;

   do
   {
      i2c_start_time_ms = TS_Get_ms();

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_WRITE_MODE, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &reg_high, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &reg_low, timeout);
      if (status < 0)
      {
         break;
      }

      if (0U < len)
      {
         for (idx = 0; idx < len - 1; idx++)
         {
            status = i2c_send_data(dev, (data + idx), timeout);
            if (status < 0)
            {
               break;
            }
         }
      }

      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         break;
      }
   }
   while (0);
   return status;
}

int8_t I2C_Master_Reg8_Recessive_Bytes(I2C_TypeDef *dev, uint8_t dev_address, uint8_t reg,
                                       uint8_t *data, uint8_t len, uint32_t timeout)
{
   int8_t  status = 0;
   uint8_t idx = 0;

   do
   {
      i2c_start_time_ms = TS_Get_ms();

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_WRITE_MODE, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &reg, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_READ_MODE, timeout);
      if (status < 0)
      {
         break;
      }

      // data
      for (idx = 0; idx < len; idx++)
      {
         if (idx == len - 1)
         {
            LL_I2C_AcknowledgeNextData(dev, LL_I2C_NACK);
#ifdef STM32F103xB
            status = i2c_stop(dev, timeout);
            if (status < 0)
            {
               break;
            }
#endif
         }
         else
         {
            LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);
         }

         status = i2c_receive_data(dev, (data + idx), timeout);
         if (status < 0)
         {
            break;
         }
      }
#ifdef STM32F401xC
      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         break;
      }
#endif
   }
   while (0);
   return status;
}

int8_t I2C_Master_Reg16_Recessive_Bytes(I2C_TypeDef *dev, uint8_t dev_address, uint16_t reg,
                                        uint8_t *data, uint8_t len, uint32_t timeout)
{
   int8_t  status = 0;
   uint8_t idx = 0;
   uint8_t reg_high = (reg >> 8) & 0xFF;
   uint8_t reg_low = reg & 0xFF;

   do
   {
      i2c_start_time_ms = TS_Get_ms();

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_WRITE_MODE, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_data(dev, &reg_high, timeout);
      if (status < 0)
      {
         break;
      }
      status = i2c_send_data(dev, &reg_low, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_start(dev, timeout);
      if (status < 0)
      {
         break;
      }

      status = i2c_send_address(dev, dev_address, I2C_READ_MODE, timeout);
      if (status < 0)
      {
         break;
      }

      // data
      for (idx = 0; idx < len; idx++)
      {
         if (idx == len - 1)
         {
            LL_I2C_AcknowledgeNextData(dev, LL_I2C_NACK);
#ifdef STM32F103xB
            status = i2c_stop(dev, timeout);
            if (status < 0)
            {
               break;
            }
#endif
         }
         else
         {
            LL_I2C_AcknowledgeNextData(dev, LL_I2C_ACK);
         }

         status = i2c_receive_data(dev, (data + idx), timeout);
         if (status < 0)
         {
            break;
         }
      }
#ifdef STM32F401xC
      status = i2c_stop(dev, timeout);
      if (status < 0)
      {
         break;
      }
#endif
   }

   while (0);

   return status;
}
