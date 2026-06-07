/**
 ********************************************************************************
 * @file    log.c
 * @author  mkpk
 * @date    25.03.13
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include <stdarg.h>
#include <stdio.h>

#include <circual_buffer.h>
#include <log.h>
#include <rtc.h>
#include <string.h>

#ifdef USED_RTOS
#include "FreeRTOS.h"
#include "semphr.h"
#endif
/************************************
 * EXTERN VARIABLES
 ************************************/
extern CirBuff_T cb_uart1_tx;
/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#ifdef USED_RTOS
static SemaphoreHandle_t log_mutex = NULL;
#endif

#if !defined(USED_RTOS) || defined(SBL_BUILD)
#define define_log_printk_level(func_name, kern_level_str)                                \
   void func_name(const struct device *dev, const char *fmt, ...)                         \
   {                                                                                      \
      char        buff[256];                                                              \
      uint8_t     timestamp[20] = {0};                                                    \
      va_list     va;                                                                     \
      const char *color = "";                                                             \
      const char *level_name = kern_level_str;                                            \
      const char *dev_name = (dev && dev->name) ? dev->name : "NULL";                     \
                                                                                          \
      Get_RTC_Time(timestamp);                                                            \
                                                                                          \
      if (strcmp(kern_level_str, KERN_ERR) == 0)                                          \
      {                                                                                   \
         color = "\033[31m"; /* red */                                                    \
      }                                                                                   \
      else if (strcmp(kern_level_str, KERN_WARNING) == 0)                                 \
      {                                                                                   \
         color = "\033[33m"; /* yellow */                                                 \
      }                                                                                   \
      else if (strcmp(kern_level_str, KERN_NOTICE) == 0)                                  \
      {                                                                                   \
         color = "\033[34m"; /* blue */                                                   \
      }                                                                                   \
      else                                                                                \
      {                                                                                   \
         color = "\033[37m"; /* white */                                                  \
      }                                                                                   \
                                                                                          \
      va_start(va, fmt);                                                                  \
      int len = vsnprintf(buff, sizeof(buff), fmt, va);                                   \
      va_end(va);                                                                         \
                                                                                          \
      printf("%s[%s] %s    %s: %s\033[0m", color, level_name, timestamp, dev_name, buff); \
                                                                                          \
      if ((uint32_t) len >= sizeof(buff))                                                 \
      {                                                                                   \
         printf("\033[35m[WARNING] Log message truncated!\033[0m");                       \
      }                                                                                   \
   }
#else
#define define_log_printk_level(func_name, kern_level_str)                                 \
   void func_name(const struct device *dev, const char *fmt, ...)                          \
   {                                                                                       \
      if (log_mutex == NULL)                                                               \
         return;                                                                           \
                                                                                           \
      if (xSemaphoreTake(log_mutex, portMAX_DELAY) == pdTRUE)                              \
      {                                                                                    \
         static char buff[256]; /* Static, to avoid stack overflow */                      \
         uint8_t     timestamp[20] = {0};                                                  \
         va_list     va;                                                                   \
         const char *color = "";                                                           \
         const char *level_name = kern_level_str;                                          \
         const char *dev_name = (dev && dev->name) ? dev->name : "NULL";                   \
                                                                                           \
         Get_RTC_Time(timestamp);                                                          \
                                                                                           \
         if (strcmp(kern_level_str, KERN_ERR) == 0)                                        \
         {                                                                                 \
            color = "\033[31m"; /* red */                                                  \
         }                                                                                 \
         else if (strcmp(kern_level_str, KERN_WARNING) == 0)                               \
         {                                                                                 \
            color = "\033[33m"; /* yellow */                                               \
         }                                                                                 \
         else if (strcmp(kern_level_str, KERN_NOTICE) == 0)                                \
         {                                                                                 \
            color = "\033[34m"; /* blue */                                                 \
         }                                                                                 \
         else                                                                              \
         {                                                                                 \
            color = "\033[37m"; /* white */                                                \
         }                                                                                 \
                                                                                           \
         va_start(va, fmt);                                                                \
         int len = vsnprintf(buff, sizeof(buff), fmt, va);                                 \
         va_end(va);                                                                       \
                                                                                           \
         char final[256];                                                                  \
         int  final_len = snprintf(final, sizeof(final), "%s[%s] %s %s: %s\033[0m", color, \
                                   level_name, timestamp, dev_name, buff);                 \
         CirBuff_Insert_Text(&cb_uart1_tx, (uint8_t *) final, final_len);                  \
                                                                                           \
         if ((uint32_t) len >= sizeof(buff))                                               \
         {                                                                                 \
            printf("\033[35m[WARNING] Log message truncated!\033[0m");                     \
         }                                                                                 \
                                                                                           \
         xSemaphoreGive(log_mutex);                                                        \
      }                                                                                    \
   }
#endif

define_log_printk_level(log_err, KERN_ERR);
define_log_printk_level(log_warn, KERN_WARNING);
define_log_printk_level(log_notice, KERN_NOTICE);
define_log_printk_level(log_info, KERN_INFO);
define_log_printk_level(log_dbg, KERN_DEBUG);
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

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
#ifdef USED_RTOS
void Log_Init(void)
{
   log_mutex = xSemaphoreCreateMutex();
}
#endif


void Log_Hex_Buffer(const struct device *dev, const uint8_t *buffer, uint8_t length)
{
   static const char hex[] = "0123456789ABCDEF";
   static char       hex_storage[(64 * 3) + 1];
   char             *hex_str = hex_storage;

   if ((buffer == NULL) || (length == 0))
   {
      log_dbg(dev, "Buffer: %s\r\n", "<empty>");
      return;
   }

   for (uint8_t i = 0; i < length; i++)
   {
      hex_str[i * 3] = hex[(buffer[i] >> 4) & 0x0F];
      hex_str[(i * 3) + 1] = hex[buffer[i] & 0x0F];
      hex_str[(i * 3) + 2] = (i + 1u < length) ? ' ' : '\0';
   }

   log_dbg(dev, "Buffer: %s\r\n", hex_str);
}

void Log_Ascii_Buffer(const struct device *dev, const uint8_t *buffer, uint8_t length)
{
   static char ascii_storage[64 + 1];

   if ((buffer == NULL) || (length == 0))
   {
      log_dbg(dev, "Buffer: %s\r\n", "<empty>");
      return;
   }

   uint8_t copy_len = length;
   if (copy_len > 64u)
   {
      copy_len = 64u;
   }

   for (uint8_t i = 0; i < copy_len; i++)
   {
      if ((buffer[i] >= 32u) && (buffer[i] <= 126u))
      {
         ascii_storage[i] = (char) buffer[i];
      }
      else
      {
         ascii_storage[i] = '.';
      }
   }
   ascii_storage[copy_len] = '\0';

   log_dbg(dev, "Buffer: %s\r\n", ascii_storage);
}
