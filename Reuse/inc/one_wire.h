/**
 ********************************************************************************
 * @file    one_wire.h
 * @author  Mikolaj Pieklo
 * @date    12.08.2025
 * @brief
 ********************************************************************************
 */

#ifndef __ONE_WIRE_H__
#define __ONE_WIRE_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/
#include <stdint.h>

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
void OneWire_Init(void);

int8_t OneWire_Read(uint8_t *tx, uint8_t tx_size, uint8_t *rx, uint8_t rx_size);

int8_t OneWire_Write(uint8_t *tx, uint8_t tx_size);

#ifdef __cplusplus
}
#endif

#endif