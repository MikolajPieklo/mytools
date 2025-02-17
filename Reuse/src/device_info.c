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
      printf("Reset cause: HSECSS\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_IWDGRST())
   {
      printf("Reset cause: IWDGRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_LPWRRST())
   {
      printf("Reset cause: LPWRRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_PINRST())
   {
      printf("Reset cause: PINRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_PORRST())
   {
      printf("Reset cause: PORRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_SFTRST())
   {
      printf("Reset cause: SFTRST\r\n");
   }
   if (true == LL_RCC_IsActiveFlag_WWDGRST())
   {
      printf("Reset cause: WWDGRST\r\n");
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

   printf("\x1b[2J\x1b[H");
   printf("\r\n");

   for (crc_idx = 0; crc_idx < text_size / 4; crc_idx++)
   {
      LL_CRC_FeedData32(CRC, (uint32_t) * (&_stext + crc_idx));
      // printf("%lx\r\n", *(&_stext + crc_idx));
   }


   printf("#############################\r\n");
   printf("Device ID: 0x%lx 0x%lx 0x%lx\r\n", LL_GetUID_Word0(), LL_GetUID_Word1(),
          LL_GetUID_Word2());
   check_restart_issues();
   printf("Flash size: %ldKB\r\n", LL_GetFlashSize());
   printf("Program size: 0x%lx\r\n", text_size);
   printf("Flash usage: \r\n");
   printf("Flash crc32: 0x%lx\r\n", program_info.text_sec_crc32);
   printf("Calculated crc32: 0x%lx\r\n", LL_CRC_ReadData32(CRC));
   printf("Build time: \r\n");
   printf("commit: \r\n");
   printf("#############################\r\n");
}
