#ifndef __SI4432_H__
#define __SI4432_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum RadioStatus
{
   RadioStatusError = 0,
   RadioStatusOk = 1
} RadioStatus_t;

RadioStatus_t SI4432_Init(void);

void SI4432_Reset(void);

void SI4432_Debug(void);

uint8_t SI4432_Tx_Debug(void);

uint8_t SI4432_isDataAvailable(uint8_t pipenum);

void SI4432_Rx_Debug(void);

void SI4432_TxMode(uint8_t *address, uint8_t channel);

void SI4432_RxMode(void);

void SI4432_ClearTxFifo(void);

void SI4432_ClearRxFifo(void);

#ifdef __cplusplus
}
#endif
#endif /* __SI4432_H__ */
