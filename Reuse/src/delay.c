/**
 ********************************************************************************
 * @file    delay.c
 * @author  Mikolaj Pieklo
 * @date    12.11.2024
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "delay.h"

#include <stdbool.h>

#ifdef STM32F103xB
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_tim.h>
#elif STM32F401xC
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_tim.h>
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
static bool   timer_us_is_initialized = false;
volatile bool is_tim_overload = 0;
/************************************
 * GLOBAL VARIABLES
 ************************************/
volatile uint32_t SysTickValue;

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/

/************************************
 * STATIC FUNCTIONS
 ************************************/
/**
 * @brief  This function handles TIM4 update interrupt.
 * @param  None
 * @retval None
 */
void TIM4_IRQHandler(void)
{
   /* Check whether update interrupt is pending */
   if (LL_TIM_IsActiveFlag_UPDATE(TIM4) == 1)
   {
      /* Clear the update interrupt flag*/
      LL_TIM_ClearFlag_UPDATE(TIM4);
      is_tim_overload = true;
      LL_TIM_DisableCounter(TIM4);
   }
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void TS_Delay_ms(uint32_t delay_ms)
{
   uint32_t tickstart = SysTickValue;
   while ((SysTickValue - tickstart) < delay_ms)
   {
      __NOP();
   }
}

void TS_Delay_us_Init(void)
{
   /* Enable the timer peripheral clock */
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);

   /* Set counter mode */
   LL_TIM_SetCounterMode(TIM4, LL_TIM_COUNTERMODE_UP);

   /*
     Prescaler = (TIM4CLK / TIM4 counter clock) - 1
     Prescaler = (36MHz / 1 MHz) - 1
   */
   LL_TIM_SetPrescaler(TIM4, 35);
   LL_TIM_SetAutoReload(TIM4, 0);

   /* Clear pending flags */
   is_tim_overload = false;

   /* Enable the update interrupt */
   LL_TIM_EnableIT_UPDATE(TIM4);

   /* Configure the NVIC to handle TIM2 update interrupt */
   NVIC_SetPriority(TIM4_IRQn, 0);
   NVIC_EnableIRQ(TIM4_IRQn);

   /* Force update generation */
   LL_TIM_GenerateEvent_UPDATE(TIM4);
   LL_TIM_ClearFlag_UPDATE(TIM4);

   timer_us_is_initialized = true;
}

void TS_Delay_us(uint32_t delay_us)
{
   do
   {
      if (false == timer_us_is_initialized)
      {
         break;
      }

      if (1 == delay_us)
      {
         delay_us = 2;
      }

      LL_TIM_SetAutoReload(TIM4, delay_us - 1);
      LL_TIM_SetCounter(TIM4, 0);
      is_tim_overload = false;
      LL_TIM_ClearFlag_UPDATE(TIM4);
      /* Enable counter */
      LL_TIM_EnableCounter(TIM4);

      while (!is_tim_overload)
      {
         __NOP();
      }
      is_tim_overload = false;
      LL_TIM_ClearFlag_UPDATE(TIM4);
   }
   while (0);
}

uint32_t TS_Get_ms(void)
{
   return SysTickValue;
}
