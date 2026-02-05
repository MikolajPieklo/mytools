/**
 ********************************************************************************
 * @file    spi.h
 * @author  Mikolaj Pieklo
 * @date    29.11.2024
 * @brief
 ********************************************************************************
 */

#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include <stdint.h>

#ifdef STM32F103xB
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
#elif STM32F401xC
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>
#else
#error Module not supported!
#endif

/************************************
 * MACROS AND DEFINES
 ************************************/
#define SPI1_CS1_Pin LL_GPIO_PIN_4
#define SPI1_CS2_Pin LL_GPIO_PIN_1

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void   SPI1_Init(void);
void   SPI2_Init(void);
int8_t SPI_Transfer(SPI_TypeDef *dev, uint32_t cs_pin, uint8_t *tx_data, uint8_t *rx_data,
                    uint8_t n);
int8_t SPI_TransferByte(SPI_TypeDef *dev, uint32_t cs_pin, uint8_t tx_data, uint8_t *rx_data);

#ifdef __cplusplus
}
#endif

#endif
