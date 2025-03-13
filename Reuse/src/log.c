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

#include <log.h>
#include <string.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define define_log_printk_level(func, kern_level)                                           \
   void func(const struct device *dev, const char *fmt, ...)                                \
   {                                                                                        \
      int     length;                                                                       \
      char    buff[256];                                                                    \
      va_list va;                                                                           \
      va_start(va, fmt);                                                                    \
      length = vsnprintf(buff, sizeof(buff), fmt, va);                                      \
      va_end(va);                                                                           \
                                                                                            \
      const char *color_code = "";                                                          \
      if (strcmp(kern_level, KERN_ERR) == 0)                                                \
      {                                                                                     \
         color_code = "\033[31m"; /* red color */                                           \
      }                                                                                     \
      if (strcmp(kern_level, KERN_WARNING) == 0)                                            \
      {                                                                                     \
         color_code = "\033[33m"; /* yellow color */                                        \
      }                                                                                     \
      if (strcmp(kern_level, KERN_NOTICE) == 0)                                             \
      {                                                                                     \
         color_code = "\033[34m"; /* blue color */                                          \
      }                                                                                     \
                                                                                            \
      printf("%s%s: %s\033[0m", color_code, (dev && dev->name) ? dev->name : "NULL", buff); \
                                                                                            \
      (void) length;                                                                        \
   }

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
