/**
 ********************************************************************************
 * @file    ds18b20.c
 * @author  Mikolaj Pieklo
 * @date    22.08.2025
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "ds18b20.h"

#include <stdbool.h>

#include <delay.h>
#include <errno.h>
#include <log.h>
#include <one_wire.h>
/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device device_dev = {
   .name = "DS18B20",
};

#define DS18B20_ROM_CODE_SIZE   8U
#define DS18B20_SCRATCHPAD_SIZE 9U

#define DS18B20_READ_ROM        0x33U
#define DS18B20_MATCH_ROM       0x55U
#define DS18B20_SKIP_ROM        0xCCU
#define DS18B20_CONVERT_T       0x44U
#define DS18B20_READ_SCRATCHPAD 0xBEU

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/
static bool init = false;

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static uint8_t byte_crc(uint8_t crc, uint8_t byte);
static int8_t  ds18b20_read_address(void);
static int8_t  ds18b20_read_memory(int16_t *temperature);
static int8_t  ds18b20_get_temperature(void);

/************************************
 * STATIC FUNCTIONS
 ************************************/
static uint8_t byte_crc(uint8_t crc, uint8_t byte)
{
   uint8_t i;
   for (i = 0U; i < 8U; i++)
   {
      uint8_t b = crc ^ byte;
      crc >>= 1;
      if (b & 0x01)
      {
         crc ^= 0x8c;
      }
      byte >>= 1;
   }
   return crc;
}

static uint8_t wire_crc(const uint8_t *data, uint8_t len)
{
   uint8_t i;
   uint8_t crc = 0;
   for (i = 0U; i < len; i++)
   {
      crc = byte_crc(crc, data[i]);
   }
   return crc;
}

static int8_t ds18b20_read_address(void)
{
   int8_t  retval = 0;
   uint8_t tx = DS18B20_READ_ROM;
   uint8_t crc = 0U;
   uint8_t rx_data[8] = {0};

   OneWire_Read(&tx, 1U, rx_data, DS18B20_ROM_CODE_SIZE);
   crc = wire_crc(rx_data, DS18B20_ROM_CODE_SIZE - 1);
   log_info(&device_dev, "ROM: %02X %02X %02X %02X %02X %02X %02X %02X CRC: %02X\r\n", rx_data[0],
            rx_data[1], rx_data[2], rx_data[3], rx_data[4], rx_data[5], rx_data[6], rx_data[7],
            crc);

   if (crc != rx_data[DS18B20_ROM_CODE_SIZE - 1])
   {
      log_info(&device_dev, "CRC ERROR!\r\n");
      retval = -EBADMSG;
   }

   return retval;
}

static int8_t ds18b20_read_memory(int16_t *temperature)
{
   int8_t  retval = 0;
   uint8_t tx[2] = {DS18B20_SKIP_ROM, DS18B20_READ_SCRATCHPAD};
   uint8_t crc = 0U;
   uint8_t rx_data[9] = {0};

   do
   {
      OneWire_Read(tx, 2U, rx_data, DS18B20_SCRATCHPAD_SIZE);
      crc = wire_crc(rx_data, DS18B20_SCRATCHPAD_SIZE - 1);
      log_info(&device_dev,
               "SCRATCHPAD: %02X %02X %02X %02X %02X %02X %02X %02X %02X CRC: %02X\r\n", rx_data[0],
               rx_data[1], rx_data[2], rx_data[3], rx_data[4], rx_data[5], rx_data[6], rx_data[7],
               rx_data[8], crc);

      if (NULL != temperature)
      {
         *temperature = ((int16_t) (rx_data[1] << 8) | rx_data[0]);
      }

      if (crc == rx_data[DS18B20_SCRATCHPAD_SIZE - 1])
      {
         break;
      }

      rx_data[0] |= 0x01U;
      crc = wire_crc(rx_data, DS18B20_SCRATCHPAD_SIZE - 1);

      if (crc != rx_data[DS18B20_SCRATCHPAD_SIZE - 1])
      {
         log_info(&device_dev, "CRC ERROR!\r\n");
         retval = -EBADMSG;
      }
   }
   while (0);

   return retval;
}

static int8_t ds18b20_get_temperature(void)
{
   int8_t  status = 0;
   uint8_t tx[2] = {DS18B20_SKIP_ROM, DS18B20_CONVERT_T};
   int16_t temperature = 0;
   OneWire_Write(tx, 2U);

   TS_Delay_ms(750);
   status = ds18b20_read_memory(&temperature);
   if (0 == status)
   {
      log_info(&device_dev, "TEMP: %d\r\n", (int16_t) (temperature * 6.25));
   }

   return status;
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
uint8_t DS18B20_Init(void)
{
   do
   {
      int32_t status = 0;
      if (init == false)
      {
         status = ds18b20_read_address();
         if (0 != status)
         {
            break;
         }
         log_info(&device_dev, "DS18B20 Init OK\r\n");
         init = true;
         ds18b20_read_memory(NULL);
      }

      ds18b20_get_temperature();
   }
   while (0);

   return 0;
}
