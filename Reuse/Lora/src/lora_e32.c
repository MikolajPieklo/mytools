/**
 ********************************************************************************
 * @file    lora_e32.c
 * @author  Mikolaj Pieklo
 * @date    10.10.2024
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "lora_e32.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_exti.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>

#include <circual_buffer.h>
#include <crc16.h>
#include <delay.h>
/************************************
 * EXTERN VARIABLES
 ************************************/
#if 0
extern volatile uint32_t irq_aux_nr;
extern volatile uint32_t irq_uart_nr;
extern volatile CirBuff_T cb_uart1_tx;
#endif

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#if defined(LORA_E32_RX) || defined(LORA_E32_TX)

#define DELAY_TIME_AFTER_RESET_MS 3500
#define DELAY_TX_TIME_MS          1000

#define LORA_RX_DEVICE_ADDRESS 0x0101
#define LORA_RX_CHANEL         0x17
#define LORA_TX_DEVICE_ADDRESS 0x0202
#define LORA_TX_CHANEL         0x17

#define LORA_GPIO_M0_PIN   LL_GPIO_PIN_13
#define LORA_GPIO_M0_PORT  GPIOC
#define LORA_GPIO_M1_PIN   LL_GPIO_PIN_1
#define LORA_GPIO_M1_PORT  GPIOB
#define LORA_GPIO_AUX_PIN  LL_GPIO_PIN_0
#define LORA_GPIO_AUX_PORT GPIOB

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
enum Lora_Machine_State
{
   lora_machine_state_uninitialized,
   lora_machine_state_reset,
   lora_machine_state_get_chip_id,
   lora_machine_state_set_configuration,
   lora_machine_state_get_configuration,
   lora_machine_state_rx,
   lora_machine_state_tx,
   lora_machine_state_service,
   lora_machine_state_size
};

enum Lora_Aux_State
{
   lora_aux_state_busy,
   lora_aux_state_idle,
   lora_aux_state_size
};

enum Lora_Uart_State
{
   lora_uart_state_idle,
   lora_uart_state_rx_busy,
   lora_uart_state_rx_done,
   lora_uart_state_tx_busy,
   lora_uart_state_tx_done,
   lora_uart_state_size
};

typedef enum Lora_Mode_Typedef
{
   lora_mode_normal,
   lora_mode_wake_up,
   lora_mode_power_saving,
   lora_mode_sleep,
   lora_mode_size
} Lora_Mode_T;

typedef struct Lora_Uart_Typedef
{
   CirBuff_T buff_rx;
   CirBuff_T buff_tx;
   uint8_t   state;
} Lora_Uart_t;

typedef struct Lora_Typedef
{
   uint32_t    machine_state;
   uint32_t    aux_state;
   uint32_t    mode;
   uint32_t    por_time;
   uint32_t    tx_time;
   uint32_t    tx_counter;
   uint32_t    rx_counter;
   Lora_Uart_t uart;

} Lora_t;

/************************************
 * STATIC VARIABLES
 ************************************/
static volatile Lora_t lora = {.machine_state = lora_machine_state_uninitialized,
                               .aux_state = lora_aux_state_idle,
                               .uart.state = lora_uart_state_idle,
                               .uart.buff_rx.head = 0,
                               .uart.buff_rx.tail = 0,
                               .uart.buff_rx.size = CIRCUAL_BUFFER_SIZE,
                               .uart.buff_rx.USARTx = USART2,
                               .uart.buff_tx.head = 0,
                               .uart.buff_tx.tail = 0,
                               .uart.buff_tx.size = CIRCUAL_BUFFER_SIZE,
                               .uart.buff_tx.USARTx = USART2,
                               .rx_counter = 0,
                               .tx_counter = 0};

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static void lora_gpio_init(void);
static void lora_uart_init(void);
static void lora_get_configuration(void);
static void lora_init(void);
static void lora_set_mode(Lora_Mode_T mode);
static void lora_set_configuration(void);
static void lora_get_chip_version(void);
static void lora_reset(void);
static void lora_tx_test(void);

/************************************
 * IRQ FUNCTIONS
 ************************************/
void EXTI0_IRQHandler(void)
{
   if (SET == LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0))
   {
      if (SET == LL_GPIO_IsInputPinSet(LORA_GPIO_AUX_PORT, LORA_GPIO_AUX_PIN))
      {
         lora.aux_state = lora_aux_state_idle;
      }
      else
      {
         lora.aux_state = lora_aux_state_busy;
      }

      LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
#if 0
      irq_aux_nr++;
#endif
   }
}

void USART2_IRQHandler(void)
{
   if (LL_USART_IsActiveFlag_RXNE(USART2))
   {
      uint8_t temp = LL_USART_ReceiveData8(USART2);
      CirBuff_Insert_Char((CirBuff_T *) &lora.uart.buff_rx, temp);
#if 0
      CirBuff_Insert_Char((CirBuff_T *) &cb_uart1_tx, temp);
#endif
   }

   if (LL_USART_IsActiveFlag_TXE(USART2))
   {
   }
#if 0
   irq_uart_nr++;
#endif
}

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void lora_gpio_init(void)
{
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

   LL_GPIO_SetPinMode(LORA_GPIO_M0_PORT, LORA_GPIO_M0_PIN, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinSpeed(LORA_GPIO_M0_PORT, LORA_GPIO_M0_PIN, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(LORA_GPIO_M0_PORT, LORA_GPIO_M0_PIN, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(LORA_GPIO_M0_PORT, LORA_GPIO_M0_PIN, LL_GPIO_PULL_DOWN);
   LL_GPIO_ResetOutputPin(LORA_GPIO_M0_PORT, LORA_GPIO_M0_PIN);

   LL_GPIO_SetPinMode(LORA_GPIO_M1_PORT, LORA_GPIO_M1_PIN, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetPinSpeed(LORA_GPIO_M1_PORT, LORA_GPIO_M1_PIN, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinOutputType(LORA_GPIO_M1_PORT, LORA_GPIO_M1_PIN, LL_GPIO_OUTPUT_PUSHPULL);
   LL_GPIO_SetPinPull(LORA_GPIO_M1_PORT, LORA_GPIO_M1_PIN, LL_GPIO_PULL_DOWN);
   LL_GPIO_ResetOutputPin(LORA_GPIO_M1_PORT, LORA_GPIO_M1_PIN);

   lora.mode = lora_mode_normal;

   LL_GPIO_SetPinMode(LORA_GPIO_AUX_PORT, LORA_GPIO_AUX_PIN, LL_GPIO_MODE_INPUT);
   // LL_GPIO_SetPinSpeed(LORA_GPIO_AUX_PORT, LORA_GPIO_AUX_PIN, LL_GPIO_SPEED_FREQ_HIGH);
   LL_GPIO_SetPinPull(LORA_GPIO_AUX_PORT, LORA_GPIO_AUX_PIN, LL_GPIO_PULL_UP);

   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
   LL_GPIO_AF_SetEXTISource(LL_GPIO_AF_EXTI_PORTB, LL_GPIO_AF_EXTI_LINE0);

   LL_EXTI_InitTypeDef exti_initstruct;
   exti_initstruct.Line_0_31 = LL_EXTI_LINE_0;
   exti_initstruct.LineCommand = ENABLE;
   exti_initstruct.Mode = LL_EXTI_MODE_IT;
   exti_initstruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;

   /* Initialize EXTI according to parameters defined in initialization structure. */
   LL_EXTI_Init(&exti_initstruct);

   /*-4- Configure NVIC for EXTI15_10_IRQn */
   NVIC_EnableIRQ(EXTI0_IRQn);
   NVIC_SetPriority(EXTI0_IRQn, 0);
}

static void lora_uart_init(void)
{
   LL_USART_InitTypeDef USART_InitStruct = {0};
   LL_GPIO_InitTypeDef  GPIO_InitStruct = {0};

   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2); /* Peripheral clock enable */
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

   /*
   USART2 GPIO Configuration
   PA2   ------> USART2_TX
   PA3   ------> USART2_RX
   */
   GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
   GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
   GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
   GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
   GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
   LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   /* USART2 interrupt Init */
   NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
   NVIC_EnableIRQ(USART2_IRQn);

   USART_InitStruct.BaudRate = 9600;
   USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
   USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
   USART_InitStruct.Parity = LL_USART_PARITY_NONE;
   USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
   USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
   USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
   LL_USART_Init(USART2, &USART_InitStruct);
   LL_USART_ConfigAsyncMode(USART2);
   LL_USART_Enable(USART2);
   // LL_USART_EnableIT_TXE(USART2);
   LL_USART_EnableIT_RXNE(USART2);
   LL_USART_EnableIT_ERROR(USART2);
}

static void lora_init(void)
{
   lora_gpio_init();
   lora_uart_init();
}

static void lora_set_mode(Lora_Mode_T mode)
{
   switch (mode)
   {
   case lora_mode_normal:
      lora.mode = lora_mode_normal;
      LL_GPIO_ResetOutputPin(LORA_GPIO_M0_PORT, LORA_GPIO_M0_PIN);
      LL_GPIO_ResetOutputPin(LORA_GPIO_M1_PORT, LORA_GPIO_M1_PIN);
      TS_Delay_ms(1);
      break;
   case lora_mode_wake_up:
      lora.mode = lora_mode_wake_up;
      LL_GPIO_SetOutputPin(LORA_GPIO_M0_PORT, LORA_GPIO_M0_PIN);
      LL_GPIO_ResetOutputPin(LORA_GPIO_M1_PORT, LORA_GPIO_M1_PIN);
      TS_Delay_ms(1);
      break;
   case lora_mode_power_saving:
      lora.mode = lora_mode_power_saving;
      LL_GPIO_ResetOutputPin(LORA_GPIO_M0_PORT, LORA_GPIO_M0_PIN);
      LL_GPIO_SetOutputPin(LORA_GPIO_M1_PORT, LORA_GPIO_M1_PIN);
      TS_Delay_ms(1);
      break;
   case lora_mode_sleep:
      lora.mode = lora_mode_sleep;
      LL_GPIO_SetOutputPin(LORA_GPIO_M0_PORT, LORA_GPIO_M0_PIN);
      LL_GPIO_SetOutputPin(LORA_GPIO_M1_PORT, LORA_GPIO_M1_PIN);
      TS_Delay_ms(1);
      break;
   default:
      break;
   }
}

static void lora_set_configuration(void)
{
   // 0xC0 0x02 0x02 0x18 0x17 0x44
   lora_set_mode(lora_mode_sleep);

   uint8_t i = 0;
#ifdef LORA_E32_RX
   uint8_t data[] = {0xC2,
                     (uint8_t) (LORA_RX_DEVICE_ADDRESS >> 8),
                     (uint8_t) (LORA_RX_DEVICE_ADDRESS & 0xFF),
                     0x18,
                     LORA_RX_CHANEL,
                     0x44};
#else
   uint8_t data[] = {0xC2,
                     (uint8_t) (LORA_TX_DEVICE_ADDRESS >> 8),
                     (uint8_t) (LORA_TX_DEVICE_ADDRESS & 0xFF),
                     0x18,
                     LORA_TX_CHANEL,
                     0x44};
#endif

   while (!LL_USART_IsActiveFlag_TXE(USART2))
   {
   }
   for (i = 0; i < sizeof(data); i++)
   {
      LL_USART_TransmitData8(USART2, data[i]);
      while (!LL_USART_IsActiveFlag_TC(USART2))
      {
      }
   }

   TS_Delay_ms(2);
   lora_set_mode(lora_mode_normal);
}

static void lora_reset(void)
{
   lora_set_mode(lora_mode_sleep);

   uint8_t i = 0;
   uint8_t data[] = {0xC4, 0xC4, 0xC4};

   while (!LL_USART_IsActiveFlag_TXE(USART2))
   {
   }
   for (i = 0; i < sizeof(data); i++)
   {
      LL_USART_TransmitData8(USART2, data[i]);
      while (!LL_USART_IsActiveFlag_TC(USART2))
      {
      }
   }
   lora_set_mode(lora_mode_normal);
}

static void lora_get_chip_version(void)
{
   lora_set_mode(lora_mode_sleep);

   uint8_t i = 0;
   uint8_t data[] = {0xC3, 0xC3, 0xC3};

   while (!LL_USART_IsActiveFlag_TXE(USART2))
   {
   }
   for (i = 0; i < sizeof(data); i++)
   {
      LL_USART_TransmitData8(USART2, data[i]);
      while (!LL_USART_IsActiveFlag_TC(USART2))
      {
      }
   }

   lora_set_mode(lora_mode_normal);
}

static void lora_get_configuration(void)
{
   lora_set_mode(lora_mode_sleep);

   uint8_t i = 0;
   uint8_t data[] = {0xC1, 0xC1, 0xC1};

   while (!LL_USART_IsActiveFlag_TXE(USART2))
   {
   }
   for (i = 0; i < sizeof(data); i++)
   {
      LL_USART_TransmitData8(USART2, data[i]);
      while (!LL_USART_IsActiveFlag_TC(USART2))
      {
      }
   }

   lora_set_mode(lora_mode_normal);
}

static void lora_tx_test(void)
{
   uint8_t i = 0;
   uint8_t data[] = {'T', 'E', 'S', 'T'};

   while (!LL_USART_IsActiveFlag_TXE(USART2))
   {
   }
   for (i = 0; i < sizeof(data); i++)
   {
      LL_USART_TransmitData8(USART2, data[i]);
      while (!LL_USART_IsActiveFlag_TC(USART2))
      {
      }
   }
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
Lora_Status Lora_Main_Thread(void)
{
   switch (lora.machine_state)
   {
   case lora_machine_state_uninitialized:
      lora_init();
      lora.machine_state = lora_machine_state_reset;
      break;

   case lora_machine_state_reset:
      if (lora_aux_state_idle == lora.aux_state)
      {
         lora_reset();
         lora.por_time = TS_Get_ms();
         lora.machine_state = lora_machine_state_get_chip_id;
      }
      break;

   case lora_machine_state_get_chip_id:
      // 0xC3 0x32 0x10 0x14
      if ((lora_aux_state_idle == lora.aux_state) && (lora_uart_state_idle == lora.uart.state)
          /*&& (irq_aux_nr == 6)*/)
      {
         if (TS_Get_ms() >= lora.por_time + DELAY_TIME_AFTER_RESET_MS)
         {
            lora_get_chip_version();
            lora.uart.state = lora_uart_state_rx_busy;
         }
         break;
      }

      if (lora_uart_state_rx_busy == lora.uart.state)
      {
         CirBuff_T *buff = (CirBuff_T *) &lora.uart.buff_rx;
         if (buff->head - buff->tail == 4)
         {
            buff->tail += 4;
            lora.uart.state = lora_uart_state_rx_done;
         }
         break;
      }

      if (lora_uart_state_rx_done == lora.uart.state)
      {
         lora.machine_state = lora_machine_state_set_configuration;
         lora.uart.state = lora_uart_state_idle;
      }
      break;

   case lora_machine_state_set_configuration:
      // 0xC0 0x00 0x00 0x18 0x17 0x44
      if (lora_aux_state_idle == lora.aux_state)
      {
         lora.uart.state = lora_uart_state_tx_busy;
         lora_set_configuration();
         // workaround
         lora.uart.state = lora_uart_state_idle;
         lora.machine_state = lora_machine_state_get_configuration;
      }
      break;


   case lora_machine_state_get_configuration:
      // 0xC1 0xC1 0xC1
      // 0xC0 0x00 0x00 0x1A 0x17 0x44
      if ((lora_aux_state_idle == lora.aux_state) && (lora_uart_state_idle == lora.uart.state))
      {
         lora_get_configuration();
         lora.uart.state = lora_uart_state_rx_busy;
         break;
      }

      if (lora_uart_state_rx_busy == lora.uart.state)
      {
         CirBuff_T *buff = (CirBuff_T *) &lora.uart.buff_rx;
         if (buff->head - buff->tail == 6)
         {
            buff->tail += 6;
            lora.uart.state = lora_uart_state_rx_done;
         }
         break;
      }

      if (lora_uart_state_rx_done == lora.uart.state)
      {
         lora.uart.state = lora_uart_state_idle;
#ifdef LORA_E32_RX
         lora.machine_state = lora_machine_state_rx;
#else
         lora.machine_state = lora_machine_state_tx;
#endif
      }
      break;

   case lora_machine_state_rx:
      // lora.rx_counter++;
      break;

   case lora_machine_state_tx:
      if (TS_Get_ms() >= lora.tx_time + DELAY_TX_TIME_MS)
      {
         lora.tx_time = TS_Get_ms();
         lora_tx_test();
         lora.tx_counter++;
      }
      break;

   case lora_machine_state_service:
      break;

   default:
      break;
   }
   return Lora_Success;
}

uint32_t Lora_Get_Machine_State(void)
{
   return lora.machine_state;
}

uint32_t Lora_Get_Tx_Counter(void)
{
   return lora.tx_counter;
}

uint32_t Lora_Get_Rx_Counter(void)
{
   return lora.rx_counter;
}

#endif /* #if defined(LORA_E32_RX) || defined(LORA_E32_TX) */