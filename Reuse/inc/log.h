/**
 ********************************************************************************
 * @file    log.h
 * @author  mkpk
 * @date    25.03.13
 * @brief
 ********************************************************************************
 */

#ifndef __LOG_H__
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/

/************************************
 * MACROS AND DEFINES
 ************************************/
struct device
{
   const char *name;
};

#define KERN_SOH       "\001" /* ASCII Start Of Header */
#define KERN_SOH_ASCII '\001'

#define KERN_ERR     KERN_SOH "0" /* error conditions */
#define KERN_WARNING KERN_SOH "1" /* warning conditions */
#define KERN_NOTICE  KERN_SOH "2" /* normal but significant condition */
#define KERN_INFO    KERN_SOH "3" /* informational */
#define KERN_DEBUG   KERN_SOH "4" /* debug-level messages */

#define KERN_DEFAULT KERN_SOH "d" /* the default kernel loglevel */

void log_printk(const char *level, const struct device *dev, const char *fmt, ...);

void log_err(const struct device *dev, const char *fmt, ...);
void log_warn(const struct device *dev, const char *fmt, ...);
void log_notice(const struct device *dev, const char *fmt, ...);
void log_info(const struct device *dev, const char *fmt, ...);
void log_dbg(const struct device *dev, const char *fmt, ...);
/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/
#ifdef USED_RTOS
void Log_Init(void);
#endif

void Log_Ascii_Buffer(const struct device *dev, const uint8_t *buffer, uint8_t length);
void Log_Hex_Buffer(const struct device *dev, const uint8_t *buffer, uint8_t length);

#ifdef __cplusplus
}
#endif

#endif
