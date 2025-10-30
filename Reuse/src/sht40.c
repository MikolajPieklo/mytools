/**
 ********************************************************************************
 * @file    sht40.c
 * @author  Mikolaj Pieklo
 * @date    21.10.2025
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "sht40.h"

#include <stdbool.h>

#include "crc8.h"
#include "delay.h"
#include "i2c.h"
#include "log.h"
#include <errno.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device sht40_dev = {
   .name = "SHT40",
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

/************************************
 * STATIC FUNCTIONS
 ************************************/

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
int8_t SHT40_Get_Temperature(int16_t *temperature)
{
   uint8_t rx[6];
   int8_t  status = 0;

   do
   {
      status = I2C_Master_Write(I2C2, 0x88U, 0xFD, I2C_DRV_TIMEOUT_MS);
      if (0 != status)
      {
         log_err(&sht40_dev, "SHT40 write command error Status %d\r\n", status);
         break;
      }
      TS_Delay_ms(10);

      status = I2C_Master_Read(I2C2, 0x88U, rx, 6, I2C_DRV_TIMEOUT_MS);
      if (0 != status)
      {
         log_err(&sht40_dev, "SHT40 read data error Status %d\r\n", status);
         break;
      }
      uint32_t t_ticks = rx[0] * 256 + rx[1];
      int32_t  t_degC = -45 + 175 * t_ticks / 65535;
      *temperature = (int16_t) t_degC;
      uint8_t crc = crc8_nrsc5(rx, 2);
      if (crc != rx[2])
      {
         log_err(&sht40_dev, "SHT40 temperature CRC error d1=0x%x d2=0x%x c1=0x%x c2=0x%x\r\n",
                 rx[0], rx[1], crc, rx[2]);
         status = -EBADMSG;
         return -1;
      }

      // log_info(&main_dev, "Read: %02x %02x %02x %02x %02x %02x\r\n", rx[0], rx[1], rx[2],
      // rx[3],
      //          rx[4], rx[5]);
      log_info(&sht40_dev, "Temp : %d\r\n", t_degC);
   }
   while (0);

   return status;
}
