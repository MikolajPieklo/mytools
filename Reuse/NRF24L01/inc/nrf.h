#ifndef __NRF_H__
#define __NRF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void    nRF24_Init(void);
void    nRF24_Debug(void);
uint8_t nRF24_Tx_Debug(void);
uint8_t nRF24_isDataAvailable(uint8_t pipenum);
void    nRF24_Rx_Debug(uint8_t *data);
void    NRF24_TxMode(uint8_t *address, uint8_t channel);
void    NRF24_RxMode(uint8_t *address, uint8_t channel);


#ifdef __cplusplus
}
#endif
#endif /* __NRF_H__ */
