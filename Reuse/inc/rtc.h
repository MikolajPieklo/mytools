/*
 * rtc.h
 *
 *  Created on: Jun 1, 2023
 *      Author: mkpk
 */

#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void RTC_Init(void);

/**
 * @brief  Display the current time and date.
 * @param  None
 * @retval None
 */
void RTC_Get_Time(uint8_t *time);

void Show_RTC_Calendar(void);

void Get_RTC_Time(uint8_t *tab);

#ifdef __cplusplus
}
#endif
#endif /* __RTC_H__ */
