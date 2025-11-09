/**
 ********************************************************************************
 * @file    WS25Qxx_reg.h
 * @author  Mikolaj Pieklo
 * @date    10.11.2024
 * @brief
 ********************************************************************************
 */

#ifndef __WS25QXX_REG_H__
#define __WS25QXX_REG_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/

/************************************
 * MACROS AND DEFINES
 ************************************/
#define WINDBOND_64M_ID 0xEF4017

#define WS25QXX_INST_Write_Enable            0x06
#define WS25QXX_INST_Manufacturer            0x90
#define WS25QXX_INST_JEDEC_ID                0x9F
#define WS25QXX_INST_READ_STATUS_REGISTER_1  0x05
#define WS25QXX_INST_READ_STATUS_REGISTER_2  0x35
#define WS25QXX_INST_WRITE_STATUS_REGISTER_2 0x31
#define WS25QXX_INST_READ_STATUS_REGISTER_3  0x15
#define WS25QXX_INST_READ_DATA               0x03
#define WS25QXX_INST_PAGE_PROGRAM            0x02
#define WS25QXX_INST_SECTOR_ERASE            0x20
#define WS25QXX_INST_CHIP_ERASE              0xC7
#define WS25QXX_INST_ENABLE_RESET            0x66
#define WS25QXX_INST_RESET                   0x99

#define WS25QXX_STATUS_BUSY_MASK 0x01

#define WS25Q64_BLOCK_NUMBERS 0x80
#define WS25Q64_SECTOR_SIZE   0x1000
#define WS25Q64_BLOCK_SIZE    0x10000



/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/


#ifdef __cplusplus
}
#endif

#endif