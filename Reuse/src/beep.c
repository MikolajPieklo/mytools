/**
 ********************************************************************************
 * @file    beep.c
 * @author  Mikolaj Pieklo
 * @date    26.10.2025
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "beep.h"

#ifdef STM32F103xB
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_pwr.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_rtc.h>
#elif STM32F401xC
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_pwr.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_rtc.h>
#else
#error Module not supported!
#endif


#include "delay.h"

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
typedef enum
{
   BEEP_STS_OFF,
   BEEP_STS_ON,
   BEEP_STS_DELAY,
} beep_sts_t;

static beep_sts_t beep_sts = BEEP_STS_OFF;
static uint32_t   beep_ts_ms = 0U;

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void Beep_Init(void)
{
#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
#elif STM32F401xC
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
#endif

   LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_6, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_6, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_DOWN);

// Inicjalizacja domeny backup (po włączeniu PWR + BKP)
#ifdef STM32F103xB
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR | LL_APB1_GRP1_PERIPH_BKP);
#elif STM32F401xC
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
#endif
   LL_PWR_EnableBkUpAccess(); // pozwala pisać
}

void Beep_Pulse(uint8_t ms)
{
   LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);
   TS_Delay_ms(ms);
   LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
}

void Beep_Set_Warning(void)
{
#ifdef STM32F103xB
   LL_RTC_BKP_SetRegister(BKP, LL_RTC_BKP_DR1, 0x01);
#elif STM32F401xC
   LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR1, 0x01);
#endif
}

void Beep_Clear_Warning(void)
{
#ifdef STM32F103xB
   LL_RTC_BKP_SetRegister(BKP, LL_RTC_BKP_DR1, 0x00);
#elif STM32F401xC
   LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR1, 0x00);
#endif
}

uint16_t Beep_Get_Warning_Status(void)
{
#ifdef STM32F103xB
   return LL_RTC_BKP_GetRegister(BKP, LL_RTC_BKP_DR1);
#elif STM32F401xC
   return LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR1);
#endif
}

void Beep_Task(void)
{
#ifdef STM32F103xB
   if (0x01 == LL_RTC_BKP_GetRegister(BKP, LL_RTC_BKP_DR1))
#elif STM32F401xC
   if (0x01 == LL_RTC_BAK_GetRegister(RTC, LL_RTC_BKP_DR1))
#endif
   {
      switch (beep_sts)
      {
      case BEEP_STS_OFF:
         LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);
         beep_sts = BEEP_STS_ON;
         beep_ts_ms = TS_Get_ms();
         break;
      case BEEP_STS_ON:
         if (TS_Get_ms() >= beep_ts_ms + 200)
         {
            LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
            beep_sts = BEEP_STS_DELAY;
            beep_ts_ms = TS_Get_ms();
         }
         break;
      case BEEP_STS_DELAY:
         if (TS_Get_ms() >= beep_ts_ms + 500)
         {
            beep_sts = BEEP_STS_OFF;
            beep_ts_ms = TS_Get_ms();
         }
         break;
      default:
         beep_sts = BEEP_STS_OFF;
         LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
         break;
      }
   }
}