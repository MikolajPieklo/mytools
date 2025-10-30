/**
********************************************************************************
* @file    tm1637.c
* @author  Mikolaj Pieklo
* @date    11.10.2025
* @brief
********************************************************************************
*/

/************************************
 * INCLUDES
 ************************************/
#include "tm1637.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>

#include "delay.h"
#include "device_info.h"
/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
// Configuration.

#define CLK_PORT GPIOA
#define DIO_PORT GPIOA
#define CLK_PIN  LL_GPIO_PIN_5
#define DIO_PIN  LL_GPIO_PIN_7

/************************************
 * PRIVATE TYPEDEFS
 ************************************/

/************************************
 * STATIC VARIABLES
 ************************************/

/************************************
 * GLOBAL VARIABLES
 ************************************/
const char segmentMap[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
                           0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, // 8-9, A-F
                           0x00};
/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static void        _tm1637Start(void);
static void        _tm1637Stop(void);
static void        _tm1637ReadResult(void);
static void        _tm1637WriteByte(uint8_t b);
static inline void _tm1637DelayUsec(uint32_t i);
static inline void _tm1637ClkHigh(void);
static inline void _tm1637ClkLow(void);
static inline void _tm1637DioHigh(void);
static inline void _tm1637DioLow(void);

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void _tm1637Start(void)
{
   _tm1637ClkHigh();
   _tm1637DioHigh();
   _tm1637DelayUsec(2);
   _tm1637DioLow();
}

static void _tm1637Stop(void)
{
   _tm1637ClkLow();
   _tm1637DelayUsec(2);
   _tm1637DioLow();
   _tm1637DelayUsec(2);
   _tm1637ClkHigh();
   _tm1637DelayUsec(2);
   _tm1637DioHigh();
}

static void _tm1637ReadResult(void)
{
   _tm1637ClkLow();
   _tm1637DelayUsec(5);
   // while (dio); // We're cheating here and not actually reading back the response.
   _tm1637ClkHigh();
   _tm1637DelayUsec(2);
   _tm1637ClkLow();
}

static void _tm1637WriteByte(uint8_t b)
{
   for (int i = 0; i < 8; ++i)
   {
      _tm1637ClkLow();
      if (b & 0x01)
      {
         _tm1637DioHigh();
      }
      else
      {
         _tm1637DioLow();
      }
      _tm1637DelayUsec(3);
      b >>= 1;
      _tm1637ClkHigh();
      _tm1637DelayUsec(3);
   }
}

static inline void _tm1637DelayUsec(uint32_t i)
{
   TS_Delay_us(i);
}

static inline void _tm1637ClkHigh(void)
{
   LL_GPIO_SetOutputPin(CLK_PORT, CLK_PIN);
}

static inline void _tm1637ClkLow(void)
{
   LL_GPIO_ResetOutputPin(CLK_PORT, CLK_PIN);
}

static inline void _tm1637DioHigh(void)
{
   LL_GPIO_SetOutputPin(DIO_PORT, DIO_PIN);
}

static inline void _tm1637DioLow(void)
{
   LL_GPIO_ResetOutputPin(DIO_PORT, DIO_PIN);
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void TM1637Init(void)
{
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
   LL_GPIO_InitTypeDef g = {0};
   g.Pull = LL_GPIO_PULL_UP;
   g.OutputType = LL_GPIO_OUTPUT_PUSHPULL; // LL_GPIO_OUTPUT_OPENDRAIN;
   g.Mode = LL_GPIO_MODE_OUTPUT;
   g.Speed = LL_GPIO_SPEED_FREQ_HIGH;
   g.Pin = CLK_PIN;
   LL_GPIO_Init(CLK_PORT, &g);
   g.Pin = DIO_PIN;
   LL_GPIO_Init(DIO_PORT, &g);

   TM1637SetBrightness(8);
}

void TM1637DisplayDecimal(int v, int displaySeparator)
{
   unsigned char digitArr[4];
   for (int i = 0; i < 4; ++i)
   {
      digitArr[i] = segmentMap[v % 10];
      if (i == 2 && displaySeparator)
      {
         digitArr[i] |= 1 << 7;
      }
      v /= 10;
   }

   _tm1637Start();
   _tm1637WriteByte(0x40);
   _tm1637ReadResult();
   _tm1637Stop();

   _tm1637Start();
   _tm1637WriteByte(0xc0);
   _tm1637ReadResult();

   for (int i = 0; i < 4; ++i)
   {
      _tm1637WriteByte(digitArr[3 - i]);
      _tm1637ReadResult();
   }

   _tm1637Stop();
}

// Valid brightness values: 0 - 8.
// 0 = display off.
void TM1637SetBrightness(char brightness)
{
   // Brightness command:
   // 1000 0XXX = display off
   // 1000 1BBB = display on, brightness 0-7
   // X = don't care
   // B = brightness
   _tm1637Start();
   _tm1637WriteByte(0x87 + brightness);
   _tm1637ReadResult();
   _tm1637Stop();
}

void TM1637ShowError(void)
{
   uint8_t i;

   _tm1637Start();
   _tm1637WriteByte(0x40);
   _tm1637ReadResult();
   _tm1637Stop();

   _tm1637Start();
   _tm1637WriteByte(0xc0);
   _tm1637ReadResult();

   for (i = 0; i < 4; i++)
   {
      _tm1637WriteByte(0x79);
      _tm1637ReadResult();
   }

   _tm1637Stop();
}

void TM1637ShowStartMessage(void)
{
   Device_Restart_Issue_T restart = Device_Info_Get_Restart_Issue();
   _tm1637Start();
   _tm1637WriteByte(0x40);
   _tm1637ReadResult();
   _tm1637Stop();

   _tm1637Start();
   _tm1637WriteByte(0xc0);
   _tm1637ReadResult();

   // Error issue code
   _tm1637WriteByte(segmentMap[restart]);
   _tm1637ReadResult();
   // H
   _tm1637WriteByte(0x76);
   _tm1637ReadResult();
   // E
   _tm1637WriteByte(0x79);
   _tm1637ReadResult();
   // L
   _tm1637WriteByte(0x38);
   _tm1637ReadResult();

   _tm1637Stop();
}

//     0x01
// 0x20      0x02
//     0x40
// 0x10     0x04
//     0x08
//              0x80