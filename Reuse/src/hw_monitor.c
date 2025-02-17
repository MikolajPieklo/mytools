/**
 ********************************************************************************
 * @file    hw_monitor.c
 * @author  Mikolaj Pieklo
 * @date    29.11.2024
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "hw_monitor.h"

#ifdef STM32F103xB
#include <stm32f1xx.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_pwr.h>
#elif STM32F401xC
#include <stm32f4xx.h>
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_pwr.h>
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

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/

/************************************
 * STATIC FUNCTIONS
 ************************************/
void PVD_IRQHandler(void)
{
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void HW_Software_Reset(void)
{
   NVIC_SystemReset();
}

void HW_PVD_Start(void)
{
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

   LL_PWR_SetPVDLevel(LL_PWR_PVDLEVEL_7);
   NVIC_SetPriority(PVD_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 2));
   NVIC_EnableIRQ(PVD_IRQn);
   LL_PWR_EnablePVD();
}
