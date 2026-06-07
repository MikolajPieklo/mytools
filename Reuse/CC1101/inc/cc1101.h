/**
 ********************************************************************************
 * @file    cc1101.h
 * @author  Mikolaj Pieklo
 * @date    08.02.2026
 * @brief
 ********************************************************************************
 */

#ifndef __CC1101_H__
#define __CC1101_H__

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
#define CC1101_TX_ADDRESS 0x01u
#define CC1101_RX_ADDRESS 0x03u
#define CC1101_BR_ADDRESS 0x00u /* Broadcast address */

/************************************
 * TYPEDEFS
 ************************************/
typedef enum CC1101_Marc_State_Tag
{
   CC1101_STATE_SLEEP = 0x00,            /* SLEEP */
   CC1101_STATE_IDLE = 0x01,             /* IDLE */
   CC1101_STATE_XOFF = 0x02,             /* XOFF */
   CC1101_STATE_VCOON_MC = 0x03,         /* VCOON_MC */
   CC1101_STATE_REGON_MC = 0x04,         /* REGON_MC */
   CC1101_STATE_MANCAL = 0x05,           /* MANCAL */
   CC1101_STATE_VCOON = 0x06,            /* VCOON */
   CC1101_STATE_REGON = 0x07,            /* REGON */
   CC1101_STATE_STARTCAL = 0x08,         /* STARTCAL */
   CC1101_STATE_BWBOOST = 0x09,          /* BWBOOST */
   CC1101_STATE_FS_LOCK = 0x0A,          /* FS_LOCK */
   CC1101_STATE_IFADCON = 0x0B,          /* SETTLING */
   CC1101_STATE_ENDCAL = 0x0C,           /* CALIBRATE */
   CC1101_STATE_RX = 0x0D,               /* RX */
   CC1101_STATE_RX_END = 0x0E,           /* RX */
   CC1101_STATE_RX_RST = 0x0F,           /* RX */
   CC1101_STATE_TXRX_SWITCH = 0x10,      /* TXRX_SETTLING */
   CC1101_STATE_RXFIFO_OVERFLOW = 0x11,  /* RXFIFO_OVERFLOW */
   CC1101_STATE_FSTXON = 0x12,           /* FSTXON */
   CC1101_STATE_TX = 0x13,               /* TX */
   CC1101_STATE_TX_END = 0x14,           /* TX */
   CC1101_STATE_RXTX_SWITCH = 0x15,      /* RXTX_SETTLING */
   CC1101_STATE_TXFIFO_UNDERFLOW = 0x16, /* TXFIFO_UNDERFLOW */
   CC1101_STATE_UNKNOWN = 0x17
} CC1101_Marc_State_T;

#define CC1101_MARC_STATE_TO_STR(x)                                 \
   ((x) == CC1101_STATE_SLEEP                ? "SLEEP" :            \
        (x) == CC1101_STATE_IDLE             ? "IDLE" :             \
        (x) == CC1101_STATE_XOFF             ? "XOFF" :             \
        (x) == CC1101_STATE_VCOON_MC         ? "VCOON_MC" :         \
        (x) == CC1101_STATE_REGON_MC         ? "REGON_MC" :         \
        (x) == CC1101_STATE_MANCAL           ? "MANCAL" :           \
        (x) == CC1101_STATE_VCOON            ? "VCOON" :            \
        (x) == CC1101_STATE_REGON            ? "REGON" :            \
        (x) == CC1101_STATE_STARTCAL         ? "STARTCAL" :         \
        (x) == CC1101_STATE_BWBOOST          ? "BWBOOST" :          \
        (x) == CC1101_STATE_FS_LOCK          ? "FS_LOCK" :          \
        (x) == CC1101_STATE_IFADCON          ? "IFADCON" :          \
        (x) == CC1101_STATE_ENDCAL           ? "ENDCAL" :           \
        (x) == CC1101_STATE_RX               ? "RX" :               \
        (x) == CC1101_STATE_RX_END           ? "RX_END" :           \
        (x) == CC1101_STATE_RX_RST           ? "RX_RST" :           \
        (x) == CC1101_STATE_TXRX_SWITCH      ? "TXRX_SWITCH" :      \
        (x) == CC1101_STATE_RXFIFO_OVERFLOW  ? "RXFIFO_OVERFLOW" :  \
        (x) == CC1101_STATE_FSTXON           ? "FSTXON" :           \
        (x) == CC1101_STATE_TX               ? "TX" :               \
        (x) == CC1101_STATE_TX_END           ? "TX_END" :           \
        (x) == CC1101_STATE_RXTX_SWITCH      ? "RXTX_SWITCH" :      \
        (x) == CC1101_STATE_TXFIFO_UNDERFLOW ? "TXFIFO_UNDERFLOW" : \
        (x) == CC1101_STATE_UNKNOWN          ? "UNKNOWN" :          \
                                               "UNKNOWN")

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
void                CC1101_Init(uint8_t addr);
void                CC1101_Reset(void);
CC1101_Marc_State_T CC1101_Check_State(void);

uint8_t CC1101_Check_Available_Data(uint8_t pipenum);
void    CC1101_Tx_Mode(uint8_t *address, uint8_t channel);
void    CC1101_Rx_Mode(uint8_t *address, uint8_t channel);

void    CC1101_Debug_Init(void);
uint8_t CC1101_Debug_Tx(void);
uint8_t CC1101_Debug_Rx(void);

int8_t CC1101_Get_Voltage(int8_t *voltage);
int8_t CC1101_Get_Temperature(int8_t *temperature);

#ifdef __cplusplus
}
#endif

#endif
