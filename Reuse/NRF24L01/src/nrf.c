/*
 * nrf.c
 *
 *  Created on: Dec 10, 2022
 *      Author: mkpk
 */


#include "nrf.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>

#include <delay.h>
#include <spi.h>

#define NRF_CE_Pin LL_GPIO_PIN_7

#define NRF_R_REG              0x00 /* OR */
#define NRF_W_REG              0x20 /* OR */
#define NRF_R_RX_PAYLOAD       0x61
#define NRF_W_TX_PAYLOAD       0xA0
#define NRF_FLUSH_TX           0xE1
#define NRF_FLUSH_RX           0xE2
#define NRF_REUSE_TX_PL        0xE3
#define NRF_R_RX_PL_WID        0x60
#define NRF_W_ACK_PAYLOAD      0xA8 /* OR */
#define NRF_W_TX_PAYLOAD_NOACK 0xB0
#define NRF_NOP                0xFF

#define NRF_REG_CONFIG      0x00
#define NRF_REG_EN_AA       0x01
#define NRF_REG_EN_RXADDR   0x02
#define NRF_REG_SETUP_AW    0x03
#define NRF_REG_SETUP_RETR  0x04
#define NRF_REG_RF_CH       0x05
#define NRF_REG_RF_SETUP    0x06
#define NRF_REG_RF_STATUS   0x07
#define NRF_REG_OBSERVE_TX  0x08
#define NRF_REG_RX_ADDR_P0  0x0A
#define NRF_REG_RX_ADDR_P1  0x0B
#define NRF_REG_RX_ADDR_P2  0x0C
#define NRF_REG_RX_ADDR_P3  0x0D
#define NRF_REG_RX_ADDR_P4  0x0E
#define NRF_REG_RX_ADDR_P5  0x0F
#define NRF_REG_TX_ADDR     0x10 /* 0xE7E7E7E7E7 */
#define NRF_REG_RX_PW_P0    0x11
#define NRF_REG_RX_PW_P1    0x12
#define NRF_REG_RX_PW_P2    0x13
#define NRF_REG_RX_PW_P3    0x14
#define NRF_REG_RX_PW_P4    0x15
#define NRF_REG_RX_PW_P5    0x16
#define NRF_REG_FIFO_STATUS 0x17

static void nrf24_enable(void)
{
   LL_GPIO_SetOutputPin(GPIOB, NRF_CE_Pin);
}

static void nrf24_disable(void)
{
   LL_GPIO_ResetOutputPin(GPIOB, NRF_CE_Pin);
}

static void nrf24_spi_cs_low(void)
{
   LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
}

static void nrf24_spi_cs_high(void)
{
   LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
}

static uint8_t spi_write(uint8_t data)
{
   LL_SPI_TransmitData8(SPI1, data);
   while (LL_SPI_IsActiveFlag_BSY(SPI1))
      ;
   return LL_SPI_ReceiveData8(SPI1);
}

static void spi_read(uint8_t reg, uint8_t *rxdata)
{
   uint8_t data = 0;
   LL_SPI_TransmitData8(SPI1, reg);
   while (LL_SPI_IsActiveFlag_BSY(SPI1))
      ;
   data = LL_SPI_ReceiveData8(SPI1);

   LL_SPI_TransmitData8(SPI1, 0xFF);
   while (LL_SPI_IsActiveFlag_BSY(SPI1))
      ;
   *rxdata = LL_SPI_ReceiveData8(SPI1);
}

static void nrf24_flush_rx(void)
{
   nrf24_spi_cs_low();
   spi_write(NRF_FLUSH_RX);
   nrf24_spi_cs_high();
}

static void nrf24_flush_tx(void)
{
   uint8_t data = 0;
   nrf24_spi_cs_low();
   spi_write(NRF_FLUSH_TX);
   nrf24_spi_cs_high();
}

static void nrf24_configure(void)
{
   TS_Delay_ms(100);
   uint8_t data = 0;

   nrf24_disable();

   /* PWR_UP, PTX */
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_CONFIG);
   spi_write(0x00);
   nrf24_spi_cs_high();

   /* disable auto ack */
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_EN_AA);
   spi_write(0x00);
   nrf24_spi_cs_high();

   /* disable rx pipes */
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_EN_RXADDR);
   spi_write(0x00);
   nrf24_spi_cs_high();

   /* 5 bytes for the TX/RX address */
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_SETUP_AW);
   spi_write(0x03);
   nrf24_spi_cs_high();

   /* No retransmission*/
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_SETUP_RETR);
   spi_write(0x00);
   nrf24_spi_cs_high();

   /* 2400 MHz*/
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_RF_CH);
   spi_write(0x00);
   nrf24_spi_cs_high();

   /* 250kbps 0dBm*/
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_RF_SETUP);
   spi_write(0x07); // 0x26
   nrf24_spi_cs_high();

   nrf24_enable();
   TS_Delay_ms(1);
}

void NRF24_TxMode(uint8_t *address, uint8_t channel)
{
   uint8_t data = 0;
   nrf24_disable();

   /* 2400 +MHz*/
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_RF_CH);
   spi_write(channel);
   nrf24_spi_cs_high();

   // Write the Tx address
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_TX_ADDR);
   spi_write(address[0]);
   spi_write(address[1]);
   spi_write(address[2]);
   spi_write(address[3]);
   spi_write(address[4]);
   nrf24_spi_cs_high();

   // read config
   uint8_t config = 0;
   nrf24_spi_cs_low();
   spi_read(NRF_R_REG | NRF_REG_CONFIG, &config);
   nrf24_spi_cs_high();

   config |= 0x02;

   // PowerUP
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_CONFIG);
   spi_write(config);
   nrf24_spi_cs_high();

   nrf24_flush_tx();
   nrf24_flush_rx();

   nrf24_enable();
   TS_Delay_ms(1);
}

void NRF24_RxMode(uint8_t *address, uint8_t channel)
{
   uint8_t data = 0;
   uint8_t reg = 0;
   nrf24_disable();

   /* 2400 + 50 MHz*/
   //   nrf24_spi_cs_low();
   //   spi_write(NRF_W_REG | NRF_REG_RF_CH);
   //   spi_write(channel);
   //   nrf24_spi_cs_high();

   //   //read reg
   //   nrf24_spi_cs_low();
   //   spi_read(NRF_R_REG | NRF_REG_EN_RXADDR, &reg);
   //   nrf24_spi_cs_high();
   //
   //   reg |= (1<<0);
   //
   //   /* Enable pipe 0*/
   //   nrf24_spi_cs_low();
   //   spi_write(NRF_W_REG | NRF_REG_EN_RXADDR);
   //   spi_write(0x3F);//reg
   //   nrf24_spi_cs_high();

   // Write the Rx address for pipe 0
   //   nrf24_spi_cs_low();
   //   spi_write(NRF_W_REG | NRF_REG_RX_ADDR_P0);
   //   spi_write(address[0]);
   //   spi_write(address[1]);
   //   spi_write(address[2]);
   //   spi_write(address[3]);
   //   spi_write(address[4]);
   //   nrf24_spi_cs_high();

   /* Set pipe 0 size */
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_RX_PW_P0);
   spi_write(32);
   nrf24_spi_cs_high();

   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_RX_PW_P1);
   spi_write(32);
   nrf24_spi_cs_high();

   // read config
   uint8_t config = 0;

   nrf24_spi_cs_low();
   spi_read(NRF_R_REG | NRF_REG_CONFIG, &config);
   nrf24_spi_cs_high();

   config |= ((1 << 1) | (1 << 0));

   // PowerUP in rx mode
   nrf24_spi_cs_low();
   spi_write(NRF_W_REG | NRF_REG_CONFIG);
   spi_write(config);
   nrf24_spi_cs_high();

   //   nrf24_flush_tx();
   //   nrf24_flush_rx();

   nrf24_enable();
   TS_Delay_ms(1);
}

void nRF24_Init(void)
{
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

   LL_GPIO_SetPinMode(GPIOB, NRF_CE_Pin, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinSpeed(GPIOB, NRF_CE_Pin, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOB, NRF_CE_Pin, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(GPIOB, NRF_CE_Pin, LL_GPIO_PULL_DOWN);
   nrf24_disable();

   TS_Delay_ms(50);
   nrf24_configure();
   TS_Delay_ms(10);
}

void nRF24_Debug(void)
{
   uint8_t data = 0;

   nrf24_spi_cs_low();
   spi_read((NRF_R_REG | 0x06), &data);
   nrf24_spi_cs_high();
   TS_Delay_ms(1);
}

uint8_t nRF24_isDataAvailable(uint8_t pipenum)
{
   uint8_t data = 0;
   uint8_t status = 0;

   nrf24_spi_cs_low();
   spi_read(NRF_R_REG | NRF_REG_RF_STATUS, &status);
   nrf24_spi_cs_high();

   if ((status & (1 << 6)) && (status & (pipenum << 1)))
   {
      // clear int
      nrf24_spi_cs_low();
      spi_write(NRF_W_REG | NRF_REG_RF_STATUS);
      spi_write((1 << 6));
      nrf24_spi_cs_high();

      return 1;
   }
   return 0;
}

uint8_t nRF24_Tx_Debug(void)
{
   uint8_t data = 0;
   uint8_t fifostatus = 0;

   nrf24_spi_cs_low();
   spi_write(NRF_W_TX_PAYLOAD);

   uint8_t i = 0;
   while (i < 32)
   {
      spi_write(i);
      i++;
   }
   nrf24_spi_cs_high();

   TS_Delay_ms(1);

   // check fifo status
   nrf24_spi_cs_low();
   spi_read((NRF_R_REG | NRF_REG_FIFO_STATUS), &fifostatus);
   nrf24_spi_cs_high();

   if ((fifostatus & (1 << 4)) && (!(fifostatus & (1 << 3))))
   {
      nrf24_flush_tx();
      return 1;
   }
   return 0;
}

void nRF24_Rx_Debug(uint8_t *data)
{
   uint8_t fifostatus = 0;
   nrf24_spi_cs_low();
   spi_write(NRF_R_RX_PAYLOAD);

   uint8_t i = 0;
   while (i < 32)
   {
      LL_SPI_TransmitData8(SPI1, 0xFF);
      while (LL_SPI_IsActiveFlag_BSY(SPI1))
         ;
      data[i] = LL_SPI_ReceiveData8(SPI1);
      i++;
   }
   nrf24_spi_cs_high();
   TS_Delay_ms(1);

   // check fifo status
   nrf24_spi_cs_low();
   spi_write(NRF_FLUSH_RX);
}
