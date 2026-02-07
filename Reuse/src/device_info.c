/**
 ********************************************************************************
 * @file    device_info.c
 * @author  Mikolaj Pieklo
 * @date    10.11.2023
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "device_info.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "tm1637.h"
#include <log.h>

#ifdef STM32F103xB
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_crc.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_utils.h>
#elif STM32F401xC
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_crc.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_utils.h>
#else
#error Module not supported!
#endif


/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
/* Dummy device */
static const struct device device_dev = {
   .name = "Device",
};

#define HEADER_APP_MAGIC_STRING 0xABCDEFABu

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef struct __attribute__((packed)) header_app_typedef
{
   uint32_t magic;
   uint8_t  version_of_header;
   uint32_t crc32;
   uint32_t app_size;
   uint32_t data;
} header_app_t;

__attribute__((section(".header_app_section"))) volatile const header_app_t header_app = {
   .magic = HEADER_APP_MAGIC_STRING,
   .version_of_header = 0x01,
   .crc32 = 0xCCCCCCCC,
   .app_size = 0xCDCDCDCD,
   .data = 0xEFEFEFEF,
};

/************************************
 * STATIC VARIABLES
 ************************************/
static Device_Restart_Issue_T restart_issue = DEVICE_RESTART_ISSUE_NONE;
/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static void check_restart_issues(void);

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void check_restart_issues(void)
{
   if (true == LL_RCC_IsActiveFlag_HSECSS())
   {
      log_info(&device_dev, "Reset cause: HSECSS\r\n");
      restart_issue = DEVICE_RESTART_ISSUE_HSECSS;
   }
   if (true == LL_RCC_IsActiveFlag_IWDGRST())
   {
      log_info(&device_dev, "Reset cause: IWDGRST\r\n");
      restart_issue = DEVICE_RESTART_ISSUE_IWDG;
   }
   if (true == LL_RCC_IsActiveFlag_LPWRRST())
   {
      log_info(&device_dev, "Reset cause: LPWRRST\r\n");
      restart_issue = DEVICE_RESTART_ISSUE_LPWR;
   }
   if (true == LL_RCC_IsActiveFlag_PINRST())
   {
      log_info(&device_dev, "Reset cause: PINRST\r\n");
      restart_issue = DEVICE_RESTART_ISSUE_PIN;
   }
   if (true == LL_RCC_IsActiveFlag_PORRST())
   {
      log_info(&device_dev, "Reset cause: PORRST\r\n");
      restart_issue = DEVICE_RESTART_ISSUE_POR;
   }
   if (true == LL_RCC_IsActiveFlag_SFTRST())
   {
      log_info(&device_dev, "Reset cause: SFTRST\r\n");
      restart_issue = DEVICE_RESTART_ISSUE_SFT;
   }
   if (true == LL_RCC_IsActiveFlag_WWDGRST())
   {
      log_info(&device_dev, "Reset cause: WWDGRST\r\n");
      restart_issue = DEVICE_RESTART_ISSUE_WWDG;
   }
   LL_RCC_ClearResetFlags();
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void Device_Info(void)
{
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);

   extern uint32_t _sisr_vector;
   extern uint32_t _sheader_app_section;

   uint32_t crc_idx = 0;
   uint32_t all_size = ((uint32_t) &_sheader_app_section - (uint32_t) &_sisr_vector) / 4;

   LL_CRC_ResetCRCCalculationUnit(CRC);
   for (crc_idx = 0; crc_idx < all_size; crc_idx++)
   {
      LL_CRC_FeedData32(CRC, (uint32_t) *(&_sisr_vector + crc_idx));
   }

#ifdef USED_RTOS
   Log_Init();
#endif
#ifndef USED_SBL
   /* Clear terminal */
   log_info(&device_dev, "\x1b[2J\x1b[H");
#endif

   if (header_app.magic != HEADER_APP_MAGIC_STRING)
   {
      log_err(&device_dev, "Invalid header magic\r\n");
   }
   log_info(&device_dev, "#############################\r\n");
   log_info(&device_dev, "Device ID: 0x%lx 0x%lx 0x%lx\r\n", LL_GetUID_Word0(), LL_GetUID_Word1(),
            LL_GetUID_Word2());
   check_restart_issues();
   log_info(&device_dev, "Flash size: %ldKB\r\n", LL_GetFlashSize());
   log_info(&device_dev, "App size: 0x%lx\r\n", header_app.app_size);
   log_info(&device_dev, "Flash usage: %ld% %%\r\n",
            (header_app.app_size * 100) / ((LL_GetFlashSize() - SBL_SIZE_KB) * 1024));
   log_info(&device_dev, "Flash crc32: 0x%lx\r\n", header_app.crc32);
   log_info(&device_dev, "Calculated crc32: 0x%lx\r\n", LL_CRC_ReadData32(CRC));
   log_info(&device_dev, "Build time: \r\n");
   log_info(&device_dev, "commit: \r\n");
   log_info(&device_dev, "#############################\r\n");
}

Device_Restart_Issue_T Device_Info_Get_Restart_Issue(void)
{
   return restart_issue;
}
