/**
 ********************************************************************************
 * @file    scd41.c
 * @author  Mikolaj Pieklo
 * @date    22.11.2025
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "scd41.h"

#include <stdbool.h>

#include "crc8.h"
#include "delay.h"
#include "i2c.h"
#include "log.h"
#include "machine/_default_types.h"
#include "reuse.h"
#include <errno.h>
#include <string.h>
/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device scd41_dev = {
   .name = "SCD41",
};

#define SCD41_I2C_ADDRESS (0x62u << 1u)

#define SCD41_VARIANT_SCD40 (0x0000u)
#define SCD41_VARIANT_SCD41 (0x0001u)
#define SCD41_VARIANT_SCD43 (0x0005u)

#define SCD41_CMD_READ_MEASUREMENT              0xec05u
#define SCD41_CMD_STOP_PERIODIC_MEASUREMENT     0x3f86u
#define SCD41_CMD_PERFORM_FORCED_RECALIBRATION  0x362fu
#define SCD41_CMD_GET_SERIAL_NUMBER             0x3682u
#define SCD41_CMD_PERFOM_FACTORY_RESET          0x3632u
#define SCD41_CMD_GET_SENSOR_VARIANT            0x202Fu
#define SCD_41_CMD_MEASURE_SINGLE_SHOT          0x219du
#define SCD_41_CMD_MEASURE_SINGLE_SHOT_RHT_ONLY 0x2196u
#define SCD_41_CMD_GET_DATA_READY_STATUS        0xE4B8u

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef enum
{
   SCD41_Initializing = 0,
   SCD41_Starting_Measurement,
   SCD41_Waiting_For_Data,
   SCD41_Reading_Data,
} scd41_t;

/************************************
 * STATIC VARIABLES
 ************************************/
static scd41_t  scd41_state = SCD41_Initializing;
static uint32_t scd41_last_ts_ms = 0;

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static inline int8_t scd41_get_read_measurement(I2C_TypeDef *dev, uint16_t *co2, uint16_t *rh,
                                                int16_t *temperature);
static inline int8_t scd41_stop_periodic_measurement(I2C_TypeDef *dev);
static inline int8_t scd41_get_serial_numer(I2C_TypeDef *dev);

static inline int8_t scd41_get_sensor_variant(I2C_TypeDef *dev);
static inline int8_t scd41_start_single_shot_measurement(I2C_TypeDef *dev);
static inline int8_t scd41_start_single_shot_rht_measurement(I2C_TypeDef *dev);
static inline int8_t scd41_get_data_ready_status(I2C_TypeDef *dev, bool *data_ready);

/************************************
 * STATIC FUNCTIONS
 ************************************/
static inline int8_t scd41_get_read_measurement(I2C_TypeDef *dev, uint16_t *co2, uint16_t *rh,
                                                int16_t *temperature)
{
   int8_t  status;
   uint8_t data[9];
   memset(data, 0u, sizeof(data));

   do
   {
      status = I2C_Master_Reg16_Recessive_Bytes(dev, SCD41_I2C_ADDRESS, SCD41_CMD_READ_MEASUREMENT,
                                                data, 9, I2C_DRV_TIMEOUT_MS);
      if (0 != status)
      {
         log_err(&scd41_dev, "SCD41 get read measurement Status %d\r\n", status);
         break;
      }
      uint16_t raw = (uint16_t) data[0] << 8 | data[1];
      *co2 = raw;
      uint8_t crc = crc8_nrsc5(data, 2);
      if (crc != data[2])
      {
         log_err(&scd41_dev,
                 "SCD41_get_read_measurement CRC error d1=0x%x d2=0x%x c1=0x%x rx_crc=0x%x\r\n",
                 data[0], data[1], crc, data[2]);
         status = -EBADMSG;
         break;
      }
      raw = (uint16_t) data[3] << 8 | data[4];
      *temperature = ((int16_t) -45 + 175 * raw / 65535);
      crc = crc8_nrsc5(data + 3, 2);
      if (crc != data[5])
      {
         log_err(&scd41_dev,
                 "SCD41_get_read_measurement CRC error d1=0x%x d2=0x%x c1=0x%x rx_crc=0x%x\r\n",
                 data[3], data[4], crc, data[5]);
         status = -EBADMSG;
         break;
      }
      raw = (uint16_t) data[6] << 8 | data[7];
      *rh = (uint16_t) (100 * raw / 65535);
      crc = crc8_nrsc5(data + 6, 2);
      if (crc != data[8])
      {
         log_err(&scd41_dev,
                 "SCD41_get_read_measurement CRC error d1=0x%x d2=0x%x c1=0x%x rx_crc=0x%x\r\n",
                 data[6], data[7], crc, data[8]);
         status = -EBADMSG;
         break;
      }
   }
   while (0);

   return status;
}

static inline int8_t scd41_stop_periodic_measurement(I2C_TypeDef *dev)
{
   int8_t  status;
   uint8_t cmd[2] = {
      (uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD41_CMD_STOP_PERIODIC_MEASUREMENT, 8, 0xFFu),
      (uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD41_CMD_STOP_PERIODIC_MEASUREMENT, 0, 0xFFu),
   };

   status = I2C_Master_Write(dev, SCD41_I2C_ADDRESS, cmd, sizeof(cmd), I2C_DRV_TIMEOUT_MS);
   if (0 != status)
   {
      log_err(&scd41_dev, "SCD41 stop periodic measurement error Status %d\r\n", status);
   }

   return status;
}

static inline int8_t scd41_get_serial_numer(I2C_TypeDef *dev)
{
   int8_t  status;
   uint8_t serial_number[9];
   memset(serial_number, 0u, sizeof(serial_number));

   do
   {
      status = I2C_Master_Reg16_Recessive_Bytes(dev, SCD41_I2C_ADDRESS, SCD41_CMD_GET_SERIAL_NUMBER,
                                                serial_number, sizeof(serial_number),
                                                I2C_DRV_TIMEOUT_MS);
      if (0 != status)
      {
         log_err(&scd41_dev, "SCD41 read serial number error Status %d\r\n", status);
         break;
      }

      log_info(&scd41_dev, "SCD41 Serial Number: %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",
               serial_number[0], serial_number[1], serial_number[2], serial_number[3],
               serial_number[4], serial_number[5], serial_number[6], serial_number[7],
               serial_number[8]);
      uint8_t crc = crc8_nrsc5(serial_number, 2);
      if (crc != serial_number[2])
      {
         log_err(&scd41_dev,
                 "SCD41_get_serial_number CRC1 error d1=0x%x d2=0x%x c1=0x%x rx_crc=0x%x\r\n",
                 serial_number[0], serial_number[1], crc, serial_number[2]);
         status = -EBADMSG;
         break;
      }

      crc = crc8_nrsc5(serial_number + 3, 2);
      if (crc != serial_number[5])
      {
         log_err(&scd41_dev,
                 "SCD41_get_serial_number CRC2 error d1=0x%x d2=0x%x c1=0x%x rx_crc=0x%x\r\n",
                 serial_number[3], serial_number[4], crc, serial_number[5]);
         status = -EBADMSG;
         break;
      }

      crc = crc8_nrsc5(serial_number + 6, 2);
      if (crc != serial_number[8])
      {
         log_err(&scd41_dev,
                 "SCD41_get_serial_number CRC3 error d1=0x%x d2=0x%x c_crc=0x%x rx_crc=0x%x\r\n",
                 serial_number[6], serial_number[7], crc, serial_number[8]);
         status = -EBADMSG;
         break;
      }
   }
   while (0);

   return status;
}

static inline int8_t scd41_get_sensor_variant(I2C_TypeDef *dev)
{
   int8_t  status;
   uint8_t serial_variant[3];
   do
   {
      status = I2C_Master_Reg16_Recessive_Bytes(dev, SCD41_I2C_ADDRESS,
                                                SCD41_CMD_GET_SENSOR_VARIANT, serial_variant,
                                                sizeof(serial_variant), I2C_DRV_TIMEOUT_MS);
      if (0 != status)
      {
         log_err(&scd41_dev, "SCD41 read serial number error Status %d\r\n", status);
         break;
      }
      uint8_t crc = crc8_nrsc5(serial_variant, 2);
      if (crc != serial_variant[2])
      {
         log_err(&scd41_dev,
                 "SCD41_get_sensor_variant CRC error d1=0x%x d2=0x%x c1=0x%x rx_crc=0x%x\r\n",
                 serial_variant[0], serial_variant[1], crc, serial_variant[2]);
         status = -EBADMSG;
         break;
      }
      log_info(&scd41_dev, "SCD41 sensor variant: %02x %02x\r\n", serial_variant[0],
               serial_variant[1]);

      uint16_t variant = (uint16_t) REUSE_SHIFT_LEFT_MASK(serial_variant[0], 8u, 0xFFFF)
          | (uint16_t) serial_variant[1];
      variant = REUSE_SHIFT_RIGHT_MASK(variant, 12, 0xFFFF);
      if (0x0000u == variant)
      {
         log_info(&scd41_dev, "SCD41 sensor variant: SCD40\r\n");
      }
      else if (0x0001u == variant)
      {
         log_info(&scd41_dev, "SCD41 sensor variant: SCD41\r\n");
      }
      else if (0x0005u == variant)
      {
         log_info(&scd41_dev, "SCD41 sensor variant: SCD43\r\n");
      }
      else
      {
         log_info(&scd41_dev, "SCD41 sensor variant: unknown\r\n");
      }
   }
   while (0);

   return status;
}

static inline int8_t scd41_start_single_shot_measurement(I2C_TypeDef *dev)
{
   int8_t  status;
   uint8_t cmd[2] = {
      (uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD_41_CMD_MEASURE_SINGLE_SHOT, 8, 0xFFu),
      (uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD_41_CMD_MEASURE_SINGLE_SHOT, 0, 0xFFu),
   };

   status = I2C_Master_Write(dev, SCD41_I2C_ADDRESS, cmd, sizeof(cmd), I2C_DRV_TIMEOUT_MS);
   if (0 != status)
   {
      log_err(&scd41_dev, "SCD41 start single shot measurement error Status %d\r\n", status);
   }

   return status;
}

static inline int8_t scd41_start_single_shot_rht_measurement(I2C_TypeDef *dev)
{
   int8_t  status;
   uint8_t cmd[2] = {(uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD_41_CMD_MEASURE_SINGLE_SHOT_RHT_ONLY, 8,
                                                      0xFFu),
                     (uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD_41_CMD_MEASURE_SINGLE_SHOT_RHT_ONLY, 0,
                                                      0xFFu)};

   status = I2C_Master_Write(dev, SCD41_I2C_ADDRESS, cmd, sizeof(cmd), I2C_DRV_TIMEOUT_MS);
   if (0 != status)
   {
      log_err(&scd41_dev, "SCD41 start single shot RHT only measurement error Status %d\r\n",
              status);
   }

   return status;
}

static inline int8_t scd41_get_data_ready_status(I2C_TypeDef *dev, bool *data_ready)
{
   int8_t   status;
   uint8_t  response_status[3];
   uint16_t ready_status = 0u;

   do
   {
      if (NULL == data_ready)
      {
         status = -EINVAL;
         break;
      }
      status = I2C_Master_Reg16_Recessive_Bytes(dev, SCD41_I2C_ADDRESS,
                                                SCD_41_CMD_GET_DATA_READY_STATUS, response_status,
                                                sizeof(response_status), I2C_DRV_TIMEOUT_MS);
      if (0 != status)
      {
         log_err(&scd41_dev, "SCD41 read serial number error Status %d\r\n", status);
         break;
      }
      uint8_t crc = crc8_nrsc5(response_status, 2);
      if (crc != response_status[2])
      {
         log_err(&scd41_dev,
                 "SCD41_get_data_ready_status CRC error d1=0x%x d2=0x%x c1=0x%x rx_crc=0x%x\r\n",
                 response_status[0], response_status[1], crc, response_status[2]);
         status = -EBADMSG;
         break;
      }
      ready_status = (uint16_t) REUSE_SHIFT_LEFT_MASK(response_status[0], 8u, 0xFF)
          | (uint16_t) REUSE_SHIFT_LEFT_MASK(response_status[1], 0u, 0xFF);

      *data_ready = (REUSE_MASK(ready_status, 0x07FFu)) ? true : false;
   }
   while (0);
   return status;
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void SCD41_Init(I2C_TypeDef *dev)
{
   scd41_get_serial_numer(dev);
   scd41_get_sensor_variant(dev);
   TS_Delay_ms(10);
}

int8_t SCD41_PerformForcedRecalibration(I2C_TypeDef *dev, uint16_t co2_concentration)
{
   REUSE_UNUSED(co2_concentration);
   int8_t status;
   do
   {
      scd41_stop_periodic_measurement(dev);
      TS_Delay_ms(500);

      uint8_t cmd[2] = {(uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD41_CMD_PERFORM_FORCED_RECALIBRATION, 8,
                                                         0xFFu),
                        (uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD41_CMD_PERFORM_FORCED_RECALIBRATION, 0,
                                                         0xFFu)};

      status = I2C_Master_Write(dev, SCD41_I2C_ADDRESS, cmd, sizeof(cmd), I2C_DRV_TIMEOUT_MS);
      if (0 != status)
      {
         log_err(&scd41_dev, "SCD41 start single shot RHT only measurement error Status %d\r\n",
                 status);
      }

      TS_Delay_ms(400);

      uint8_t response_status[3];
      status =
          I2C_Master_Reg16_Recessive_Bytes(dev, SCD41_I2C_ADDRESS,
                                           SCD41_CMD_PERFORM_FORCED_RECALIBRATION, response_status,
                                           sizeof(response_status), I2C_DRV_TIMEOUT_MS);

      uint8_t crc = crc8_nrsc5(response_status, 2);
      if (crc != response_status[2])
      {
         log_err(&scd41_dev,
                 "SCD41_get_data_ready_status CRC error d1=0x%x d2=0x%x c1=0x%x rx_crc=0x%x\r\n",
                 response_status[0], response_status[1], crc, response_status[2]);
         status = -EBADMSG;
         break;
      }

      log_info(&scd41_dev, "SCD41 perform forced recalibration response: %02x %02x\r\n",
               response_status[0], response_status[1]);
   }
   while (0);
   return status;
}

int8_t SCD41_GetTemperatureBlocking(I2C_TypeDef *dev, int16_t *temperature)
{
   REUSE_UNUSED(temperature);
   int8_t status;
   status = scd41_start_single_shot_measurement(dev);
   TS_Delay_ms(1000); /* delay 1000 ms */
   return status;
}

int8_t SCD41_GetTemperatureUnblocking(I2C_TypeDef *dev, int16_t *temperature)
{
   int8_t retval = 0;
   bool   data_ready = false;

   do
   {
      if (NULL == temperature)
      {
         retval = -EINVAL;
         break;
      }
      scd41_get_data_ready_status(dev, &data_ready);
      log_info(&scd41_dev, "status %d\r\n", data_ready);

      // scd41_start_single_shot_measurement(dev);
   }
   while (0);

   return retval;
}

int8_t SCD41_GetRHTBlocking(I2C_TypeDef *dev, uint16_t *rh, int16_t *temperature)
{
   int8_t retval = 0;
   do
   {
      retval = scd41_start_single_shot_rht_measurement(dev);
      if (0 != retval)
      {
         log_err(&scd41_dev, "SCD41 start single shot RHT only measurement error Status %d\r\n",
                 retval);
         break;
      }
      TS_Delay_ms(100); /* delay 100 ms */
      retval = scd41_get_read_measurement(dev, NULL, rh, temperature);
      if (0 != retval)
      {
         log_err(&scd41_dev, "SCD41 get read measurement error Status %d\r\n", retval);
         break;
      }
   }
   while (0);
   return retval;
}

int8_t SCD41_GetCO2RHTBlocking(I2C_TypeDef *dev, uint16_t *co2, uint16_t *rh, int16_t *temperature)
{
   int8_t retval = 0;
   do
   {
      scd41_start_single_shot_measurement(dev);
      if (0 != retval)
      {
         log_err(&scd41_dev, "SCD41 start single shot CO2RHT measurement error Status %d\r\n",
                 retval);
         break;
      }

      TS_Delay_ms(5000); /* delay 5000 ms */
      retval = scd41_get_read_measurement(dev, co2, rh, temperature);
      if (0 != retval)
      {
         log_err(&scd41_dev, "SCD41 get read measurement error Status %d\r\n", retval);
         break;
      }
   }
   while (0);
   return retval;
}

int8_t SCD41_PerformFactoryResetBlocking(I2C_TypeDef *dev)
{
   int8_t status;
   do
   {
      uint8_t cmd[2] = {(uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD41_CMD_PERFOM_FACTORY_RESET, 8, 0xFFu),
                        (uint8_t) REUSE_SHIFT_RIGHT_MASK(SCD41_CMD_PERFOM_FACTORY_RESET, 0, 0xFFu)};

      status = I2C_Master_Write(dev, SCD41_I2C_ADDRESS, cmd, sizeof(cmd), I2C_DRV_TIMEOUT_MS);
      if (0 != status)
      {
         log_err(&scd41_dev, "SCD41 start single shot RHT only measurement error Status %d\r\n",
                 status);
      }

      TS_Delay_ms(1200);
   }
   while (0);
   return status;
}

int8_t SCD41_Task(I2C_TypeDef *dev)
{
   bool   data_ready = false;
   int8_t status;

   switch (scd41_state)
   {
   case SCD41_Initializing:
      status = scd41_get_serial_numer(dev);
      status |= scd41_get_sensor_variant(dev);
      if (0 != status)
      {
         scd41_state = SCD41_Starting_Measurement;
      }
      break;

   case SCD41_Starting_Measurement:
      status = scd41_start_single_shot_rht_measurement(dev);
      if (0 != status)
      {
         scd41_last_ts_ms = TS_Get_ms();
         scd41_state = SCD41_Waiting_For_Data;
      }
      break;

   case SCD41_Waiting_For_Data:
      if (TS_Get_ms() - scd41_last_ts_ms >= 50u)
      {
         scd41_state = SCD41_Reading_Data;
      }
      status = 0;
      break;

   case SCD41_Reading_Data:

      status = scd41_get_data_ready_status(dev, &data_ready);
      if ((0 == status) && (true == data_ready))
      {
         log_info(&scd41_dev, "SCD41 Data is ready\r\n");
      }
      else
      {
         log_info(&scd41_dev, "SCD41 Data is NOT ready\r\n");
      }
      scd41_state = SCD41_Starting_Measurement;
      break;
   }

   return status;
}