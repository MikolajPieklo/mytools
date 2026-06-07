/**
 ********************************************************************************
 * @file    scd41.h
 * @author  Mikolaj Pieklo
 * @date    22.11.2025
 * @brief
 ********************************************************************************
 */

#ifndef __SCD41_H__
#define __SCD41_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include <stdint.h>
#ifdef STM32F103xB
#include <stm32f103xb.h>
#elif STM32F401xC
#include <stm32f401xc.h>
#else
#error Module not supported!
#endif
/************************************
 * MACROS AND DEFINES
 ************************************/

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void   SCD41_Init(I2C_TypeDef *dev);
int8_t SCD41_PerformForcedRecalibration(I2C_TypeDef *dev, uint16_t co2_concentration);

int8_t SCD41_GetTemperatureBlocking(I2C_TypeDef *dev, int16_t *temperature);
int8_t SCD41_GetTemperatureUnblocking(I2C_TypeDef *dev, int16_t *temperature);

int8_t SCD41_GetRHTBlocking(I2C_TypeDef *dev, uint16_t *rh, int16_t *temperature);
int8_t SCD41_GetCO2RHTBlocking(I2C_TypeDef *dev, uint16_t *co2, uint16_t *rh, int16_t *temperature);

int8_t SCD41_PerformFactoryResetBlocking(I2C_TypeDef *dev);

int8_t SCD41_Task(I2C_TypeDef *dev);

#ifdef __cplusplus
}
#endif

#endif
