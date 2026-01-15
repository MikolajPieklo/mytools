/*
 * rtc.c
 *
 *  Created on: Jul 01, 2023
 *      Author: mkpk
 */


#include "rtc.h"

#include <stdio.h>

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_exti.h>
#include <stm32f1xx_ll_pwr.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_rtc.h>

#include <string.h>

#define RTC_ERROR_NONE 0
/* ck_apre=LSIFreq/(ASYNC prediv + 1) with LSIFreq=40kHz RC */
#define RTC_ASYNCH_PREDIV ((uint32_t) 0x9C3F)

/* Time Structure definition */
struct time_t
{
   uint8_t sec;
   uint8_t min;
   uint8_t hour;
};
struct time_t RTC_TimeStruct;

struct date_t
{
   uint8_t month;
   uint8_t day;
   uint8_t year;
};
struct date_t RTC_DateStruct;

uint8_t EndOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/* Buffers used for displaying Time and Date */
uint32_t timeCounter = 0;
uint8_t  dateUpdate = 0;
uint8_t  timeUpdate = 0;
uint8_t  aShowTime[13] = {0};
uint8_t  aShowDate[13] = {0};

static void     configure_rtc(void);
static void     configure_rtc_calendar(void);
static void     rtc_date_config(uint8_t fDate, uint8_t fMonth, uint8_t fYear);
static void     rtc_time_config(uint8_t fHour, uint8_t fMin, uint8_t fSec);
static void     rtc_time_structupadate(void);
static void     rtc_date_structupdate(void);
static void     calendar_callback(void);
static uint32_t waitforsynchro_rtc(void);

void RTC_IRQHandler(void)
{
   if (LL_RTC_IsEnabledIT_SEC(RTC) != 0)
   {
      /* Clear the RTC Second interrupt */
      LL_RTC_ClearFlag_SEC(RTC);
      calendar_callback();
      /* Wait until last write operation on RTC registers has finished */
      LL_RTC_WaitForSynchro(RTC);
   }
   /* Clear the EXTI's Flag for RTC Alarm */
   LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
}

void RTC_Init(void)
{
   /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
   /* To change the source clock of the RTC feature (LSE, LSI,HSE_DIV128), you have to:
      - Enable the power clock
      - Enable write access to configure the RTC clock source (to be done once after reset).
      - Reset the Back up Domain
      - Configure the needed RTC clock source */

   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_BKP);

   LL_PWR_EnableBkUpAccess();

   /*##-2- Configure LSI as RTC clock source ###############################*/
   /* Enable LSI */
   LL_RCC_LSI_Enable();
   while (LL_RCC_LSI_IsReady() != 1)
   {
   }
   /* Reset backup domain only if LSI is not yet selected as RTC clock source */
   if (LL_RCC_GetRTCClockSource() != LL_RCC_RTC_CLKSOURCE_LSI)
   {
      LL_RCC_ForceBackupDomainReset();
      LL_RCC_ReleaseBackupDomainReset();
      LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
   }

   configure_rtc();
   configure_rtc_calendar();
}

void Show_RTC_Calendar(void)
{
   uint8_t i = 0;
   rtc_time_structupadate();
   rtc_date_structupdate();

   /* Note: need to convert in decimal value in using __LL_RTC_CONVERT_BCD2BIN helper macro */
   /* Display time Format : hh:mm:ss */
   sprintf((char *) aShowTime, "%.2d:%.2d:%.2d", RTC_TimeStruct.hour, RTC_TimeStruct.min,
           RTC_TimeStruct.sec);
   for (i = 0; i < sizeof(aShowTime); i++)
   {
      if (aShowTime[i] == '\0')
      {
         break;
      }
      printf("%c", aShowTime[i]);
   }
   printf("\r\n");

   /* Display date Format : mm-dd-yy */
   sprintf((char *) aShowDate, "%.2d-%.2d-%.2d", RTC_DateStruct.day, RTC_DateStruct.month,
           (2000 + RTC_DateStruct.year));
}

void RTC_Get_Time(uint8_t *time)
{
   rtc_time_structupadate();
   rtc_date_structupdate();
   time[0] = RTC_TimeStruct.hour;
   time[1] = RTC_TimeStruct.min;
   time[2] = RTC_TimeStruct.sec;
}

void Get_RTC_Time(uint8_t *tab)
{
   uint8_t i = 0;
   rtc_time_structupadate();
   rtc_date_structupdate();

   /* Note: need to convert in decimal value in using __LL_RTC_CONVERT_BCD2BIN helper macro */
   /* Display time Format : hh:mm:ss */
   sprintf((char *) aShowTime, "%.2d:%.2d:%.2d", RTC_TimeStruct.hour, RTC_TimeStruct.min,
           RTC_TimeStruct.sec);
   memcpy(tab, aShowTime, 8);
}

/**
 * @brief  Configure RTC.
 * @note   Peripheral configuration is minimal configuration from reset values.
 *         Thus, some useless LL unitary functions calls below are provided as
 *         commented examples - setting is default configuration from reset.
 * @param  None
 * @retval None
 */
static void configure_rtc(void)
{
   /*##-1- Enable RTC peripheral Clocks #######################################*/
   /* Enable RTC Clock */
   LL_RCC_EnableRTC();

   /*##-2- Disable RTC registers write protection ##############################*/
   LL_RTC_DisableWriteProtection(RTC);

   /*##-3- Enter in initialization mode #######################################*/
   if (LL_RTC_EnterInitMode(RTC) != RTC_ERROR_NONE)
   {
      /* Initialization Error */
   }

   /*##-4- Configure RTC ######################################################*/
   /* Configure RTC prescaler */
   /* Set Asynch Prediv (value according to source clock) */
   LL_RTC_SetAsynchPrescaler(RTC, RTC_ASYNCH_PREDIV);

   /* RTC_Alarm Interrupt Configuration: EXTI configuration */
   LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
   LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);

   /*##-5- Configure the NVIC for RTC Alarm ###############################*/
   NVIC_SetPriority(RTC_IRQn, 0);
   NVIC_EnableIRQ(RTC_IRQn);

   /*##-6- Exit of initialization mode #######################################*/
   LL_RTC_ExitInitMode(RTC);

   /*##-7- Enable RTC registers write protection #############################*/
   LL_RTC_EnableWriteProtection(RTC);
}

/**
 * @brief  Configure the current time and date.
 * @param  None
 * @retval None
 */
static void configure_rtc_calendar(void)
{
   /*##-1- Disable RTC registers write protection ############################*/
   LL_RTC_DisableWriteProtection(RTC);

   /*##-2- Enter in initialization mode ######################################*/
   if (LL_RTC_EnterInitMode(RTC) != RTC_ERROR_NONE)
   {
      /* Initialization Error */
   }

   /*##-3- Configure the Date ################################################*/
   /* Note: __LL_RTC_CONVERT_BIN2BCD helper macro can be used if user wants to*/
   /*       provide directly the decimal value:                               */
   /*       LL_RTC_DATE_Config(RTC, ,                                         */
   /*                          __LL_RTC_CONVERT_BIN2BCD(31), (...))           */
   /* Set Date: 31 December 2017 */
   rtc_date_config(01, 01, 70);

   /*##-4- Configure the Time ################################################*/
   /* Set Time: 23:59:55 */
   rtc_time_config(00, 00, 00);

   /* Enable Second Interrupt */
   LL_RTC_EnableIT_SEC(RTC);

   /*##-5- Exit of initialization mode #######################################*/
   if (LL_RTC_ExitInitMode(RTC) != RTC_ERROR_NONE)
   {
      /* Initialization Error */
   }

   /*##-6- Enable RTC registers write protection #############################*/
   LL_RTC_EnableWriteProtection(RTC);
}

/**
 * @brief  Configure RTC_DATE Structure
 * @param  fDate:  Date
 *         fMonth: Month
 *         fYear:  Year
 * @retval None
 */
static void rtc_date_config(uint8_t fDate, uint8_t fMonth, uint8_t fYear)
{
   RTC_DateStruct.day = fDate;
   RTC_DateStruct.month = fMonth;
   RTC_DateStruct.year = fYear;
}

/**
 * @brief  Configure RTC_TIME Structure
 * @param  fHour: Hour
 *         fMin:  Minut
 *         fSec:  seconds
 * @retval None
 */
static void rtc_time_config(uint8_t fHour, uint8_t fMin, uint8_t fSec)
{
   RTC_TimeStruct.hour = fHour;
   RTC_TimeStruct.min = fMin;
   RTC_TimeStruct.sec = fSec;

   LL_RTC_TIME_Set(RTC,
                   ((RTC_TimeStruct.hour * 3600) + (RTC_TimeStruct.min * 60) + RTC_TimeStruct.sec));
}

/**
 * @brief  Configure RTC_TIME Structure
 * @param  None
 * @retval None
 */
static void rtc_time_structupadate(void)
{
   if (timeUpdate != 0)
   {
      timeCounter = LL_RTC_TIME_Get(RTC);
      timeUpdate = 0;
      RTC_TimeStruct.hour = (timeCounter / 3600);
      RTC_TimeStruct.min = (timeCounter % 3600) / 60;
      RTC_TimeStruct.sec = (timeCounter % 3600) % 60;
   }
}

/**
 * @brief  Update RTC_Date Structure
 * @param  None
 * @retval None
 */
static void rtc_date_structupdate(void)
{
   /* Update DATE when Time is 23:59:59 */
   if ((timeCounter == 0x0001517FU) & (dateUpdate != 0U))
   {
      dateUpdate = 0;
      if (RTC_DateStruct.day == EndOfMonth[RTC_DateStruct.month - 1U])
      {
         RTC_DateStruct.day = 1U;
         if (RTC_DateStruct.month == 12U)
         {
            RTC_DateStruct.month = 1U;
            RTC_DateStruct.year += 1U;
         }
         else
         {
            RTC_DateStruct.month += 1U;
         }
      }
      else
      {
         RTC_DateStruct.day = RTC_DateStruct.day + 1U;
      }
   }
}

/**
 * @brief  Calendar callback
 * @param  None
 * @retval None
 */
static void calendar_callback(void)
{
   timeCounter = LL_RTC_TIME_Get(RTC);
   timeUpdate = 1;
   /* Reset RTC Counter when Time is 23:59:59 */
   if (timeCounter == 0x0001517FU)
   {
      dateUpdate = 1;
      LL_RTC_DisableWriteProtection(RTC);
      if (LL_RTC_EnterInitMode(RTC) != RTC_ERROR_NONE)
      {
         /* Initialization Error */
      }
      LL_RTC_TIME_Set(RTC, 0x0U);
      /* Wait until last write operation on RTC registers has finished */
      waitforsynchro_rtc();
      if (LL_RTC_ExitInitMode(RTC) != RTC_ERROR_NONE)
      {
         /* Initialization Error */
      }
      LL_RTC_EnableWriteProtection(RTC);
   }
}

/**
 * @brief  Wait until the RTC Time and Date registers (RTC_TR and RTC_DR) are
 *         synchronized with RTC APB clock.
 * @param  None
 * @retval RTC_ERROR_NONE if no error (RTC_ERROR_TIMEOUT will occur if RTC is
 *         not synchronized)
 */
static uint32_t waitforsynchro_rtc(void)
{
   /* Clear RSF flag */
   LL_RTC_ClearFlag_RS(RTC);

   /* Wait the registers to be synchronised */
   while (LL_RTC_IsActiveFlag_RS(RTC) != 1)
   {
   }
   return RTC_ERROR_NONE;
}
