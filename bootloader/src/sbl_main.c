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

#ifdef STM32F103xB
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_dma.h>
#include <stm32f1xx_ll_exti.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_pwr.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_utils.h>

#include "stm32f1xx.h"
#elif STM32F401xC
#include <stm32f4xx_ll_bus.h>
#include <stm32f4xx_ll_cortex.h>
#include <stm32f4xx_ll_dma.h>
#include <stm32f4xx_ll_exti.h>
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_pwr.h>
#include <stm32f4xx_ll_rcc.h>
#include <stm32f4xx_ll_system.h>
#include <stm32f4xx_ll_utils.h>

#include "stm32f4xx.h"
#else
#error Module not supported!
#endif

#include "WS25Qxx.h"
#include "uart.h"
#include <circual_buffer.h>
#include <log.h>
#include <spi.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define APP_ADDRESS          (0x08000000UL + (SBL_SIZE_KB * 1024U))
#define NVIC_PRIORITYGROUP_4 ((uint32_t) 0x00000003)

#define LED_Port GPIOB
#define LED_Pin  LL_GPIO_PIN_12

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef void (*pFunction)(void);

/************************************
 * STATIC VARIABLES
 ************************************/
/* Dummy device */
static const struct device sbl_dev = {
   .name = "BOOT",
};

CirBuff_T cb_uart1_tx = {.tail = 0, .head = 0, .USARTx = USART1};
CirBuff_T cb_uart1_rx = {.tail = 0, .head = 0, .USARTx = USART1};

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
void SystemClock_Config(void);

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

#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
#endif
   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

   /* System interrupt init*/
   NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

   /* SysTick_IRQn interrupt configuration */
   NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 15, 0));

#ifdef STM32F103xB
   /* NOJTAG: JTAG-DP Disabled and SW-DP Enabled */
   LL_GPIO_AF_Remap_SWJ_NOJTAG();
#endif

   /* Configure the system clock */
   SystemClock_Config();
   LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
   LL_Init1msTick(72000000);
   // SysTick_Config(SystemCoreClock / 1000);
   LL_SYSTICK_EnableIT();

   LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

/* GPIO Ports Clock Enable */
#ifdef STM32F103xB
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
#elif STM32F401xC
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
#endif

   /**/
   LL_GPIO_ResetOutputPin(LED_Port, LED_Pin);

   /**/
   GPIO_InitStruct.Pin = LED_Pin;
   GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
   GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
   GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
   LL_GPIO_Init(LED_Port, &GPIO_InitStruct);

   UART1_Init();
   /* Clear terminal */
   log_info(&sbl_dev, "\x1b[2J\x1b[H");
   log_info(&sbl_dev, "\r\n");

   log_info(&sbl_dev, "Bootloader\r\n");

   SPI1_Init();
   WS25Qxx_Init();

   while (cb_uart1_tx.head != cb_uart1_tx.tail)
   {
      // Wait for UART TX end
      LL_mDelay(1);
   }

   /* Disable SysTick */
   SysTick->CTRL = 0;
   SysTick->LOAD = 0;
   SysTick->VAL = 0;

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

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
   (void) file;
   (void) line;
   // log_info(&main_dev, "Wrong parameters value: file %s on line %ld\r\n", file, line);
}
#endif /* USE_FULL_ASSERT */

void SystemClock_Config(void)
{
#ifdef STM32F103xB
   LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
   while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
   {
   }
   LL_FLASH_EnablePrefetch();

   LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
   while (LL_RCC_HSE_IsReady() != 1)
   {
   }
   LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
   LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
   while (LL_RCC_PLL_IsReady() != 1)
   {
   }
   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
   LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
   LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
   LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
   while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
   {
   }

   SystemCoreClockUpdate();
   // LL_SetSystemCoreClock(72000000);
#elif STM32F401xC
   LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
   while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
   {
   }
   LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE2);
   LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
   while (LL_RCC_HSE_IsReady() != 1)
   {
   }
   LL_RCC_HSE_EnableCSS();
   LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 168, LL_RCC_PLLP_DIV_2);
   LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
   while (LL_RCC_PLL_IsReady() != 1)
   {
   }
   while (LL_PWR_IsActiveFlag_VOS() == 0)
   {
   }
   LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
   LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
   LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
   LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
   while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
   {
   }
   LL_Init1msTick(84000000);
   LL_SetSystemCoreClock(84000000);
   LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
#endif
}