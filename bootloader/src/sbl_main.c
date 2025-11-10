/**
 ********************************************************************************
 * @file    sbl_main.c
 * @author  Mikolaj Pieklo
 * @date    12.08.2025
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include <stdint.h>

#include "stm32f1xx.h"
/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define APP_ADDRESS (0x08000000UL + (SBL_SIZE_KB * 1024U))

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef void (*pFunction)(void);

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
void boot_main(void)
{
   pFunction appEntry;
   uint32_t  appStack;

   /* Get application stack pointer (first entry in vector table) */
   appStack = (uint32_t) *((__IO uint32_t *) APP_ADDRESS);

   /* Get application entry point (second entry in vector table) */
   appEntry = (pFunction) * ((__IO uint32_t *) (APP_ADDRESS + 4U));

   /*Reconfigure vector table offset register to point to the application */
   SCB->VTOR = APP_ADDRESS;

   /* Set main stack pointer */
   __set_MSP(appStack);

   /* Jump to application */
   appEntry();

   while (1)
   {
   }
}