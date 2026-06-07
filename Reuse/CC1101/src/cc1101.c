/**
 ********************************************************************************
 * @file    cc1101.c
 * @author  Mikolaj Pieklo
 * @date    08.02.2026
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
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

/************************************
 * EXTERN VARIABLES
 ************************************/
/* Dummy device */
static const struct device cc1101_dev = {
   .name = "CC1101",
};
/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define CC1101_GDO0_Pin LL_GPIO_PIN_7 // GPIOB
#define CC1101_GDO2_Pin LL_GPIO_PIN_6 // GPIOB

#define TXPower     0xC0
#define ChannelRF   0x01
#define RSSI_OFFSET 74

#define CC1101_DBG_PROFILE_TAG       "RPI-2026-06-07-143517"
#define CC1101_DBG_PKTCTRL1_PROFILE  0x04
#define CC1101_DBG_PKTCTRL0_PROFILE  0x05
#define CC1101_DBG_MDMCFG2_PROFILE   0x13
#define CC1101_DBG_MDMCFG2_FALLBACK  0x12
#define CC1101_DBG_MDMCFG2_FALLBACK2 0x11
#define CC1101_DBG_MDMCFG1_PROFILE   0x22
#define CC1101_DBG_ADDR_PROFILE      0xFF

#define CRYSTAL_FREQUENCY   26000
#define CRYSTAL_FREQUENCY_M CRYSTAL_FREQUENCY / 1000
/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static uint8_t cc1101_write_reg(uint8_t reg, uint8_t value);
static uint8_t cc1101_write_burst_reg(uint8_t reg, uint8_t *data, uint8_t size);
static uint8_t cc1101_cmd_strobe(uint8_t command);
static uint8_t cc1101_read_reg(uint8_t reg);
static uint8_t cc1101_read_burst_reg(uint8_t reg, uint8_t *data, uint8_t size);
static float   rf_set_carrier_frequency(float target_freq);
static uint8_t cc1101_set_ism(void);
static uint8_t cc1101_set_channel(void);
static uint8_t cc1101_set_output_power(int8_t dBm);
static int8_t  cc1101_get_rssi(void);
static uint8_t cc1101_get_lqi(void);
static void    cc1101_log_hex_buffer(const uint8_t *buffer, uint8_t length);
static void    cc1101_log_ascii_buffer(const uint8_t *buffer, uint8_t length);

/************************************
 * STATIC FUNCTIONS
 ************************************/
static uint8_t cc1101_write_reg(uint8_t reg, uint8_t value)
{
   uint8_t rxdata[2] = {0, 0};
   uint8_t txdata[2] = {CC1101_WRITE_SINGLE_BYTE | reg, value};

   SPI_Transfer(SPI1, LL_GPIO_PIN_4, txdata, rxdata, 2);

   return rxdata[1];
}

static uint8_t cc1101_write_burst_reg(uint8_t reg, uint8_t *data, uint8_t size)
{
   uint8_t rxdata[64];
   uint8_t txdata[64];
   txdata[0] = reg | CC1101_WRITE_BURST;
   memcpy(txdata + 1, data, size);

   SPI_Transfer(SPI1, LL_GPIO_PIN_4, txdata, rxdata, size + 1);
   return rxdata[size - 1];
}

/* This function is used for changing the state of the CC1101, e.g. to enter RX or TX mode. */
static uint8_t cc1101_cmd_strobe(uint8_t command)
{
   uint8_t rxdata = 0;
   uint8_t txdata = CC1101_WRITE_SINGLE_BYTE | command;

   SPI_Transfer(SPI1, LL_GPIO_PIN_4, &txdata, &rxdata, 1);
   return rxdata;
}

static uint8_t cc1101_read_reg(uint8_t reg)
{
   uint8_t rxdata[2] = {0, 0};
   uint8_t txdata[2] = {CC1101_READ_BURST | reg, 0xFFu};
   SPI_Transfer(SPI1, LL_GPIO_PIN_4, txdata, rxdata, 2);
   return rxdata[1];
}

static uint8_t cc1101_read_burst_reg(uint8_t reg, uint8_t *data, uint8_t size)
{
   uint8_t tx_data[64] = {0};
   uint8_t rx_data[64] = {0};

   tx_data[0] = reg | CC1101_READ_BURST;
   for (uint8_t i = 1; i < (uint8_t) (size + 1); i++)
   {
      tx_data[i] = 0xFFu;
   }

   SPI_Transfer(SPI1, LL_GPIO_PIN_4, tx_data, rx_data, size + 1);
   memcpy(data, &rx_data[1], size);
   return rx_data[0];
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

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
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

   cc1101_cmd_strobe(CC1101_CMD_SFTX);
   TS_Delay_ms(100);
   cc1101_cmd_strobe(CC1101_CMD_SFRX);
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
   cc1101_write_reg(CC1101_R_IOCFG0, 0x06 /*0x80*/);   // GDO0_Pin Output Pin Configuration
   cc1101_write_reg(CC1101_R_FIFOTHR, 0x07);           // RX FIFO and TX FIFO Thresholds
   cc1101_write_reg(CC1101_R_SYNC1, 0x57);             // Sync Word, High Byte
   cc1101_write_reg(CC1101_R_SYNC0, 0x43);             // Sync Word, Low Byte
   cc1101_write_reg(CC1101_R_PKTLEN, 0x3E);            // Packet Length
   cc1101_write_reg(CC1101_R_PKTCTRL1, 0x00 /*0x06*/); // Packet Automation Control
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

   cc1101_write_burst_reg(CC1101_CMD_PATABLE, PA, PA_LEN);
   cc1101_set_ism();
   cc1101_set_channel();
   cc1101_set_output_power(0);

   CC1101_Check_State();

   cc1101_cmd_strobe(CC1101_CMD_SRX);
   CC1101_Check_State();
   while (CC1101_STATE_STARTCAL == cc1101_read_reg(CC1101_R_MARCSTATE))
   {
   }
   CC1101_Check_State();
   while (CC1101_STATE_RX != cc1101_read_reg(CC1101_R_MARCSTATE))
   {
   }
}

void CC1101_Reset(void)
{
   cc1101_cmd_strobe(CC1101_CMD_SRES);
   TS_Delay_ms(1);
   while (cc1101_cmd_strobe(CC1101_CMD_SNOP) & 0x80)
   {
   }
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
   uint8_t data[2];
   uint8_t tx[2] = {CC1101_READ_BURST | CC1101_R_MARCSTATE, 0xFF};

   SPI_Transfer(SPI1, LL_GPIO_PIN_4, tx, data, 2);

   switch (data[1])
   {
   case CC1101_STATE_SLEEP:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_IDLE:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_XOFF:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_VCOON_MC:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_REGON_MC:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_MANCAL:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_VCOON:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_REGON:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_STARTCAL:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_BWBOOST:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_FS_LOCK:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_IFADCON:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_ENDCAL:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_RX:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_RX_END:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_RX_RST:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_TXRX_SWITCH:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_RXFIFO_OVERFLOW:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_FSTXON:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_TX:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_TX_END:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_RXTX_SWITCH:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   case CC1101_STATE_TXFIFO_UNDERFLOW:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   default:
      log_info(&cc1101_dev, "CC1101_STATE: %s | STATUS BYTE = 0x%x\r\n",
               CC1101_MARC_STATE_TO_STR(data[1]), data[0]);
      break;
   }

   return data[1];
}

void CC1101_Debug_Init(void)
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

   cc1101_cmd_strobe(CC1101_CMD_SIDLE);

   // Match RPi dump: GDO2=0x29, GDO1=0x2E, GDO0=0x06
   cc1101_write_reg(CC1101_R_IOCFG2, 0x29);
   cc1101_write_reg(CC1101_R_IOCFG1, 0x2E);
   cc1101_write_reg(CC1101_R_IOCFG0, 0x06);

   // --- FIFO threshold ---
   cc1101_write_reg(CC1101_R_FIFOTHR, 0x47);

   // --- Sync word (match RPi) ---
   cc1101_write_reg(CC1101_R_SYNC1, 0xD3);
   cc1101_write_reg(CC1101_R_SYNC0, 0x91);

   // --- Packet config (match RPi dump, profile: RPI-2026-05-29-225028) ---
   cc1101_write_reg(CC1101_R_PKTLEN, 0x3D);
   cc1101_write_reg(CC1101_R_PKTCTRL1,
                    CC1101_DBG_PKTCTRL1_PROFILE); // 0x04: APPEND_STATUS only
   cc1101_write_reg(CC1101_R_PKTCTRL0,
                    CC1101_DBG_PKTCTRL0_PROFILE); // 0x05: variable length, whitening OFF, CRC ON

   cc1101_write_reg(CC1101_R_ADDR, CC1101_DBG_ADDR_PROFILE);
   cc1101_write_reg(CC1101_R_CHANNR, 0x00);

   // --- Frequency synthesizer ---
   cc1101_write_reg(CC1101_R_FSCTRL1, 0x06); // IF = 152.3 kHz
   cc1101_write_reg(CC1101_R_FSCTRL0, 0x00); // brak offsetu

   // Frequency 434.00 MHz
   cc1101_write_reg(CC1101_R_FREQ2, 0x21);
   cc1101_write_reg(CC1101_R_FREQ1, 0x62);
   cc1101_write_reg(CC1101_R_FREQ0, 0x76);

   // Modem config (GFSK ~2.4 kbps)
   cc1101_write_reg(CC1101_R_MDMCFG4, 0xCA);
   cc1101_write_reg(CC1101_R_MDMCFG3, 0x83);
   cc1101_write_reg(CC1101_R_MDMCFG2,
                    CC1101_DBG_MDMCFG2_PROFILE); // profile-controlled SYNC_MODE
   cc1101_write_reg(CC1101_R_MDMCFG1,
                    CC1101_DBG_MDMCFG1_PROFILE); // profile-controlled MDMCFG1
   cc1101_write_reg(CC1101_R_MDMCFG0, 0xF8);

   cc1101_write_reg(CC1101_R_DEVIATN, 0x35);

   // State machine
   cc1101_write_reg(CC1101_R_MCSM2, 0x07);
   cc1101_write_reg(CC1101_R_MCSM0, 0x18); // auto-calibrate
   cc1101_write_reg(CC1101_R_MCSM1, 0x30);

   cc1101_write_reg(CC1101_R_FOCCFG, 0x16);
   cc1101_write_reg(CC1101_R_BSCFG, 0x6C);

   cc1101_write_reg(CC1101_R_AGCTRL2, 0x43);
   cc1101_write_reg(CC1101_R_AGCTRL1, 0x40);
   cc1101_write_reg(CC1101_R_AGCTRL0, 0x91);

   cc1101_write_reg(CC1101_R_WOREVT1, 0x87);
   cc1101_write_reg(CC1101_R_WOREVT0, 0x6B);
   cc1101_write_reg(CC1101_R_WORCTRL, 0xFB);

   cc1101_write_reg(CC1101_R_FREND1, 0x56);
   cc1101_write_reg(CC1101_R_FREND0, 0x10);

   cc1101_write_reg(CC1101_R_FSCAL3, 0xEF);
   cc1101_write_reg(CC1101_R_FSCAL2, 0x2B);
   cc1101_write_reg(CC1101_R_FSCAL1, 0x1A);
   cc1101_write_reg(CC1101_R_FSCAL0, 0x1F);

   cc1101_write_reg(CC1101_R_RCCTRL1, 0x41);
   cc1101_write_reg(CC1101_R_RCCTRL0, 0x00);
   cc1101_write_reg(CC1101_R_FSTEST, 0x59);
   cc1101_write_reg(CC1101_R_PTEST, 0x7F);
   cc1101_write_reg(CC1101_R_AGCTEST, 0x3E);

   // Test registers
   cc1101_write_reg(CC1101_R_TEST2, 0x81);
   cc1101_write_reg(CC1101_R_TEST1, 0x35);
   cc1101_write_reg(CC1101_R_TEST0, 0x09);

   // PATABLE jak w dumpie RPi
   uint8_t       PA[] = {0xC0};
   const uint8_t PA_LEN = 1;
   cc1101_write_burst_reg(CC1101_CMD_PATABLE, PA, PA_LEN);

   // Verification dump after init
   uint8_t sync1 = cc1101_read_reg(CC1101_R_SYNC1);
   uint8_t sync0 = cc1101_read_reg(CC1101_R_SYNC0);
   uint8_t pktlen = cc1101_read_reg(CC1101_R_PKTLEN);
   uint8_t pktctrl1 = cc1101_read_reg(CC1101_R_PKTCTRL1);
   uint8_t mdmcfg4 = cc1101_read_reg(CC1101_R_MDMCFG4);
   uint8_t mdmcfg3 = cc1101_read_reg(CC1101_R_MDMCFG3);
   uint8_t mdmcfg2 = cc1101_read_reg(CC1101_R_MDMCFG2);
   uint8_t mdmcfg1 = cc1101_read_reg(CC1101_R_MDMCFG1);
   uint8_t pktctrl0 = cc1101_read_reg(CC1101_R_PKTCTRL0);
   uint8_t addr = cc1101_read_reg(CC1101_R_ADDR);
   log_dbg(&cc1101_dev, "CC1101 Init Verification:\r\n");
   log_dbg(&cc1101_dev, "  Profile tag=%s\r\n", CC1101_DBG_PROFILE_TAG);
   log_dbg(&cc1101_dev, "  SYNC1/0=0x%02X/0x%02X PKTLEN=0x%02X PKTCTRL1/0=0x%02X/0x%02X\r\n", sync1,
           sync0, pktlen, pktctrl1, pktctrl0);
   log_dbg(&cc1101_dev, "  MDMCFG4/3/2/1=0x%02X/0x%02X/0x%02X/0x%02X\r\n", mdmcfg4, mdmcfg3,
           mdmcfg2, mdmcfg1);
   log_dbg(&cc1101_dev, "  ADDR=0x%02X\r\n", addr);
}

uint8_t CC1101_Debug_Tx(void)
{
   // uint8_t payload[] = "HelloWorld";
   // uint8_t payload_len = strlen((char *) payload);

   // uint8_t txdata[3 + payload_len];

   // txdata[0] = payload_len + 2;   // długość BEZ bajtu length
   // txdata[1] = CC1101_BR_ADDRESS; // dest
   // txdata[2] = CC1101_TX_ADDRESS; // src

   // memcpy(&txdata[3], payload, payload_len);

   // cc1101_write_burst_reg(CC1101_R_TX_FIFO, txdata, payload_len + 3);

   // CC1101_Check_State();

   // log_dbg(&cc1101_dev, "Changing state to IDLE\r\n");
   // cc1101_cmd_strobe(CC1101_CMD_SIDLE);
   // while (CC1101_STATE_IDLE != cc1101_read_reg(CC1101_R_MARCSTATE))
   // {
   // }

   // cc1101_write_burst_reg(CC1101_R_TX_FIFO, txdata, payload_len + 3); // Write TX data

   // log_dbg(&cc1101_dev, "Changing state to TX\r\n");
   // cc1101_cmd_strobe(CC1101_CMD_STX);
   // CC1101_Check_State();
   // while (CC1101_STATE_STARTCAL == cc1101_read_reg(CC1101_R_MARCSTATE))
   // {
   // }
   // CC1101_Check_State();
   // while (CC1101_STATE_TX == cc1101_read_reg(CC1101_R_MARCSTATE))
   // {
   // }
   // // CC1101_Check_State();
   // // while (CC1101_STATE_TX_END == cc1101_read_reg(CC1101_R_MARCSTATE))
   // // {
   // // }
   // CC1101_Check_State();
   // while (CC1101_STATE_IDLE != cc1101_read_reg(CC1101_R_MARCSTATE))
   // {
   // }
   // cc1101_cmd_strobe(CC1101_CMD_STX);

   // while (cc1101_read_reg(CC1101_R_MARCSTATE) == CC1101_STATE_TX)
   // {
   // }

   // return 1;

   uint8_t data[10] = "HelloWorld";

   log_dbg(&cc1101_dev, "---- TX START ----\r\n");

   // 1️⃣ IDLE
   cc1101_cmd_strobe(CC1101_CMD_SIDLE);
   TS_Delay_ms(1);

   uint8_t state = cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F;
   log_dbg(&cc1101_dev, "State after SIDLE: 0x%02X\r\n", state);

   // 2️⃣ Flush TX FIFO
   cc1101_cmd_strobe(CC1101_CMD_SFTX);
   TS_Delay_ms(1);

   // Sprawdź czy FIFO puste
   uint8_t txbytes = cc1101_read_reg(CC1101_R_TXBYTES) & 0x7F;
   log_dbg(&cc1101_dev, "TXBYTES after flush: %d\r\n", txbytes);

   // 3️⃣ Załaduj dane
   cc1101_write_burst_reg(CC1101_R_TX_FIFO, data, sizeof(data));

   txbytes = cc1101_read_reg(CC1101_R_TXBYTES) & 0x7F;
   log_dbg(&cc1101_dev, "TXBYTES after write: %d\r\n", txbytes);

   // 4️⃣ Start TX
   cc1101_cmd_strobe(CC1101_CMD_STX);

   TS_Delay_ms(1);

   state = cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F;
   log_dbg(&cc1101_dev, "State after STX: 0x%02X\r\n", state);

   // 5️⃣ Czekaj aż wejdzie w TX
   while ((cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F) != 0x13)
   {
      // 0x13 = TX
   }

   log_dbg(&cc1101_dev, "Entered TX state\r\n");

   // 6️⃣ Czekaj aż wyjdzie z TX
   while ((cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F) == 0x13)
   {
   }

   log_dbg(&cc1101_dev, "Exited TX state\r\n");

   // 7️⃣ Sprawdź końcowy stan
   state = cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F;
   log_dbg(&cc1101_dev, "Final state: 0x%02X\r\n", state);

   log_dbg(&cc1101_dev, "---- TX END ----\r\n");
}

uint8_t CC1101_Debug_Rx(void)
{
   static uint32_t stat_ok = 0;
   static uint32_t stat_bad_len = 0;
   static uint32_t stat_bad_crc = 0;
   static uint32_t stat_resync = 0;
   static uint32_t stat_incomplete = 0;
   static uint32_t stat_whitening_toggle = 0;
   static uint32_t stat_sync_relax = 0;
   static uint32_t stat_pktctrl1_relax = 0;
   static uint32_t stat_next_print_ms = 0;
   static uint32_t stat_len_histogram[256] = {0};
   static uint8_t  bad_crc_streak = 0;
   static uint8_t  runtime_mdmcfg2 = CC1101_DBG_MDMCFG2_PROFILE;

   uint8_t  rxBuffer[64];
   uint8_t  marcstate = 0;
   uint8_t  packet_len = 0;
   uint8_t  rxbytes = 0;
   uint8_t  pktstatus = 0;
   uint8_t  seen_carrier = 0;
   uint8_t  seen_pqt = 0;
   uint8_t  seen_sfd = 0;
   int8_t   max_rssi = -127;
   uint32_t next_diag_ms = 0;
   uint32_t sfd_stuck_since_ms = 0;

   if (stat_next_print_ms == 0)
   {
      stat_next_print_ms = TS_Get_ms() + 10000;
   }

   log_dbg(&cc1101_dev, "---- RX START ----\r\n");

   // IDLE + flush RX
   cc1101_cmd_strobe(CC1101_CMD_SIDLE);
   TS_Delay_ms(10);

   marcstate = cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F;
   log_dbg(&cc1101_dev, "After SIDLE: 0x%02X\r\n", marcstate);

   cc1101_cmd_strobe(CC1101_CMD_SFRX);
   TS_Delay_ms(10);

   // Wejdź w RX
   cc1101_cmd_strobe(CC1101_CMD_SRX);
   TS_Delay_ms(10);

   // Czekaj na wejście w RX MODE - WAŻNE!
   uint32_t timeout_rx = TS_Get_ms() + 1000;
   while (TS_Get_ms() < timeout_rx)
   {
      marcstate = cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F;

      // Czekaj na wejście w RX (state 0x0D) lub STARTCAL (0x08)
      if (marcstate == CC1101_STATE_RX || marcstate == 0x0D)
      {
         log_dbg(&cc1101_dev, "Entered RX mode (MARCSTATE: 0x%02X)\r\n", marcstate);
         break;
      }
      else if (marcstate == CC1101_STATE_STARTCAL || marcstate == 0x08)
      {
         log_dbg(&cc1101_dev, "Calibrating... (MARCSTATE: 0x%02X)\r\n", marcstate);
         TS_Delay_ms(1);
      }
      else
      {
         log_dbg(&cc1101_dev, "Waiting for RX mode... current: 0x%02X\r\n", marcstate);
         TS_Delay_ms(1);
      }
   }

   marcstate = cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F;
   if (marcstate != CC1101_STATE_RX && marcstate != 0x0D)
   {
      log_dbg(&cc1101_dev, "Failed to enter RX mode! MARCSTATE: 0x%02X\r\n", marcstate);
      CC1101_Check_State();
      return 0;
   }

   log_dbg(&cc1101_dev, "Entered RX mode. Waiting for data...\r\n");

   /*
    * RX profile (match RPi dump):
    *   PKTCTRL1 = 0x04
    *   PKTCTRL0 = 0x05
    *   MDMCFG1/2 = 0x22/0x13
    * Struktura w RX FIFO: [length][payload...][RSSI][LQI/CRC_OK]
    * Pierwszy bajt to dlugosc payloadu (bez bajtu length i bez status bytes).
    */

   uint8_t pktctrl1_cfg = cc1101_read_reg(CC1101_R_PKTCTRL1);
   uint8_t status_bytes = (pktctrl1_cfg & (1u << 2)) ? 2u : 0u;

   // Wymus profile na kazdym starcie RX, aby uniknac dryftu miedzy buildami/stanami runtime.
   cc1101_cmd_strobe(CC1101_CMD_SIDLE);
   cc1101_write_reg(CC1101_R_PKTCTRL1, CC1101_DBG_PKTCTRL1_PROFILE);
   cc1101_write_reg(CC1101_R_PKTCTRL0, CC1101_DBG_PKTCTRL0_PROFILE);
   cc1101_write_reg(CC1101_R_ADDR, CC1101_DBG_ADDR_PROFILE);
   cc1101_write_reg(CC1101_R_MDMCFG1, CC1101_DBG_MDMCFG1_PROFILE);
   cc1101_write_reg(CC1101_R_MDMCFG2, runtime_mdmcfg2);
   cc1101_cmd_strobe(CC1101_CMD_SRX);
   TS_Delay_ms(2);
   pktctrl1_cfg = cc1101_read_reg(CC1101_R_PKTCTRL1);
   status_bytes = (pktctrl1_cfg & (1u << 2)) ? 2u : 0u;

   // Utrzymuj zgodnosc z profilem RPi.
   uint8_t mdmcfg1_cfg = cc1101_read_reg(CC1101_R_MDMCFG1);
   uint8_t mdmcfg2_cfg = cc1101_read_reg(CC1101_R_MDMCFG2);
   if ((mdmcfg1_cfg != CC1101_DBG_MDMCFG1_PROFILE) || (mdmcfg2_cfg != runtime_mdmcfg2))
   {
      cc1101_cmd_strobe(CC1101_CMD_SIDLE);
      cc1101_write_reg(CC1101_R_MDMCFG1, CC1101_DBG_MDMCFG1_PROFILE);
      cc1101_write_reg(CC1101_R_MDMCFG2, runtime_mdmcfg2);
      cc1101_cmd_strobe(CC1101_CMD_SRX);
      log_dbg(&cc1101_dev,
              "RX profile restore: MDMCFG1/2 -> 0x%02X/0x%02X (RPi profile, adaptive)\r\n",
              CC1101_DBG_MDMCFG1_PROFILE, runtime_mdmcfg2);
      TS_Delay_ms(2);
   }

   // Czekaj na pierwszy bajt długości (dłuższe okno ogranicza restart RX)
   uint32_t timeout = TS_Get_ms() + 30000;
   next_diag_ms = TS_Get_ms() + 1000;

   while (1)
   {
      while (TS_Get_ms() < timeout)
      {
         marcstate = cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F;
         rxbytes = cc1101_read_reg(CC1101_R_RXBYTES);
         pktstatus = cc1101_read_reg(CC1101_R_PKTSTATUS);

         int8_t current_rssi = cc1101_get_rssi();
         if (current_rssi > max_rssi)
         {
            max_rssi = current_rssi;
         }

         if (pktstatus & (1u << 6))
         {
            seen_carrier = 1;
         }
         if (pktstatus & (1u << 5))
         {
            seen_pqt = 1;
         }
         if (pktstatus & (1u << 3))
         {
            seen_sfd = 1;
         }

         if (rxbytes & 0x80)
         {
            log_dbg(&cc1101_dev, "RX FIFO OVERFLOW!\r\n");
            cc1101_cmd_strobe(CC1101_CMD_SIDLE);
            cc1101_cmd_strobe(CC1101_CMD_SFRX);
            return 0;
         }

         rxbytes &= 0x7F;

         if ((pktstatus & (1u << 3)) && (rxbytes == 0))
         {
            if (sfd_stuck_since_ms == 0)
            {
               sfd_stuck_since_ms = TS_Get_ms();
            }
            else if ((TS_Get_ms() - sfd_stuck_since_ms) > 300)
            {
               log_dbg(&cc1101_dev,
                       "SFD stuck without FIFO data -> RX re-sync (PKTSTATUS=0x%02X)\r\n",
                       pktstatus);
               stat_resync++;
               cc1101_cmd_strobe(CC1101_CMD_SIDLE);
               cc1101_cmd_strobe(CC1101_CMD_SFRX);
               cc1101_cmd_strobe(CC1101_CMD_SRX);
               sfd_stuck_since_ms = 0;
               TS_Delay_ms(2);
               continue;
            }
         }
         else
         {
            sfd_stuck_since_ms = 0;
         }

         if (rxbytes > 0)
         {
            if (rxbytes == 1)
            {
               uint32_t wait_more_ms = TS_Get_ms() + 30;
               while (TS_Get_ms() < wait_more_ms)
               {
                  uint8_t rxbytes_now = cc1101_read_reg(CC1101_R_RXBYTES) & 0x7F;
                  if (rxbytes_now > 1)
                  {
                     rxbytes = rxbytes_now;
                     break;
                  }
                  TS_Delay_ms(1);
               }

               if (rxbytes == 1)
               {
                  stat_resync++;
                  cc1101_cmd_strobe(CC1101_CMD_SIDLE);
                  cc1101_cmd_strobe(CC1101_CMD_SFRX);
                  cc1101_cmd_strobe(CC1101_CMD_SRX);
                  sfd_stuck_since_ms = 0;
                  TS_Delay_ms(2);
                  continue;
               }
            }

            break;
         }

         if (TS_Get_ms() >= next_diag_ms)
         {
#if 0
            log_dbg(&cc1101_dev,
                    "RX wait: MARCSTATE=0x%02X RXBYTES=%u RSSI=%d PKTSTATUS=0x%02X\r\n", marcstate,
                    rxbytes, current_rssi, pktstatus);
#endif
            next_diag_ms += 500;
         }

         if (TS_Get_ms() >= stat_next_print_ms)
         {
            log_dbg(&cc1101_dev,
                    "RX stats (10s): ok=%lu bad_len=%lu bad_crc=%lu incomplete=%lu resync=%lu "
                    "wht_tgl=%lu sync_relax=%lu p1_relax=%lu\r\n",
                    (unsigned long) stat_ok, (unsigned long) stat_bad_len,
                    (unsigned long) stat_bad_crc, (unsigned long) stat_incomplete,
                    (unsigned long) stat_resync, (unsigned long) stat_whitening_toggle,
                    (unsigned long) stat_sync_relax, (unsigned long) stat_pktctrl1_relax);
            uint32_t hist_sum = 0;
            for (int i = 0; i < 256; i++)
            {
               if (stat_len_histogram[i] > 0)
               {
                  hist_sum++;
               }
            }
            if (hist_sum > 0)
            {
               log_dbg(&cc1101_dev, "Length histogram (non-zero buckets):\r\n");
               for (int i = 0; i < 256; i++)
               {
                  if (stat_len_histogram[i] > 0)
                  {
                     log_dbg(&cc1101_dev, "  len=%d: %lu\r\n", i,
                             (unsigned long) stat_len_histogram[i]);
                  }
               }
            }
            stat_next_print_ms += 10000;
         }

         TS_Delay_ms(1);
      }

      if (rxbytes == 0)
      {
         uint8_t sync1 = cc1101_read_reg(CC1101_R_SYNC1);
         uint8_t sync0 = cc1101_read_reg(CC1101_R_SYNC0);
         uint8_t mdmcfg2_now = cc1101_read_reg(CC1101_R_MDMCFG2);
         uint8_t pktstatus_now = cc1101_read_reg(CC1101_R_PKTSTATUS);
         uint8_t pktctrl1_now = cc1101_read_reg(CC1101_R_PKTCTRL1);
         uint8_t addr_now = cc1101_read_reg(CC1101_R_ADDR);

         if (seen_carrier && seen_pqt && !seen_sfd)
         {
            if (mdmcfg2_now == CC1101_DBG_MDMCFG2_PROFILE)
            {
               runtime_mdmcfg2 = CC1101_DBG_MDMCFG2_FALLBACK;
            }
            else if (mdmcfg2_now == CC1101_DBG_MDMCFG2_FALLBACK)
            {
               runtime_mdmcfg2 = CC1101_DBG_MDMCFG2_FALLBACK2;
            }
            else
            {
               runtime_mdmcfg2 = CC1101_DBG_MDMCFG2_PROFILE;
            }

            cc1101_cmd_strobe(CC1101_CMD_SIDLE);
            cc1101_write_reg(CC1101_R_MDMCFG2, runtime_mdmcfg2);
            cc1101_cmd_strobe(CC1101_CMD_SRX);
            stat_sync_relax++;
            log_dbg(&cc1101_dev,
                    "RX fallback: carrier+pqt bez SFD, MDMCFG2 -> 0x%02X (adaptive 13/12/11)\r\n",
                    runtime_mdmcfg2);

            // Upewnij sie, ze timeout dump pokazuje aktualna wartosc po fallbacku.
            mdmcfg2_now = cc1101_read_reg(CC1101_R_MDMCFG2);
         }

         log_dbg(&cc1101_dev, "RX timeout (30s) - no data received\r\n");
         log_dbg(&cc1101_dev, "RX summary: carrier=%u pqt=%u sfd=%u max_rssi=%d\r\n", seen_carrier,
                 seen_pqt, seen_sfd, max_rssi);
         log_dbg(&cc1101_dev,
                 "RX timeout regs: SYNC1/0=0x%02X/0x%02X MDMCFG2=0x%02X PKTCTRL1=0x%02X "
                 "ADDR=0x%02X PKTSTATUS=0x%02X\r\n",
                 sync1, sync0, mdmcfg2_now, pktctrl1_now, addr_now, pktstatus_now);
         marcstate = cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F;
         log_dbg(&cc1101_dev, "Final MARCSTATE: 0x%02X\r\n", marcstate);
         CC1101_Check_State();
         return 0;
      }

      packet_len = cc1101_read_reg(CC1101_R_RX_FIFO);
      log_dbg(&cc1101_dev, "Packet length byte: %u\r\n", packet_len);
      stat_len_histogram[packet_len]++;

      uint8_t max_payload = (uint8_t) (sizeof(rxBuffer) - status_bytes);
      if (packet_len > max_payload)
      {
         uint8_t pktstatus_now = cc1101_read_reg(CC1101_R_PKTSTATUS);
         uint8_t rxbytes_now = cc1101_read_reg(CC1101_R_RXBYTES) & 0x7F;
         stat_bad_len++;
         stat_resync++;
         log_dbg(&cc1101_dev,
                 "Invalid packet length: %u (max %u for local buffer), PKTSTATUS=0x%02X "
                 "RXBYTES=%u\r\n",
                 packet_len, max_payload, pktstatus_now, rxbytes_now);
         cc1101_cmd_strobe(CC1101_CMD_SIDLE);
         cc1101_cmd_strobe(CC1101_CMD_SFRX);
         cc1101_cmd_strobe(CC1101_CMD_SRX);
         rxbytes = 0;
         sfd_stuck_since_ms = 0;
         TS_Delay_ms(2);
         continue;
      }
      break;
   }

   timeout = TS_Get_ms() + 3000;
   uint32_t empty_fifo_since_ms = 0;
   while (TS_Get_ms() < timeout)
   {
      rxbytes = cc1101_read_reg(CC1101_R_RXBYTES);
      pktstatus = cc1101_read_reg(CC1101_R_PKTSTATUS);

      if (rxbytes & 0x80)
      {
         log_dbg(&cc1101_dev, "RX FIFO OVERFLOW while waiting for full packet!\r\n");
         cc1101_cmd_strobe(CC1101_CMD_SIDLE);
         cc1101_cmd_strobe(CC1101_CMD_SFRX);
         return 0;
      }

      rxbytes &= 0x7F;
      if (rxbytes >= (uint8_t) (packet_len + status_bytes))
      {
         break;
      }

      if ((rxbytes == 0) && ((pktstatus & (1u << 3)) == 0))
      {
         if (empty_fifo_since_ms == 0)
         {
            empty_fifo_since_ms = TS_Get_ms();
         }
         else if ((TS_Get_ms() - empty_fifo_since_ms) > 40)
         {
            break;
         }
      }
      else
      {
         empty_fifo_since_ms = 0;
      }

      TS_Delay_ms(5);
   }

   rxbytes = cc1101_read_reg(CC1101_R_RXBYTES) & 0x7F;
   if (rxbytes < (uint8_t) (packet_len + status_bytes))
   {
      uint8_t pktctrl0_now = cc1101_read_reg(CC1101_R_PKTCTRL0);
      uint8_t pktctrl1_now = cc1101_read_reg(CC1101_R_PKTCTRL1);
      uint8_t pktstatus_now = cc1101_read_reg(CC1101_R_PKTSTATUS);
      uint8_t marcstate_now = cc1101_read_reg(CC1101_R_MARCSTATE) & 0x1F;

      stat_incomplete++;
      log_dbg(&cc1101_dev, "Incomplete packet: expected at least %u bytes, got %u\r\n",
              (uint8_t) (packet_len + status_bytes), rxbytes);
      log_dbg(&cc1101_dev,
              "Incomplete context: PKTCTRL1=0x%02X PKTCTRL0=0x%02X PKTSTATUS=0x%02X "
              "MARCSTATE=0x%02X\r\n",
              pktctrl1_now, pktctrl0_now, pktstatus_now, marcstate_now);

      cc1101_cmd_strobe(CC1101_CMD_SIDLE);
      cc1101_cmd_strobe(CC1101_CMD_SFRX);
      cc1101_cmd_strobe(CC1101_CMD_SRX);
      return 0;
   }

   cc1101_read_burst_reg(CC1101_R_RX_FIFO, rxBuffer, packet_len + status_bytes);

   char              ascii_payload[64 + 1];
   static const char hex_chars[] = "0123456789ABCDEF";
   char              hex_payload[(64 * 3) + 1];

   for (uint8_t i = 0; i < packet_len; i++)
   {
      ascii_payload[i] = ((rxBuffer[i] >= 32u) && (rxBuffer[i] <= 126u)) ? (char) rxBuffer[i] : '.';
      hex_payload[i * 3] = hex_chars[(rxBuffer[i] >> 4) & 0x0F];
      hex_payload[(i * 3) + 1] = hex_chars[rxBuffer[i] & 0x0F];
      hex_payload[(i * 3) + 2] = (i + 1u < packet_len) ? ' ' : '\0';
   }
   ascii_payload[packet_len] = '\0';

   if (status_bytes == 2)
   {
      uint8_t rssi_raw = rxBuffer[packet_len];
      uint8_t lqi_crc_raw = rxBuffer[packet_len + 1];
      uint8_t crc_ok = (lqi_crc_raw & 0x80u) ? 1u : 0u;

      if (rxBuffer[packet_len + 1] & 0x80u)
      {
         stat_ok++;
         bad_crc_streak = 0;
      }
      else
      {
         stat_bad_crc++;
         bad_crc_streak++;

         if (bad_crc_streak >= 5)
         {
            stat_whitening_toggle++;
            bad_crc_streak = 0;
            log_dbg(&cc1101_dev,
                    "CRC streak reached 5 (auto-fallback disabled; keeping static config)\r\n");
         }
      }

      log_dbg(&cc1101_dev,
              "RX OK: len=%u crc=%s rssi_raw=0x%02X lqi_crc=0x%02X ascii=\"%s\" hex=%s\r\n",
              packet_len, crc_ok ? "yes" : "no", rssi_raw, lqi_crc_raw, ascii_payload, hex_payload);
   }
   else
   {
      log_dbg(&cc1101_dev, "RX OK: len=%u status=off ascii=\"%s\" hex=%s\r\n", packet_len,
              ascii_payload, hex_payload);
   }
   log_dbg(&cc1101_dev, "---- RX DONE ----\r\n");

   return 0;
}

int8_t CC1101_Get_Temperature(int8_t *temperature)
{
   cc1101_cmd_strobe(CC1101_CMD_SIDLE);
   while (CC1101_STATE_IDLE != cc1101_read_reg(CC1101_R_MARCSTATE))
   {
   }
   /* Enable temperature sensor */
   cc1101_write_reg(CC1101_R_PTEST, 0xBF);
   cc1101_write_reg(CC1101_R_TEST0, 0x09);
   /* Wait for conversion to complete ~1ms */
   TS_Delay_ms(10);
   /* Read temperature value */
   uint8_t temp = cc1101_read_reg(CC1101_R_TEST1);
   /* Convert to Celsius using the formula from the datasheet */
   *temperature = (int8_t) temp;
   log_info(&cc1101_dev, "Temperature: %d °C\r\n", *temperature);
   /* Restore original TEST0 value */
   cc1101_write_reg(CC1101_R_PTEST, 0x7F);
   cc1101_write_reg(CC1101_R_TEST0, 0x09);

   cc1101_cmd_strobe(CC1101_CMD_SRX);
   while (CC1101_STATE_RX != cc1101_read_reg(CC1101_R_MARCSTATE))
   {
   }
   return 0;
}
