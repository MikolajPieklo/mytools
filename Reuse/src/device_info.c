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

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef struct program_info_typedef
{
   uint32_t text_sec_crc32;
   uint32_t data;
   uint32_t size;
} program_info_t;

__attribute__((section(".program_info_section"))) volatile const program_info_t program_info =
    {.text_sec_crc32 = 0xABCDEFAB, .data = 0x63, .size = 0xFFFFFFFF};

/************************************
 * STATIC VARIABLES
 ************************************/

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
   }
   if (true == LL_RCC_IsActiveFlag_IWDGRST())
   {
      log_info(&device_dev, "Reset cause: IWDGRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_LPWRRST())
   {
      log_info(&device_dev, "Reset cause: LPWRRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_PINRST())
   {
      log_info(&device_dev, "Reset cause: PINRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_PORRST())
   {
      log_info(&device_dev, "Reset cause: PORRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_SFTRST())
   {
      log_info(&device_dev, "Reset cause: SFTRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_WWDGRST())
   {
      log_info(&device_dev, "Reset cause: WWDGRST\r\n");
   }
   LL_RCC_ClearResetFlags();
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void Device_Info(void)
{
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);

   extern uint32_t _etext;
   extern uint32_t _stext;

   LL_CRC_ResetCRCCalculationUnit(CRC);

   uint32_t crc_idx = 0;
   uint32_t text_size = (uint32_t) &_etext - (uint32_t) &_stext;

   log_info(&device_dev, "\x1b[2J\x1b[H");
   log_info(&device_dev, "\r\n");

   for (crc_idx = 0; crc_idx < text_size / 4; crc_idx++)
   {
      LL_CRC_FeedData32(CRC, (uint32_t) * (&_stext + crc_idx));
   }

   log_info(&device_dev, "#############################\r\n");
   log_info(&device_dev, "Device ID: 0x%lx 0x%lx 0x%lx\r\n", LL_GetUID_Word0(), LL_GetUID_Word1(),
            LL_GetUID_Word2());
   check_restart_issues();
   log_info(&device_dev, "Flash size: %ldKB\r\n", LL_GetFlashSize());
   log_info(&device_dev, "Program size: 0x%lx\r\n", text_size);
   log_info(&device_dev, "Flash usage: \r\n");
   log_info(&device_dev, "Flash crc32: 0x%lx\r\n", program_info.text_sec_crc32);
   log_info(&device_dev, "Calculated crc32: 0x%lx\r\n", LL_CRC_ReadData32(CRC));
   log_info(&device_dev, "Build time: \r\n");
   log_info(&device_dev, "commit: \r\n");
   log_info(&device_dev, "#############################\r\n");
}
