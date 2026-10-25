/**
  ******************************************************************************
  * @file     my_lcd.h
  * @author   Gaby Fotio
  * @version  V1.0
  * @date     27.10.2025
  * @brief    Custom LCD drawing utilities for bargraph and cross rendering
  ******************************************************************************
*/

#ifndef MY_LCD_MY_LCD_H_
#define MY_LCD_MY_LCD_H_

/* Includes */
#include "stm32f4xx.h"
#include <lcd/lcd.h>

/**
 * Public functions (prototypes)
 */
void my_lcd_draw_bargraph(uint16_t x, uint16_t y,uint16_t width, uint16_t height,uint16_t value,
                          uint16_t color, uint16_t bgcolor);
void lcd_draw_cross(uint16_t x_center, uint16_t y_center, uint16_t size, uint16_t color);

#endif /* MY_LCD_MY_LCD_H_ */
