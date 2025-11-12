/*
 * si4432.c
 *
 *  Created on: Dec 24, 2022
 *      Author: mkpk
 */

#include "si4432.h"

#include <stdbool.h>
#include <stdio.h>

#ifdef STM32F103xB
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_exti.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
#elif STM32F401xC
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_exti.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>
#include <stm32f4xx_ll_system.h>
#else
#error Module not supported!
#endif

#include <delay.h>
#include <log.h>
#include <reuse.h>
#include <si4432_reg.h>
#include <spi.h>
// B7 - SDN "0"
// B6 - irq

/* Dummy device */
static const struct device si4432_dev = {
   .name = "SI4432",
};

#define SI4432_DeviceTypeCode 0x08
#define SI4432_VersionCode    0x06

#define SI4432_ENSWDET  0x80
#define SI4432_IPKVALID 0x02

volatile bool irq = false;

void EXTI9_5_IRQHandler(void)
{
   if (SET == LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_6))
   {
      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_6);
      irq = true;
   }
}

static void si4432_spi_cs_low(void)
{
   LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
}

static void si4432_spi_cs_high(void)
{
   LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4);
}

static uint8_t spi_write(uint8_t reg, uint8_t txdata)
{
   si4432_spi_cs_low();

   LL_SPI_TransmitData8(SPI1, reg);
   while (LL_SPI_IsActiveFlag_BSY(SPI1))
      ;
   LL_SPI_ReceiveData8(SPI1);

   LL_SPI_TransmitData8(SPI1, txdata);
   while (LL_SPI_IsActiveFlag_BSY(SPI1))
      ;
   LL_SPI_ReceiveData8(SPI1);

   si4432_spi_cs_high();

   return 0;
}

static void spi_read(uint8_t reg, uint8_t *rxdata)
{
   si4432_spi_cs_low();

   LL_SPI_TransmitData8(SPI1, reg);
   while (LL_SPI_IsActiveFlag_BSY(SPI1))
      ;
   LL_SPI_ReceiveData8(SPI1);

   LL_SPI_TransmitData8(SPI1, 0xFF);
   while (LL_SPI_IsActiveFlag_BSY(SPI1))
      ;
   *rxdata = LL_SPI_ReceiveData8(SPI1);

   si4432_spi_cs_high();
}

static void si4432_configure_adc(void)
{
   spi_write(SI4432_WRITE | SI4432_R_ADC_CONFIGURATION, 0x00);
   spi_write(SI4432_WRITE | SI4432_R_TEMPERATURE_SENSOR_CONTROL, 0x20);
}

static void si4432_start_adc(void)
{
   uint8_t adc, temp;

   spi_write(SI4432_WRITE | SI4432_R_ADC_CONFIGURATION, 0x80);
   TS_Delay_ms(10);
   spi_read(SI4432_READ | SI4432_R_ADC_VALUE, &adc);

   log_info(&si4432_dev, "ADC_VALUE: 0x%x\r\n", adc);
   temp = (uint8_t) adc * 0.5 - 64;
   log_info(&si4432_dev, "TEMP: %d\r\n", temp);
}

static void si4432_enable_lbd(void)
{
   uint8_t data = 0;

   spi_read(SI4432_READ | SI4432_R_OPERATING_FUNCTION_CONTROL_1, &data);
   data |= 0x40;
   spi_write(SI4432_WRITE | SI4432_R_OPERATING_FUNCTION_CONTROL_1, data);
}

static void si4432_read_lbd(void)
{
   uint8_t  adc;
   uint32_t voltage;

   spi_read(SI4432_READ | SI4432_R_BATTERY_VOLTAGE_LEVEL, &adc);

   log_info(&si4432_dev, "ADC_VALUE: 0x%x\r\n", adc);
   voltage = 1700 + (50 * adc);
   log_info(&si4432_dev, "Voltage: %dmV\r\n", voltage);
}

RadioStatus_t SI4432_Init(void)
{
   uint8_t data;

#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
#elif STM32F401xC
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#endif

   LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinSpeed(GPIOB, LL_GPIO_PIN_7, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOB, LL_GPIO_PIN_7, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_DOWN);

   // configure EXTI B6
   LL_EXTI_InitTypeDef exti_initstruct;
   /* Configure IO */
   LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_INPUT);

   /* -2- Connect External Line to the GPIO*/
#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
   LL_GPIO_AF_SetEXTISource(LL_GPIO_AF_EXTI_PORTB, LL_GPIO_AF_EXTI_LINE6);
#elif STM32F401xC
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
   LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE6);
#endif

   /*-3- Enable a falling trigger EXTI line 13 Interrupt */
   /* Set fields of initialization structure */
   exti_initstruct.Line_0_31 = LL_EXTI_LINE_6;
   exti_initstruct.LineCommand = ENABLE;
   exti_initstruct.Mode = LL_EXTI_MODE_IT;
   exti_initstruct.Trigger = LL_EXTI_TRIGGER_FALLING;

   /* Initialize EXTI according to parameters defined in initialization structure. */
   LL_EXTI_Init(&exti_initstruct);

   /*-4- Configure NVIC for EXTI15_10_IRQn */
   NVIC_EnableIRQ(EXTI9_5_IRQn);
   NVIC_SetPriority(EXTI9_5_IRQn, 0);

   si4432_spi_cs_high();
   LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);

   TS_Delay_ms(30);

   RadioStatus_t retval = RadioStatusError;
   do
   {
      // RESET
      spi_write(SI4432_WRITE | SI4432_R_OPERATING_FUNCTION_CONTROL_1, 0xFF);
      TS_Delay_ms(5);

      spi_read(SI4432_READ | SI4432_R_DEVICE_TYPE, &data);
      if (SI4432_DeviceTypeCode != data) // Check device type code
      {
         retval = RadioStatusError;
         break;
      }
      log_info(&si4432_dev, "TYPE: 0x%x\r\n", data);

      spi_read(SI4432_READ | SI4432_R_DEVICE_VERSION, &data);
      if (SI4432_VersionCode != data) // Check version code
      {
         retval = RadioStatusError;
         break;
      }
      log_info(&si4432_dev, "VERSION: 0x%x\r\n", data);

      // Carrier freq
      spi_write(SI4432_WRITE | SI4432_R_FREQUENCY_BAND_SELECT, 0x53);
      spi_write(SI4432_WRITE | SI4432_R_NOMINAL_CARRIER_FREQUENCY_1, 0x62);
      spi_write(SI4432_WRITE | SI4432_R_NOMINAL_CARRIER_FREQUENCY_0, 0x00);

      // Tx Data rate
      spi_write(SI4432_WRITE | SI4432_R_TX_DATA_RATE_1, 0x10);
      spi_write(SI4432_WRITE | SI4432_R_TX_DATA_RATE_0, 0x62);
      spi_write(SI4432_WRITE | SI4432_R_MODULATION_MODE_CONTROL_1, 0x2C);
      spi_write(SI4432_WRITE | SI4432_R_CHARGE_PUMP_CURRENT_TRIMMING, 0x80);

      // Tx freq deviation
      spi_write(SI4432_WRITE | SI4432_R_FREQUENCY_DEVIATION, 0x50);
      spi_write(SI4432_WRITE | SI4432_R_MODULATION_MODE_CONTROL_2, 0x23);

      // Modem settings GFSK for RX, AFC Enable
      spi_write(SI4432_WRITE | SI4432_R_IF_FILTER_BANDWIDTH, 0x05);
      spi_write(SI4432_WRITE | SI4432_R_CLOCK_RECOVERY_OVERSAMPLING_RATIO, 0xD0);
      spi_write(SI4432_WRITE | SI4432_R_CLOCK_RECOVERY_OFFSET_2, 0xE0);
      spi_write(SI4432_WRITE | SI4432_R_CLOCK_RECOVERY_OFFSET_1, 0x10);
      spi_write(SI4432_WRITE | SI4432_R_CLOCK_RECOVERY_OFFSET_0, 0x62);
      spi_write(SI4432_WRITE | SI4432_R_CLOCK_RECOVERY_TIMING_LOOP_GAIN_1, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_CLOCK_RECOVERY_TIMING_LOOP_GAIN_0, 0x03);
      spi_write(SI4432_WRITE | SI4432_R_AFC_LOOP_GEARSHIFT_OVERRIDE, 0x3C);
      spi_write(SI4432_WRITE | SI4432_R_AFC_TIMING_CONTROL, 0x02);
      spi_write(SI4432_WRITE | SI4432_R_AFC_LIMITER, 0xFF);
      spi_write(SI4432_WRITE | SI4432_R_CLOCK_RECOVERY_GEARSHIFT_OVERRIDE, 0x03);
      spi_write(SI4432_WRITE | SI4432_R_AGC_OVERRIDE_1, 0x60);

      // Set TX Power -1dBm ..+20dBm step 3dBm / 0b000 minimum ...0b111 maximum
      spi_write(SI4432_WRITE | SI4432_R_TX_POWER, 0x1F);

      // Select Turn On Packet Handler - ON
      // Select LSB/MSB First - MSB
      // Enable CRC
      // CRC Over Data Only
      // CRC TYPE - CRC16-IBM
      spi_write(SI4432_WRITE | SI4432_R_DATA_ACCESS_CONTROL, 0xAD);

      // No broadcast address enable
      // No Received Header check
      spi_write(SI4432_WRITE | SI4432_R_HEADER_CONTROL_1, 0x00);

      // Synchronization Word 3 and 2, Disable header bytes
      spi_write(SI4432_WRITE | SI4432_R_HEADER_CONTROL_2, 0x02);

      // Preamble Lenght 10 x 4 bits = 40 bits, AFC - ON
      spi_write(SI4432_WRITE | SI4432_R_PREAMBLE_LENGTH, 0x08); // 0x0A

      // Preamble Detection Threshold 5 x 4 bits = 20 bits rssi_offset
      spi_write(SI4432_WRITE | SI4432_R_PREAMBLE_DETECTION_CONTROL, 0x2A);

      // Configure sync Word 3 Value = 2D
      // Configure sync Word 2 Value = D4
      spi_write(SI4432_WRITE | SI4432_R_SYNC_WORD_3, 0x2D);
      spi_write(SI4432_WRITE | SI4432_R_SYNC_WORD_2, 0xD4);
      spi_write(SI4432_WRITE | SI4432_R_SYNC_WORD_1, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_SYNC_WORD_0, 0x00);

      // Transmit Header
      spi_write(SI4432_WRITE | SI4432_R_TRANSMIT_HEADER_3, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_TRANSMIT_HEADER_2, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_TRANSMIT_HEADER_1, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_TRANSMIT_HEADER_0, 0x00);

      // DATA Length Transmit
      spi_write(SI4432_WRITE | SI4432_R_TRANSMIT_PACKET_LENGTH, 0x08);

      // Check Header
      spi_write(SI4432_WRITE | SI4432_R_CHECK_HEADER_3, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_CHECK_HEADER_2, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_CHECK_HEADER_1, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_CHECK_HEADER_0, 0x00);

      // Header Enable
      spi_write(SI4432_WRITE | SI4432_R_HEADER_ENABLE_3, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_HEADER_ENABLE_2, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_HEADER_ENABLE_1, 0x00);
      spi_write(SI4432_WRITE | SI4432_R_HEADER_ENABLE_0, 0x00);

      // Set IRQ Enable Packet Sent.
      spi_write(SI4432_WRITE | SI4432_R_INTERRUPT_ENABLE_1, 0x04);
      spi_write(SI4432_WRITE | SI4432_R_INTERRUPT_ENABLE_2, 0x00);

      //      spi_read(SI4432_READ | SI4432_R_DEVICE_STATUS, &data);
      //      printf ("DEVICE_STATUS: 0x%x\r\n", data);
      //
      //      spi_read(SI4432_READ | SI4432_R_INTERRUPT_STATUS_1, &data);
      //      printf ("STATUS1: 0x%x\r\n", data);
      //
      //      spi_read(SI4432_READ | SI4432_R_INTERRUPT_STATUS_2, &data);
      //      printf ("STATUS2: 0x%x\r\n", data);
      //
      //      spi_read(SI4432_READ | SI4432_R_TX_POWER, &data);
      //      printf ("TX_POWER: 0x%x\r\n", data);
      //
      //      spi_read(SI4432_READ | SI4432_R_OPERATING_FUNCTION_CONTROL_1, &data);
      //      printf ("OPERATING_FUNCTION_CONTROL_1: 0x%x\r\n", data);
      //
      //      si4432_configure_adc();
      //      si4432_start_adc();
      //
      //      si4432_enable_lbd();
      //      TS_Delay_ms(2000);
      //      si4432_read_lbd();

      retval = RadioStatusOk;
   }
   while (0);

   if (retval == RadioStatusOk)
   {
      log_info(&si4432_dev, "Radio status: OK\r\n");
   }
   else
   {
      log_info(&si4432_dev, "Radio status: Error\r\n");
   }

   return retval;
}

void SI4432_Reset(void)
{
}

void SI4432_Debug(void)
{
}

uint8_t SI4432_Tx_Debug(void)
{
   // Packet
   spi_write(SI4432_WRITE | SI4432_R_TRANSMIT_PACKET_LENGTH, 0x08);

   /*fill the payload into the transmit FIFO, 8 bytes*/
   spi_write(SI4432_WRITE | SI4432_R_FIFO_ACCESS, 0xFF);
   spi_write(SI4432_WRITE | SI4432_R_FIFO_ACCESS, 0x00);
   spi_write(SI4432_WRITE | SI4432_R_FIFO_ACCESS, 0xFF);
   spi_write(SI4432_WRITE | SI4432_R_FIFO_ACCESS, 0x55);
   spi_write(SI4432_WRITE | SI4432_R_FIFO_ACCESS, 0x55);
   spi_write(SI4432_WRITE | SI4432_R_FIFO_ACCESS, 0x00);
   spi_write(SI4432_WRITE | SI4432_R_FIFO_ACCESS, 0xFF);
   spi_write(SI4432_WRITE | SI4432_R_FIFO_ACCESS, 0x0D);

   // CRC
   spi_write(SI4432_WRITE | SI4432_R_DATA_ACCESS_CONTROL, 0xAD);

   spi_write(SI4432_WRITE | SI4432_R_OPERATING_FUNCTION_CONTROL_1, 0x09);

   return 0;
}

uint8_t SI4432_isDataAvailable(uint8_t pipenum)
{
   UNUSED(pipenum);
   return 0;
}

void SI4432_Rx_Debug(void)
{
   if (true == irq)
   {
      uint8_t i, irq_status1, irq_status2, rssi, length, rxdata;

      irq = false;
      spi_read(SI4432_READ | SI4432_R_INTERRUPT_STATUS_1, &irq_status1);
      log_info(&si4432_dev, "irq_status1 0x%x\r\n", irq_status1);
      spi_read(SI4432_READ | SI4432_R_INTERRUPT_STATUS_2, &irq_status2);
      log_info(&si4432_dev, "irq_status1 0x%x\r\n", irq_status2);

      /*if Sync Word Received interrupt occurred*/
      if ((irq_status2 & SI4432_ENSWDET) == SI4432_ENSWDET)
      {
         spi_read(SI4432_READ | SI4432_R_RECEIVED_SIGNAL_STRENGTH_INDICATOR, &rssi);
         log_info(&si4432_dev, "rssi 0x%x\r\n", rssi);
      }
      /*if Valid Packet Received interrupt occurred*/
      if ((irq_status1 & SI4432_IPKVALID) == SI4432_IPKVALID)
      {
         // disable the receiver chain
         spi_write(SI4432_WRITE | SI4432_R_OPERATING_FUNCTION_CONTROL_1, 0x01);
         // read the Received Packet Length register
         spi_read(SI4432_READ | SI4432_R_RECEIVED_PACKET_LENGTH, &length);
         log_info(&si4432_dev, "length 0x%x\r\n", length);

         for (i = 0; i < length; i++)
         {
            spi_read(SI4432_READ | SI4432_R_FIFO_ACCESS, &rxdata);
            log_info(&si4432_dev, "%c\r\n", rxdata);
         }
      }

      spi_write(SI4432_WRITE | SI4432_R_OPERATING_FUNCTION_CONTROL_2, 0x02);
      spi_write(SI4432_WRITE | SI4432_R_OPERATING_FUNCTION_CONTROL_2, 0x00);

      /*enable transmitter
       Ready Mode - ON,
       RX on in Manual Transmit Mode. (Automatically cleared if Multiple Packets config. is disabled
       and a valid packet received.)
       */
      spi_write(SI4432_WRITE | SI4432_R_OPERATING_FUNCTION_CONTROL_1, 0x05);
   }
}

void SI4432_TxMode(uint8_t *address, uint8_t channel)
{
   UNUSED(address);
   UNUSED(channel);
}

void SI4432_RxMode(void)
{
   /*Set IRQ
    Enable Valid Packet Received
    Enable CRC Error
    Enable Sync Word Detected.*/
   spi_write(SI4432_WRITE | SI4432_R_INTERRUPT_ENABLE_1, 0x03);
   spi_write(SI4432_WRITE | SI4432_R_INTERRUPT_ENABLE_2, 0x80);
   /*enable transmitter
    * Ready Mode - ON,
    * RX on in Manual Transmit Mode. (Automatically cleared if Multiple Packets config. is disabled
    * and a valid packet received.)
    */
   spi_write(SI4432_WRITE | SI4432_R_OPERATING_FUNCTION_CONTROL_1, 0x05);
}

void SI4432_ClearTxFifo(void)
{
   uint8_t data;
   spi_read(SI4432_READ | SI4432_R_DEVICE_VERSION, &data);
   data |= 0x01;
   spi_write(SI4432_WRITE | SI4432_R_DEVICE_VERSION, data);
}

void SI4432_ClearRxFifo(void)
{
   uint8_t data;
   spi_read(SI4432_READ | SI4432_R_DEVICE_VERSION, &data);
   data |= 0x02;
   spi_write(SI4432_WRITE | SI4432_R_DEVICE_VERSION, data);
}
