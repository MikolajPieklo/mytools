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
#include <one_wire.h>

#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_bus.h>

#include <stdint.h>
#include <uart.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define DS18B20_SCRATCHPAD_SIZE    9
#define DS18B20_READ_ROM           0x33
#define DS18B20_MATCH_ROM          0x55
#define DS18B20_SKIP_ROM           0xCC
#define DS18B20_CONVERT_T          0x44
#define DS18B20_READ_SCRATCHPAD    0xBE

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
   uint8_t tx = 0xF0;

   USARTx_Set_BaudRate(USART2, 9600);
   USARTx_Tx(USART2, &tx, 1);
}

static void onewire_write(uint8_t byte)
{
   uint8_t i = 0;

   USARTx_Set_BaudRate(USART2, 115200);

   for (i = 0; i < 8; i++)
   {
      write_bit(byte & 0x01);
      byte >>= 1;
   }
}

static uint8_t onewire_read(void)
{
   uint8_t value = 0;
   uint8_t i = 0;

   USARTx_Set_BaudRate(USART2, 115200);

   for (i = 0; i < 8; i++)
   {
      value >>= 1;
      if (read_bit())
         value |= 0x80;
   }
   return value;
}

static void write_bit(uint8_t value)
{
   uint8_t tx = 0x00U;
   if (value)
   {
      tx = 0xff;
   }
   else
   {
      tx = 0x0;
   }
   USARTx_Tx(USART2, &tx, 1);
}

static uint8_t read_bit(void)
{
   uint8_t tx = 0xFF;
   uint8_t rx = 0;
   USARTx_Tx(USART2, &tx, 1);
   USARTx_Rx(USART2, &rx, 1);
   return rx & 0x01;
}

static uint8_t byte_crc(uint8_t crc, uint8_t byte)
{
   int i;
   for (i = 0; i < 8; i++) {
      uint8_t b = crc ^ byte;
      crc >>= 1;
      if (b & 0x01)
         crc ^= 0x8c;
      byte >>= 1;
   }
   return crc;
}

// read addres
// send 0x33
// odp 8 bajtow


// wire_reset();
// wire_write(0xcc); //Skip ROM
// wire_write(0xbe); //Read Scratchpad
// odp 9 bajtow


// wire_reset();
// wire_write(0xcc);
// wire_write(0x44); // Convert T
// HAL_Delay(750);
// wire_reset();
// wire_write(0xcc);
// wire_write(0xbe);



/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void OneWire_Init(void)
{
   USART2_Init();
}

void OneWire_Read(void)
{
   onewire_reset();
}

void OneWire_Write()
{

}