/**
  ******************************************************************************
  * @file     my_lcd.c
  * @author   Gaby Fotio
  * @version  V1.0
  * @date     27.10.2025
  * @brief    Implementation of custom LCD drawing utilities
  @verbatim
  ==============================================================================
		### Resources used ###
  			LCD-Display
  ==============================================================================
  ###     Usage      ###
  	(#) Call 'my_lcd_draw_bargraph(x, y, width, height, value, color, bgcolor)'
  		for bargraph visualization on the LCD-Display
  	(#) Call 'lcd_draw_cross(x_center, y_center, size, color)' for drawing a pixel-
  		based centered cross
  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* Includes */
#include "my_lcd/my_lcd.h"
#include "lcd/lcd.h"      // provides ILI9341_Draw_Filled_Rectangle_Coord()
#include <stdio.h>

/**
 * @brief  Draws a horizontal bargraph on the LCD
 * @param  x: X-coordinate of the lower-left corner of the bargraph
 * @param  y: Y-coordinate of the lower-left corner of the bargraph
 * @param  width: Total width of the bargraph (in pixels)
 * @param  height: Height of the bargraph (in pixels)
 * @param  value: Bargraph fill value
 * @param  color: Color of the filled section (graph).
 * @param  bgcolor: Color of the unfilled (background) section.
 * @return none
 */
void my_lcd_draw_bargraph(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                          uint16_t value, uint16_t color, uint16_t bgcolor)
{
    /* Safety limit range 1 - 1000 */
    if (value > 1000)
        value = 1000;
    else if(value < 1)
    	value = 1;

	/*calculate filled part of graph*/
    uint16_t filled_width = (width * value) / 1000;

    /* Draw  filled part of the bargraph */
    ILI9341_Draw_Filled_Rectangle_Coord(x, y, x + filled_width, y + height, color);

    /* Draw the remaining background part */
    ILI9341_Draw_Filled_Rectangle_Coord(x + filled_width, y, x + width, y + height, bgcolor);
}


/**
 * @brief	Draws a centered cross on the LCD-Display
 * @param	x_center: X-coordinate of the cross center
 * @param 	y_center: Y-coordinate of the cross center
 * @param	size: size of the cross
 * @param	color: color of the cross
 * @return	none
 */
void lcd_draw_cross(uint16_t x_center, uint16_t y_center, uint16_t size, uint16_t color)
{
	/* Draw horizontal line, left to right */
	for (int x = x_center - size; x <= x_center + size; x++)
	{
		lcd_draw_pixel(x, y_center, color);
	}

	/* Draw vertical line, up to down */
	for (int y = y_center - size; y <= y_center + size; y++)
	{
	    lcd_draw_pixel(x_center, y, color);
	}
}


/*
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
	ILI9341_Draw_Pixel(x, y, color);
}
*/
