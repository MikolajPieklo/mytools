/**
 ********************************************************************************
 * @file    lora_e32.h
 * @author  Mikolaj Pieklo
 * @date    10.10.2024
 * @brief
 ********************************************************************************
 */

#ifndef __LORA_E32_H__
#define __LORA_E32_H__

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

typedef enum Lora_Status_t
{
   Lora_Error = 0,
   Lora_Success = 1,
   Lora_Size
} Lora_Status;

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
Lora_Status Lora_Main_Thread(void);

uint32_t Lora_Get_Machine_State(void);

uint32_t Lora_Get_Tx_Counter(void);

uint32_t Lora_Get_Rx_Counter(void);

#ifdef __cplusplus
}
#endif

#endif