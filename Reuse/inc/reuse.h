/**
 ********************************************************************************
 * @file    reuse.h
 * @author  Mikolaj Pieklo
 * @date    23.11.2025
 * @brief
 ********************************************************************************
 */

#ifndef __REUSE_H__
#define __REUSE_H__

#ifdef __cplusplus
extern "C" {
#endif

/************************************
 * INCLUDES
 ************************************/

/************************************
 * MACROS AND DEFINES
 ************************************/
#define REUSE_UNUSED(x) (void) (x)

#define REUSE_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define REUSE_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define REUSE_MAX(a, b) (((a) > (b)) ? (a) : (b))

#define REUSE_SHIFT_LEFT_MASK(val, shift, mask)  (((val) << (shift)) & (mask))
#define REUSE_SHIFT_RIGHT_MASK(val, shift, mask) (((val) >> (shift)) & (mask))

#define REUSE_MASK(val, mask) ((val) & (mask))

/************************************
 * TYPEDEFS
 ************************************/

/************************************
 * EXPORTED VARIABLES
 ************************************/

/************************************
 * GLOBAL FUNCTION PROTOTYPES
 ************************************/


#ifdef __cplusplus
}
#endif

#endif