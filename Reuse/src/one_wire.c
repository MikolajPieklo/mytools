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

#include <stdint.h>

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>

#include <delay.h>
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
static void    onewire_reset(void);
static void    onewire_write(uint8_t byte);
static uint8_t onewire_read(void);
static void    write_bit(uint8_t value);
static uint8_t read_bit(void);

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

static uint8_t onewire_read(void)
{
   uint8_t value = 0U;
   uint8_t i = 0U;

   USARTx_Set_BaudRate(USART2, 115200U);

   for (i = 0U; i < 8U; i++)
   {
      value >>= 1U;
      if (read_bit())
      {
         value |= 0x80U;
      }
   }
   return value;
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

static uint8_t read_bit(void)
{
   uint8_t tx = 0xFFU;
   uint8_t rx = 0U;
   USARTx_Tx(USART2, &tx, 1U);
   USARTx_Rx(USART2, &rx, 1U);
   return rx & 0x01U;
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void OneWire_Init(void)
{
   USART2_Init();
}

void OneWire_Read(uint8_t *tx, uint8_t tx_size, uint8_t *rx, uint8_t rx_size)
{
   uint8_t i;

   onewire_reset();

   for (i = 0U; i < tx_size; i++)
   {
      onewire_write(tx[i]);
   }

   for (i = 0U; i < rx_size; i++)
   {
      rx[i] = onewire_read();
   }

   return;
}

void OneWire_Write(uint8_t *tx, uint8_t tx_size)
{
   uint8_t i;

   onewire_reset();

   for (i = 0U; i < tx_size; i++)
   {
      onewire_write(tx[i]);
   }
}