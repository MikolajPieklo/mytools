/**
 ********************************************************************************
 * @file    one_wire.h
 * @author  Mikolaj Pieklo
 * @date    12.08.2025
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "one_wire.h"

#include <stddef.h>
#include <stdint.h>

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>

#include <delay.h>
#include <errno.h>
#include <log.h>
#include <uart.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device device_dev = {
   .name = "OneWire",
};

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
static void   onewire_reset(void);
static void   onewire_write(uint8_t byte);
static int8_t onewire_read(uint8_t *byte);
static void   write_bit(uint8_t value);
static int8_t read_bit(uint8_t *value);

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void onewire_reset(void)
{
   uint8_t tx = 0xF0U;

   USARTx_Set_BaudRate(USART2, 9600U);
   USARTx_Tx(USART2, &tx, 1U);
}

static void onewire_write(uint8_t byte)
{
   uint8_t i = 0U;

   USARTx_Set_BaudRate(USART2, 115200U);

   for (i = 0U; i < 8U; i++)
   {
      write_bit(byte & 0x01U);
      byte >>= 1U;
   }
}

static int8_t onewire_read(uint8_t *byte)
{
   uint8_t value = 0U;
   uint8_t i = 0U;
   uint8_t rx = 0U;
   int8_t  status = 0;

   USARTx_Set_BaudRate(USART2, 115200U);

   for (i = 0U; i < 8U; i++)
   {
      value >>= 1U;
      status = read_bit(&rx);
      if (status != 0)
      {
         break;
      }
      if (rx)
      {
         value |= 0x80U;
      }
   }
   if (NULL != byte)
   {
      *byte = value;
   }
   return status;
}

static void write_bit(uint8_t value)
{
   uint8_t tx = 0x00U;
   if (value)
   {
      tx = 0xFFU;
   }
   else
   {
      tx = 0x0U;
   }
   USARTx_Tx(USART2, &tx, 1U);
}

static int8_t read_bit(uint8_t *value)
{
   int8_t  status = 0;
   uint8_t tx = 0xFFU;
   uint8_t rx = 0U;
   USARTx_Tx(USART2, &tx, 1U);
   status = USARTx_Rx(USART2, &rx, 1U);
   if (NULL != value)
   {
      *value = rx & 0x01U;
   }
   return status;
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void OneWire_Init(void)
{
   USART2_Init();
}

int8_t OneWire_Read(uint8_t *tx, uint8_t tx_size, uint8_t *rx, uint8_t rx_size)
{
   uint8_t i;
   int8_t  status = 0;

   if (NULL == tx || tx_size == 0U || NULL == rx || rx_size == 0U)
   {
      log_err(&device_dev, "OneWire_Read() - Invalid parameters\r\n");
      return -EINVAL;
   }

   onewire_reset();

   for (i = 0U; i < tx_size; i++)
   {
      onewire_write(tx[i]);
   }

   for (i = 0U; i < rx_size; i++)
   {
      uint8_t tmp_rx = 0U;
      status = onewire_read(&tmp_rx);
      if (status != 0)
      {
         break;
      }
      rx[i] = tmp_rx;
   }

   return status;
}

int8_t OneWire_Write(uint8_t *tx, uint8_t tx_size)
{
   uint8_t i;

   if (NULL == tx || tx_size == 0U)
   {
      log_err(&device_dev, "OneWire_Write() - Invalid parameters\r\n");
      return -EINVAL;
   }
   onewire_reset();

   for (i = 0U; i < tx_size; i++)
   {
      onewire_write(tx[i]);
   }

   return 0;
}