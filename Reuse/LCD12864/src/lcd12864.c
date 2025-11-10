/**
 ********************************************************************************
 * @file    lcd12864.c
 * @author  Mikolaj Pieklo
 * @date    20.11.2024
 * @brief
 ********************************************************************************
 */

/************************************
 * INCLUDES
 ************************************/
#include "lcd12864.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>

#include <delay.h>
#include <log.h>
#include <spi.h>
#include <string.h>

/************************************
 * EXTERN VARIABLES
 ************************************/

/************************************
 * PRIVATE MACROS AND DEFINES
 ************************************/
#define LCD12864_RST_PIN LL_GPIO_PIN_4
#define LCD12864_RS_PIN  LL_GPIO_PIN_5

#define FONT16_0_INDEX 512
#define FONT16_STEP    32
#define ASCII_0_VALUE  48

/************************************
 * PRIVATE TYPEDEFS
 ************************************/
typedef struct Lcd12864_Status_Typedef
{
   bool is_init;
   bool graphic_mode;
   bool rotate;
} Lcd12864_Status_T;

/************************************
 * STATIC VARIABLES
 ************************************/
/* Dummy device */
static const struct device lcd12864_dev = {
   .name = "LCD12864",
};
static Lcd12864_Status_T lcd12864_status = {.is_init = false,
                                            .graphic_mode = false,
                                            .rotate = true};

/************************************
 * GLOBAL VARIABLES
 ************************************/

/************************************
 * STATIC FUNCTION PROTOTYPES
 ************************************/
static void    lcd12864_init_gpio(void);
static void    lcd12864_send_byte(uint8_t data);
static void    lcd12864_send_command(uint8_t cmd);
static void    lcd12864_send_data(uint8_t data);
static void    lcd12864_send_string(int row, int col, char *string);
static uint8_t rotate(uint8_t ch);

extern const uint8_t Font16_Table[];

/************************************
 * STATIC FUNCTIONS
 ************************************/
static void lcd12864_init_gpio(void)
{
   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

   LL_GPIO_SetPinMode(GPIOB, LCD12864_RST_PIN, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_SetOutputPin(GPIOB, LCD12864_RST_PIN);
   LL_GPIO_SetPinMode(GPIOB, LCD12864_RS_PIN, LL_GPIO_MODE_OUTPUT);
   LL_GPIO_ResetOutputPin(GPIOB, LCD12864_RS_PIN);
}

static void lcd12864_send_byte(uint8_t data)
{
   uint8_t x = data;
   SPI_Transfer(SPI2, 0, &x, NULL, 1);
}

static void lcd12864_send_command(uint8_t cmd)
{
   LL_GPIO_SetOutputPin(GPIOB, LCD12864_RS_PIN); // RS = 0 (komenda)
   lcd12864_send_byte(0xF8);
   lcd12864_send_byte((cmd & 0xF0));      // Wysokie 4 bity
   lcd12864_send_byte((cmd << 4) & 0xF0); // Niskie 4 bity
   TS_Delay_us(30);
   LL_GPIO_ResetOutputPin(GPIOB, LCD12864_RS_PIN); // RS = 0 (komenda)
}

static void lcd12864_send_data(uint8_t data)
{
   LL_GPIO_SetOutputPin(GPIOB, LCD12864_RS_PIN); // RS = 1 (dane)
   lcd12864_send_byte(0xFA);
   lcd12864_send_byte(data & 0xF0);        // Wysokie 4 bity
   lcd12864_send_byte((data << 4) & 0xF0); // Niskie 4 bity
   TS_Delay_us(30);
   LL_GPIO_ResetOutputPin(GPIOB, LCD12864_RS_PIN); // RS = 0 (komenda)
}

static void lcd12864_send_string(int row, int col, char *string)
{
   switch (row)
   {
   case 0:
      col |= 0x80;
      break;
   case 1:
      col |= 0x90;
      break;
   case 2:
      col |= 0x88;
      break;
   case 3:
      col |= 0x98;
      break;
   default:
      col |= 0x80;
      break;
   }

   lcd12864_send_command(col);

   while (*string)
   {
      lcd12864_send_data(*string++);
   }
}

static uint8_t rotate(uint8_t ch)
{
   uint8_t ret = ch;
   if (true == lcd12864_status.rotate)
   {
      uint8_t i = 0;
      ret = 0x00;

      for (i = 0; i < 8; i++)
      {
         if (ch & (0x80 >> i))
         {
            ret |= 0x01 << i;
         }
      }
   }
   return ret;
}

/************************************
 * GLOBAL FUNCTIONS
 ************************************/
void LCD12864_Init(void)
{
   lcd12864_init_gpio();
   TS_Delay_ms(10);

   LL_GPIO_ResetOutputPin(GPIOB, LCD12864_RST_PIN);
   TS_Delay_ms(10);
   LL_GPIO_SetOutputPin(GPIOB, LCD12864_RST_PIN);
   TS_Delay_ms(100);

   lcd12864_send_command(0x30); // 8bit mode
   TS_Delay_ms(1);              //  >100us delay

   lcd12864_send_command(0x30); // 8bit mode
   TS_Delay_ms(1);              // >37us delay

   lcd12864_send_command(0x08); // D=0, C=0, B=0
   TS_Delay_ms(1);              // >100us delay

   LCD12864_Clear_Screen();
   TS_Delay_ms(12); // >10 ms delay


   lcd12864_send_command(0x06); // cursor increment right no shift
   TS_Delay_ms(1);              // 1ms delay

   lcd12864_send_command(0x0C); // D=1, C=0, B=0
   TS_Delay_ms(1);              // 1ms delay

   lcd12864_send_command(0x02); // return to home
   TS_Delay_ms(1);              // 1ms delay

   LCD12864_Clear_Screen();
   TS_Delay_ms(1); // >10 ms delay
   lcd12864_status.is_init = true;

   LCD12864_Graphic_Mode(true);
   LCD12864_Clear_Screen();
}

void LCD12864_Set_Char(void)
{
   lcd12864_send_data('C');
}

void LCD12864_Graphic_Mode(bool enable) // 1-enable, 0-disable
{
   if ((true == enable) && (false == lcd12864_status.graphic_mode))
   {
      lcd12864_send_command(0x30); // 8 bit mode
      TS_Delay_ms(1);
      lcd12864_send_command(0x34); // switch to Extended instructions
      TS_Delay_ms(1);
      lcd12864_send_command(0x36); // enable graphics
      TS_Delay_ms(1);
      lcd12864_status.graphic_mode = true;
   }
   else if (true == lcd12864_status.graphic_mode)
   {
      lcd12864_send_command(0x30); // 8 bit mode
      TS_Delay_ms(1);
   }
   else
   {
      /* Do nothing */
   }
}

void LCD12864_Fill_Screen(void)
{
   uint8_t x, y = 0;

   for (y = 0; y < 64; y++)
   {
      if (y < 32)
      {
         for (x = 0; x < 8; x++)
         {
            // Vertical coordinate of the screen is specified first. (0-31)
            lcd12864_send_command(0x80 | y);
            // Then horizontal coordinate of the screen is specified. (0-8)
            lcd12864_send_command(0x80 | x);
            lcd12864_send_data(0xAA); // Data to the upper byte is sent to the coordinate.
            lcd12864_send_data(0xAA); // Data to the lower byte is sent to the coordinate.
         }
      }
      else
      {
         for (x = 0; x < 8; x++)
         {
            // Vertical coordinate of the screen is specified first. (0-31)
            lcd12864_send_command(0x80 | (y - 32));
            // Then horizontal coordinate of the screen is specified. (0-8)
            lcd12864_send_command(0x88 | x);
            lcd12864_send_data(0xAA); // Data to the upper byte is sent to the coordinate.
            lcd12864_send_data(0xAA); // Data to the lower byte is sent to the coordinate.
         }
      }
   }
}

void LCD12864_Clear_Screen(void)
{
   uint8_t x, y = 0;

   if (true == lcd12864_status.graphic_mode)
   {
      for (y = 0; y < 64; y++)
      {
         if (y < 32)
         {
            lcd12864_send_command(0x80 | y);
            lcd12864_send_command(0x80);
         }
         else
         {
            lcd12864_send_command(0x80 | (y - 32));
            lcd12864_send_command(0x88);
         }
         for (x = 0; x < 8; x++)
         {
            lcd12864_send_data(0);
            lcd12864_send_data(0);
         }
      }
   }
   else
   {
      lcd12864_send_command(0x01); // clear screen
   }
}

int32_t LCD12864_Show_Value(uint8_t x_grid, uint8_t y_grid, int32_t value)
{
   // 512 - 0
   // 544 - 1
   char str[8];

   do
   {
      itoa(value, str, 10);

      uint32_t len = strlen(str);
      // printf("Dlugosc lancucha: %ld\r\n", len);

      if (len > 7)
      {
         log_err(&lcd12864_dev, "String is too long!\r\n");
         break;
      }

      uint32_t x = 7, y = 0;

      uint32_t i = 0;
      for (i = 0; i < len; i++)
      {
         uint32_t proper_ch = FONT16_0_INDEX + FONT16_STEP * (str[i] - ASCII_0_VALUE);
         if (y_grid <= 32)
         {
            for (y = 0; y < 16; y++)
            {
               // Vertical coordinate of the screen is specified first. (0-31)
               lcd12864_send_command(0x80 | (y_grid - y));
               // Then horizontal coordinate of the screen is specified. (0-7)
               lcd12864_send_command(0x80 | (x - i));

               // Data to the upper byte is sent to the coordinate.
               lcd12864_send_data(rotate(Font16_Table[proper_ch + 2 * y + 1]));
               // Data to the lower byte is sent to the coordinate.
               lcd12864_send_data(rotate(Font16_Table[proper_ch + 2 * y]));
            }
         }
         else
         {
            for (y = 0; y < 16; y++)
            {
               // Vertical coordinate of the screen is specified first. (0-31)
               lcd12864_send_command(0x80 | (y_grid - y - 32));
               // Then horizontal coordinate of the screen is specified. (0-7)
               lcd12864_send_command(0x88 | (x - i));

               // Data to the upper byte is sent to the coordinate.
               lcd12864_send_data(rotate(Font16_Table[proper_ch + 2 * y + 1]));
               // Data to the lower byte is sent to the coordinate.
               lcd12864_send_data(rotate(Font16_Table[proper_ch + 2 * y]));
            }
         }
      }
   }
   while (0);

   return 0;
}