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

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
/************************************
 * MACROS AND DEFINES
 ************************************/
#define SPI1_CS1_Pin  LL_GPIO_PIN_4
#define SPI1_CS2_Pin  LL_GPIO_PIN_1
#define SPI1_SCK_Pin  LL_GPIO_PIN_5
#define SPI1_MISO_Pin LL_GPIO_PIN_6
#define SPI1_MOSI_Pin LL_GPIO_PIN_7
#define SPI2_SCK_Pin  LL_GPIO_PIN_13
#define SPI2_MOSI_Pin LL_GPIO_PIN_15

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void    SPI1_Init(void);
void    SPI2_Init(void);
uint8_t SPI_Transfer(SPI_TypeDef *dev, uint32_t cs_pin, uint8_t *tx_data, uint8_t *rx_data,
                     uint8_t n);

#ifdef __cplusplus
}
#endif

#endif
