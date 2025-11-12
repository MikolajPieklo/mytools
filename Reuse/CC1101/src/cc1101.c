/*
 * cc1101.c
 *
 *  Created on: Dec 18, 2022
 *      Author: mkpk
 */

#include "cc1101.h"

#include <stdio.h>

#ifdef STM32F103xB
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
#elif STM32F401xC
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>
#else
#error Module not supported!
#endif


#include "cc1101_reg.h"
#include <delay.h>
#include <log.h>
#include <spi.h>
#include <string.h>

/* Dummy device */
static const struct device cc1101_dev = {
   .name = "CC1101",
};

#define CC1101_GDO0_Pin LL_GPIO_PIN_7 // GPIOB
#define CC1101_GDO2_Pin LL_GPIO_PIN_6 // GPIOB

#define TXPower     0xC0
#define ChannelRF   0x01
#define RSSI_OFFSET 74

#define CRYSTAL_FREQUENCY   26000
#define CRYSTAL_FREQUENCY_M CRYSTAL_FREQUENCY / 1000

static void    cc1101_spi_cs_low(void);
static void    cc1101_spi_cs_high(void);
static uint8_t spi_write(uint8_t *txdata, uint8_t *rxdata, size_t n);
static void    spi_read(uint8_t reg, uint8_t *rxdata);
static uint8_t cc1101_write_reg(uint8_t reg, uint8_t value);
static uint8_t cc1101_write_burst_reg(uint8_t reg, uint8_t *data, uint8_t size);
static uint8_t cc1101_strobe(uint8_t command);
static uint8_t cc1101_read_reg(uint8_t reg);
static uint8_t cc1101_read_burst_reg(uint8_t reg, uint8_t *data, uint8_t size);
static float   rf_set_carrier_frequency(float target_freq);
static uint8_t cc1101_set_ism(void);
static uint8_t cc1101_set_channel(void);
static uint8_t cc1101_set_output_power(int8_t dBm);
static int8_t  cc1101_get_rssi(void);
static uint8_t cc1101_get_lqi(void);

void CC1101_Init(uint8_t addr)
{
#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
#elif STM32F401xC
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#endif

   LL_GPIO_SetPinMode(GPIOB, CC1101_GDO0_Pin, LL_GPIO_MODE_INPUT);
   LL_GPIO_SetPinMode(GPIOB, CC1101_GDO2_Pin, LL_GPIO_MODE_INPUT);

   uint8_t rxdata = 0;
   CC1101_Reset();

   cc1101_strobe(CC1101_C_SFTX);
   TS_Delay_ms(100);
   cc1101_strobe(CC1101_C_SFRX);
   TS_Delay_ms(100);

   rxdata = cc1101_read_reg(CC1101_R_PARTNUM);
   log_info(&cc1101_dev, "PARTNUM = 0x%x\r\n", rxdata);

   rxdata = cc1101_read_reg(CC1101_R_VERSION);
   log_info(&cc1101_dev, "VERSION = 0x%x\r\n", rxdata);

   CC1101_Check_State();

   // uint8_t PA[] =  {TXPower,TXPower,TXPower,TXPower,TXPower,TXPower,TXPower,TXPower};
   uint8_t       PA[] = {0x6C, 0x1C, 0x06, 0x3A, 0x51, 0x85, 0xC8, 0xC0};
   const uint8_t PA_LEN = 8;
   cc1101_write_reg(CC1101_R_IOCFG2, 0x07);            // GDO2 Output Pin Configuration
   cc1101_write_reg(CC1101_R_IOCFG1, 0x2E);            // GDO1 Output Pin Configuration
   cc1101_write_reg(CC1101_R_IOCFG0, 0x80);            // GDO0_Pin Output Pin Configuration
   cc1101_write_reg(CC1101_R_FIFOTHR, 0x07);           // RX FIFO and TX FIFO Thresholds
   cc1101_write_reg(CC1101_R_SYNC1, 0x57);             // Sync Word, High Byte
   cc1101_write_reg(CC1101_R_SYNC0, 0x43);             // Sync Word, Low Byte
   cc1101_write_reg(CC1101_R_PKTLEN, 0x3E);            // Packet Length
   cc1101_write_reg(CC1101_R_PKTCTRL1, 0x06);          // Packet Automation Control
   cc1101_write_reg(CC1101_R_PKTCTRL0, 0x45);          // Packet Automation Control
   cc1101_write_reg(CC1101_R_ADDR, addr);              // Device Address
   cc1101_write_reg(CC1101_R_CHANNR, 0x01);            // Channel Number
   cc1101_write_reg(CC1101_R_FSCTRL1, 0x08);           // Frequency Synthesizer Control
   cc1101_write_reg(CC1101_R_FSCTRL0, 0x00);           // Frequency Synthesizer Control
   cc1101_write_reg(CC1101_R_FREQ2, 0x21);             // Frequency Control Word, High Byte
   cc1101_write_reg(CC1101_R_FREQ1, 0x65);             // Frequency Control Word, Middle Byte
   cc1101_write_reg(CC1101_R_FREQ0, 0x6A);             // Frequency Control Word, Low Byte
   cc1101_write_reg(CC1101_R_MDMCFG4, /*.0x5B*/ 0xF5); // Modem Configuration
   cc1101_write_reg(CC1101_R_MDMCFG3, /*.0xF8*/ 0x83); // Modem Configuration
   cc1101_write_reg(CC1101_R_MDMCFG2, 0x13);           // Modem Configuration
   cc1101_write_reg(CC1101_R_MDMCFG1, 0xA0 /*0xC0*/);  // Modem Configuration
   cc1101_write_reg(CC1101_R_MDMCFG0, 0xF8);           // Modem Configuration
   cc1101_write_reg(CC1101_R_DEVIATN, /*.0x47*/ 0x15); // Modem Deviation Setting
   cc1101_write_reg(CC1101_R_MCSM2, 0x07); // Main Radio Control State Machine Configuration
   cc1101_write_reg(CC1101_R_MCSM1,
                    /*..0x0C*/ 0x00);      // Main Radio Control State Machine Configuration
   cc1101_write_reg(CC1101_R_MCSM0, 0x18); // Main Radio Control State Machine Configuration
   cc1101_write_reg(CC1101_R_FOCCFG, /*.0x1D*/ 0x16); // Frequency Offset Compensation Configuration
   cc1101_write_reg(CC1101_R_BSCFG, /*.0x1C*/ 0x6C);  // Bit Synchronization Configuration
   cc1101_write_reg(CC1101_R_AGCTRL2, /*.0xC7*/ 0x03); // AGC Control
   cc1101_write_reg(CC1101_R_AGCTRL1, /*.0x00*/ 0x40); // AGC Control
   cc1101_write_reg(CC1101_R_AGCTRL0, /*.0xB2*/ 0x91); // AGC Control
   cc1101_write_reg(CC1101_R_WOREVT1, 0x02);           // High Byte Event0 Timeout
   cc1101_write_reg(CC1101_R_WOREVT0, 0x26);           // Low Byte Event0 Timeout
   cc1101_write_reg(CC1101_R_WORCTRL, 0x09);           // Wake On Radio Control
   cc1101_write_reg(CC1101_R_FREND1, /*.0xB6*/ 0x56);  // Front End RX Configuration
   cc1101_write_reg(CC1101_R_FREND0, 0x17);            // Front End TX Configuration
   cc1101_write_reg(CC1101_R_FSCAL3, /*.0xEA*/ 0xA9);  // Frequency Synthesizer Calibration
   cc1101_write_reg(CC1101_R_FSCAL2, 0x0A);            // Frequency Synthesizer Calibration
   cc1101_write_reg(CC1101_R_FSCAL1, 0x00);            // Frequency Synthesizer Calibration
   cc1101_write_reg(CC1101_R_FSCAL0, 0x11);            // Frequency Synthesizer Calibration
   cc1101_write_reg(CC1101_R_RCCTRL1, 0x41);           // RC Oscillator Configuration
   cc1101_write_reg(CC1101_R_RCCTRL0, 0x00);           // RC Oscillator Configuration
   cc1101_write_reg(CC1101_R_FSTEST, 0x59);            // Frequency Synthesizer Calibration Control
   cc1101_write_reg(CC1101_R_PTEST, 0x7F);             // Production Test
   cc1101_write_reg(CC1101_R_AGCTEST, 0x3F);           // AGC Test
   cc1101_write_reg(CC1101_R_TEST2, 0x81);             // Various Test Settings
   cc1101_write_reg(CC1101_R_TEST1, 0x3F);             // Various Test Settings
   cc1101_write_reg(CC1101_R_TEST0, 0x0B);             // Various Test Settings

   cc1101_write_burst_reg(CC1101_C_PATABLE, PA, PA_LEN);
   cc1101_set_ism();
   cc1101_set_channel();
   cc1101_set_output_power(0);

   CC1101_Check_State();

   cc1101_strobe(CC1101_C_SRX);
   CC1101_Check_State();
   while (CC1101_STATE_STARTCAL == cc1101_read_reg(CC1101_R_MARCSTATE))
      ;
   CC1101_Check_State();
   while (CC1101_STATE_RX != cc1101_read_reg(CC1101_R_MARCSTATE))
      ;
}

void CC1101_Reset(void)
{
   uint32_t counter = 0;
   cc1101_strobe(CC1101_C_SRES);
   TS_Delay_ms(1);
   while (cc1101_strobe(CC1101_C_SNOP) & 0x80)
   {
      counter++;
   }
   log_info(&cc1101_dev, "counter %ld\r\n", counter);
}

/*
+------+---------------------------+---------------------------------------------------------------------------------------------------------------------+
| Bits | Name                      | Description |
+------+---------------------------+---------------------------------------------------------------------------------------------------------------------+
| 7    | CHIP_RDYn                 | Stays high until power and crystal have stabilized. Should
always be low when using                                 | |      |                           | the
SPI interface. |
+------+---------------------------+---------------------------------------------------------------------------------------------------------------------+
| 6:4  | STATE[2:0]                | Indicates the current main state machine mode | |      | |
Value   State              Description | |      |                           | 000     IDLE IDLE
state (Also reported for some transitional states instead of SETTLING or CALIBRATE) | |      | | 001
RX                 Receive mode | |      |                           | 010     TX Transmit mode | |
|                           | 011     FSTXON             Fast TX ready | |      | | 100 CALIBRATE
Frequency synthesizer calibration is running                                             | |      |
| 101     SETTLING           PLL is settling | |      |                           | 110
RXFIFO_OVERFLOWRX  FIFO has overflowed. Read out any useful data, then flush the FIFO with SFRX | |
|                           | 111     TXFIFO_UNDERFLOWTX FIFO has underflowed. Acknowledge with SFTX
|
+------+---------------------------+---------------------------------------------------------------------------------------------------------------------+
| 3:0  | FIFO_BYTES_AVAILABLE[3:0] | The number of bytes available in the RX FIFO or free bytes in
the TX FIFO                                           |
+------+---------------------------+---------------------------------------------------------------------------------------------------------------------+
*/

CC1101_Marc_State_T CC1101_Check_State(void)
{
   static CC1101_Marc_State_T status = CC1101_STATE_UNKNOW;
   static uint8_t             counter = 0;
   uint8_t                    data[2];

   cc1101_spi_cs_low();
   spi_read(CC1101_READ_BURST | CC1101_R_MARCSTATE, data);
   cc1101_spi_cs_high();
   counter++;

   if ((status != data[1]) || (counter % 20 == 0))
   {
      status = data[1];

      switch (data[1])
      {
      case CC1101_STATE_SLEEP:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: SLEEP | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_IDLE:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: IDLE | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_XOFF:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: XOFF | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_VCOON_MC:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: VCOON_MC | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_REGON_MC:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: REGON_MC | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_MANCAL:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: MANCAL | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_VCOON:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: VCOON | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_REGON:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: REGON | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_STARTCAL:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: STARTCAL | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_BWBOOST:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: BWBOOST | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_FS_LOCK:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: FS_LOCK | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_IFADCON:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: IFADCON | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_ENDCAL:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: ENDCAL | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_RX:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: RX | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_RX_END:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: RX_END | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_RX_RST:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: RX_RST | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_TXRX_SWITCH:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: TXRX_SWITCH | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_RXFIFO_OVERFLOW:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: RXFIFO_OVERFLOW | STATUS BYTE = 0x%x\r\n",
                  data[0]);
         break;
      case CC1101_STATE_FSTXON:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: FSTXON | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_TX:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: TX | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_TX_END:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: TX_END | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_RXTX_SWITCH:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: RXTX_SWITCH | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      case CC1101_STATE_TXFIFO_UNDERFLOW:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: TXFIFO_UNDERFLOW | STATUS BYTE = 0x%x\r\n",
                  data[0]);
         break;
      default:
         log_info(&cc1101_dev, "CC1101_MARCSTATE: UNKNOW | STATUS BYTE = 0x%x\r\n", data[0]);
         break;
      }
   }
   return data[1];
}

uint8_t CC1101_Tx_Debug(void)
{
   static uint8_t counter;

   if (counter % 10 == 0)
   {
      uint8_t txdata[] = "   HelloWorld\0";
      uint8_t len = strlen((char *) txdata);

      log_info(&cc1101_dev, "value[0]: %d\r\n", txdata[0]);
      log_info(&cc1101_dev, "size: %d\r\n", strlen((char *) txdata));
      txdata[0] = len;
      txdata[1] = CC1101_BR_ADDRESS;
      txdata[2] = CC1101_TX_ADDRESS;

      CC1101_Check_State();

      cc1101_strobe(CC1101_C_SIDLE);
      while (CC1101_STATE_IDLE != cc1101_read_reg(CC1101_R_MARCSTATE))
         ;

      cc1101_write_burst_reg(CC1101_R_TX_FIFO, txdata, len + 1); // Write TX data

      cc1101_strobe(CC1101_C_STX); // Change state to TX, initiating
      CC1101_Check_State();
      while (CC1101_STATE_STARTCAL == cc1101_read_reg(CC1101_R_MARCSTATE))
         ;
      CC1101_Check_State();
      while (CC1101_STATE_TX == cc1101_read_reg(CC1101_R_MARCSTATE))
         ;
      CC1101_Check_State();
      while (CC1101_STATE_TX_END == cc1101_read_reg(CC1101_R_MARCSTATE))
         ;
      CC1101_Check_State();
      while (CC1101_STATE_IDLE != cc1101_read_reg(CC1101_R_MARCSTATE))
         ;
   }

   counter++;
   return 1;
}

uint8_t CC1101_Rx_Debug(void)
{
   uint32_t GDO2_Pin = 0;
   uint8_t  bytes_in_rxfifo = 0;
   uint8_t  rxFifo[50];
   uint8_t  i = 0;

   if (CC1101_STATE_RX_END == cc1101_read_reg(CC1101_R_MARCSTATE))
   {
      log_info(&cc1101_dev, "Message is recived\r\n");
   }

   GDO2_Pin = (LL_GPIO_ReadInputPort(GPIOB) & 0x40);

   if (GDO2_Pin != 0)
   {
      bytes_in_rxfifo = cc1101_read_reg(CC1101_R_RXBYTES);
      log_info(&cc1101_dev, "Bytes in rxfifo 0x%x\r\n", bytes_in_rxfifo);

      cc1101_read_burst_reg(CC1101_R_RX_FIFO, rxFifo, bytes_in_rxfifo);

      for (i = 0; i < bytes_in_rxfifo; i++)
      {
         log_info(&cc1101_dev, "%x ", rxFifo[i]);
      }
      log_info(&cc1101_dev, "\r\n");

      log_info(&cc1101_dev, "LQI: 0x%x RSSI: %d\r\n", cc1101_get_lqi(), cc1101_get_rssi());

      cc1101_strobe(CC1101_C_SRX);
      CC1101_Check_State();
      while (CC1101_STATE_STARTCAL == cc1101_read_reg(CC1101_R_MARCSTATE))
         ;
      CC1101_Check_State();
      while (CC1101_STATE_RX != cc1101_read_reg(CC1101_R_MARCSTATE))
         ;
   }
   return 0;
}

static void cc1101_spi_cs_low(void)
{
   LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
}

static void cc1101_spi_cs_high(void)
{
   LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
}

static uint8_t spi_write(uint8_t *txdata, uint8_t *rxdata, size_t n)
{
   memset(rxdata, 0, n);
   size_t i = 0;
   while (i < n)
   {
      LL_SPI_TransmitData8(SPI1, txdata[i]);
      while (LL_SPI_IsActiveFlag_BSY(SPI1))
         ;
      rxdata[i] = LL_SPI_ReceiveData8(SPI1);
      i++;
   }

   return 0;
}

static void spi_read(uint8_t reg, uint8_t *rxdata)
{
   LL_SPI_TransmitData8(SPI1, reg);
   while (LL_SPI_IsActiveFlag_BSY(SPI1))
      ;
   rxdata[0] = LL_SPI_ReceiveData8(SPI1);

   LL_SPI_TransmitData8(SPI1, 0xFF);
   while (LL_SPI_IsActiveFlag_BSY(SPI1))
      ;
   rxdata[1] = LL_SPI_ReceiveData8(SPI1);
}

static uint8_t cc1101_write_reg(uint8_t reg, uint8_t value)
{
   uint8_t rxdata[2] = {0, 0};
   uint8_t txdata[2] = {CC1101_WRITE_SINGLE_BYTE | reg, value};

   cc1101_spi_cs_low();
   spi_write(txdata, rxdata, 2);
   cc1101_spi_cs_high();
   return rxdata[1];
}

static uint8_t cc1101_write_burst_reg(uint8_t reg, uint8_t *data, uint8_t size)
{
   uint8_t rxdata[20];
   uint8_t reg2 = reg | CC1101_WRITE_BURST;

   cc1101_spi_cs_low();
   spi_write(&reg2, rxdata, 1);
   spi_write(data, rxdata, size);
   cc1101_spi_cs_high();
   return rxdata[size - 1];
}

static uint8_t cc1101_strobe(uint8_t command)
{
   uint8_t rxdata = 0;
   uint8_t txdata = CC1101_WRITE_SINGLE_BYTE | command;

   cc1101_spi_cs_low();
   spi_write(&txdata, &rxdata, 1);
   cc1101_spi_cs_high();
   return rxdata;
}

static uint8_t cc1101_read_reg(uint8_t reg)
{
   cc1101_spi_cs_low();
   uint8_t rxdata[2] = {0, 0};
   spi_read(CC1101_READ_BURST | reg, rxdata);
   cc1101_spi_cs_high();
   return rxdata[1];
}

static uint8_t cc1101_read_burst_reg(uint8_t reg, uint8_t *data, uint8_t size)
{
   uint8_t reg2 = reg | CC1101_READ_BURST;
   uint8_t tx_data[size];
   memset(tx_data, 0, size);

   cc1101_spi_cs_low();
   spi_write(&reg2, data, 1);
   spi_write(tx_data, data, size);
   cc1101_spi_cs_high();
   return data[0];
}

float rf_set_carrier_frequency(float target_freq)
{
   /* Note that this functions depends on the value of CRYSTAL_FREQUENCY_M.
    * @param target_freq Frequency targeted, in MHz. Positive number. Note that the actual frequency
    * may vary.
    * @return Actual configured frequency.
    */
   target_freq = target_freq * 1000000;
   float    freqf = target_freq * 65536.0 / (float) CRYSTAL_FREQUENCY_M;
   uint32_t freq = (uint32_t) freqf;
   freq = freq & 0x00FFFFFF;
   cc1101_write_reg(CC1101_R_FREQ0, freq);
   cc1101_write_reg(CC1101_R_FREQ1, (freq >> 8));
   cc1101_write_reg(CC1101_R_FREQ2, (freq >> 16));
   float t = ((float) freq * (float) CRYSTAL_FREQUENCY_M) / 65536.0;

   return t;
}

static uint8_t cc1101_set_ism(void)
{
   uint8_t freq2 = 0x10;
   uint8_t freq1 = 0xB0;
   uint8_t freq0 = 0x71;

   cc1101_write_reg(CC1101_R_FREQ2, freq2);
   cc1101_write_reg(CC1101_R_FREQ1, freq1);
   cc1101_write_reg(CC1101_R_FREQ0, freq0);

   return 0;
}

static uint8_t cc1101_set_channel(void)
{
   cc1101_write_reg(CC1101_R_CHANNR, 0x01);
   return 0;
}

static uint8_t cc1101_set_output_power(int8_t dBm)
{
   uint8_t pa = 0x04;
   if (dBm <= -30)
   {
      pa = 0x00;
   }
   else if (dBm <= -20)
   {
      pa = 0x01;
   }
   else if (dBm <= -15)
   {
      pa = 0x02;
   }
   else if (dBm <= -10)
   {
      pa = 0x03;
   }
   else if (dBm <= 0)
   {
      pa = 0x04;
   }
   else if (dBm <= 5)
   {
      pa = 0x05;
   }
   else if (dBm <= 7)
   {
      pa = 0x06;
   }
   else if (dBm <= 10)
   {
      pa = 0x07;
   }

   cc1101_write_reg(CC1101_R_FREND0, pa);
   return 0;
}

static int8_t cc1101_get_rssi(void)
{
   uint8_t rxData = 0;
   int8_t  rssi_dbm = 0;
   int16_t Rssi_dec = 0;
   cc1101_read_burst_reg(CC1101_R_RSSI, &rxData, 1);
   Rssi_dec = rxData;

   if (Rssi_dec >= 128)
   {
      rssi_dbm = ((Rssi_dec - 256) / 2) - RSSI_OFFSET;
   }
   else
   {
      rssi_dbm = ((Rssi_dec) / 2) - RSSI_OFFSET;
   }
   return rssi_dbm;
}

static uint8_t cc1101_get_lqi(void)
{
   uint8_t rxData = 0;
   cc1101_read_burst_reg(CC1101_R_LQI, &rxData, 1);
   return rxData;
}
