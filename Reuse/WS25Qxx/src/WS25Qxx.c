/**
 ********************************************************************************
 * @file    WS25Qxx.c
 * @author  Mikolaj Pieklo
 * @date    10.11.2024
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "WS25Qxx.h"

#include <stdio.h>

#include "WS25Qxx_reg.h"
#include <delay.h>
#include <log.h>
#include <spi.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device ws25qxx_dev = {
   .name = "WS25QXX",
};

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
static void     ws25qxx_reset(void);
static uint32_t ws25qxx_check_id(void);
static void     ws25qxx_write_enable(void);
static uint8_t  ws25qxx_read_status_register1(void);
static uint8_t  ws25qxx_read_status_register2(void);
static uint8_t  ws25qxx_read_status_register3(void);
static void     ws25qxx_write_status_register2(uint8_t data);
static void     ws25qxx_erase_sector(uint32_t address);
static void     ws25qxx_erase_chip(void);
static uint8_t  ws25qxx_read_data(uint32_t address);
static void     ws25qxx_write_data(uint32_t address, uint8_t data);
static void     ws25qxx_wait_until_busy(void);

/************************************
 * STATIC FUNCTIONS
 ************************************/
uint32_t ws25qxx_check_id(void)
{
   uint32_t result = 0;
   uint8_t  tx_data[4] = {WS25QXX_INST_JEDEC_ID, 0, 0, 0};
   uint8_t  rx_data[4] = {0, 0, 0, 0};
   SPI_Transfer(SPI1, SPI1_CS2_Pin, tx_data, rx_data, 4);

   result = rx_data[1] << 16 | rx_data[2] << 8 | rx_data[3];

   return result;
}

void ws25qxx_reset(void)
{
   uint8_t tx_data = WS25QXX_INST_ENABLE_RESET;
   SPI_Transfer(SPI1, SPI1_CS2_Pin, &tx_data, NULL, 1);
   tx_data = WS25QXX_INST_RESET;
   SPI_Transfer(SPI1, SPI1_CS2_Pin, &tx_data, NULL, 1);
   TS_Delay_ms(1);
}

void ws25qxx_write_enable(void)
{
   uint8_t tx_data = WS25QXX_INST_Write_Enable;
   SPI_Transfer(SPI1, SPI1_CS2_Pin, &tx_data, NULL, 1);

   ws25qxx_wait_until_busy();
}

uint8_t ws25qxx_read_status_register1(void)
{
   uint8_t tx_data[2] = {WS25QXX_INST_READ_STATUS_REGISTER_1, 0};
   uint8_t rx_data[2];

   SPI_Transfer(SPI1, SPI1_CS2_Pin, tx_data, rx_data, 2);

   return rx_data[1];
}

uint8_t ws25qxx_read_status_register2(void)
{
   uint8_t tx_data[2] = {WS25QXX_INST_READ_STATUS_REGISTER_2, 0};
   uint8_t rx_data[2] = {0, 0};

   SPI_Transfer(SPI1, SPI1_CS2_Pin, tx_data, rx_data, 2);

   return rx_data[1];
}

uint8_t ws25qxx_read_status_register3(void)
{
   uint8_t tx_data[2] = {WS25QXX_INST_READ_STATUS_REGISTER_3, 0};
   uint8_t rx_data[2];

   SPI_Transfer(SPI1, SPI1_CS2_Pin, tx_data, rx_data, 2);

   return rx_data[1];
}

void ws25qxx_write_status_register2(uint8_t data)
{
   uint8_t tx_data[2] = {WS25QXX_INST_WRITE_STATUS_REGISTER_2, data};

   ws25qxx_write_enable();

   SPI_Transfer(SPI1, SPI1_CS2_Pin, tx_data, NULL, 2);
}

uint8_t ws25qxx_read_data(uint32_t address)
{
   uint8_t tx_data[5] = {WS25QXX_INST_READ_DATA, (uint8_t) ((address >> 16) & 0xFF),
                         (uint8_t) ((address >> 8) & 0xFF), (uint8_t) (address & 0xFF), 0};
   uint8_t rx_data[5] = {0, 0, 0, 0, 0};

   SPI_Transfer(SPI1, SPI1_CS2_Pin, tx_data, rx_data, 5);

   return rx_data[4];
}

void ws25qxx_write_data(uint32_t address, uint8_t data)
{
   uint8_t tx_data[5] = {WS25QXX_INST_PAGE_PROGRAM, (uint8_t) ((address >> 16) & 0xFF),
                         (uint8_t) ((address >> 8) & 0xFF), (uint8_t) (address & 0xFF), data};
   ws25qxx_write_enable();
   SPI_Transfer(SPI1, SPI1_CS2_Pin, tx_data, NULL, 5);

   ws25qxx_wait_until_busy();
}

void ws25qxx_wait_until_busy(void)
{
   while (0 != (WS25QXX_STATUS_BUSY_MASK & ws25qxx_read_status_register1()))
   {
   }
}

void ws25qxx_erase_sector(uint32_t address)
{
   uint8_t tx_data[4] = {WS25QXX_INST_SECTOR_ERASE, (uint8_t) ((address >> 16) & 0xFF),
                         (uint8_t) ((address >> 8) & 0xFF), (uint8_t) (address & 0xFF)};
   SPI_Transfer(SPI1, SPI1_CS2_Pin, tx_data, NULL, 4);

   ws25qxx_wait_until_busy();
}

void ws25qxx_erase_chip(void)
{
   uint8_t tx_data = WS25QXX_INST_CHIP_ERASE;
   SPI_Transfer(SPI1, SPI1_CS2_Pin, &tx_data, NULL, 1);

   ws25qxx_wait_until_busy();
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void WS25Qxx_Init(void)
{
   uint32_t dev_id = 0;
   ws25qxx_reset();

   dev_id = ws25qxx_check_id();
   log_info(&ws25qxx_dev, "Flash id: 0x%06X\r\n", dev_id);

   if (WINDBOND_64M_ID == dev_id)
   {
      log_info(&ws25qxx_dev, "Flash: WINDBOND 64M\r\n");
   }
   else if (WINDBOND_128M_ID == dev_id)
   {
      log_info(&ws25qxx_dev, "Flash: WINDBOND 128M\r\n");
   }
   else
   {
      log_info(&ws25qxx_dev, "Flash: Unknown device\r\n");
   }

   ws25qxx_write_status_register2(0x00);
}

void WS25Qxx_Erase_Chip(void)
{
   log_info(&ws25qxx_dev, "Flash erase\r\n");
   ws25qxx_erase_chip();
}