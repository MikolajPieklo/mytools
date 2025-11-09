#ifndef __CC1101_H__
#define __CC1101_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define CC1101_TX_ADDRESS 0x01
#define CC1101_RX_ADDRESS 0x03
#define CC1101_BR_ADDRESS 0x00 /* Broadcast address */

typedef enum CC1101_Marc_State_Tag
{
   CC1101_STATE_SLEEP = 0x00,            /* SLEEP */
   CC1101_STATE_IDLE = 0x01,             /* IDLE */
   CC1101_STATE_XOFF = 0x02,             /* XOFF */
   CC1101_STATE_VCOON_MC = 0x03,         /* MANCAL */
   CC1101_STATE_REGON_MC = 0x04,         /* MANCAL */
   CC1101_STATE_MANCAL = 0x05,           /* MANCAL MANCAL */
   CC1101_STATE_VCOON = 0x06,            /* FS_WAKEUP */
   CC1101_STATE_REGON = 0x07,            /* FS_WAKEUP */
   CC1101_STATE_STARTCAL = 0x08,         /* CALIBRATE */
   CC1101_STATE_BWBOOST = 0x09,          /* SETTLING */
   CC1101_STATE_FS_LOCK = 0x0A,          /* SETTLING */
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
   CC1101_STATE_UNKNOW = 0x17
} CC1101_Marc_State_T;

void                CC1101_Init(uint8_t addr);
void                CC1101_Reset(void);
CC1101_Marc_State_T CC1101_Check_State(void);
void                CC1101_Debug(void);
uint8_t             CC1101_Tx_Debug(void);
uint8_t             CC1101_Rx_Debug(void);

uint8_t CC1101_Check_Available_Data(uint8_t pipenum);
void    CC1101_Tx_Mode(uint8_t *address, uint8_t channel);
void    CC1101_Rx_Mode(uint8_t *address, uint8_t channel);


#ifdef __cplusplus
}
#endif
#endif /* __CC1101_H__ */
