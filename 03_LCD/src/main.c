/**
  ******************************************************************************
  * @file    	main.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		27.10.2025
  * @brief  	Demonstration of LCD drawing functions and utilities
  ******************************************************************************
		### Resources used ###
  			Modules: LCD, My_lcd, Utils
  ******************************************************************************
*/

/* Includes */
#include <lcd/lcd.h>
#include <my_lcd/my_lcd.h>
#include "utils/utils.h"
#include "stm32f4xx.h"
#include <stdio.h>

/**
 * @brief  Main program entry point
 * @return none
 */
int main(void)
{
	/* Initialize the HAL (configures SysTick and system clock) */
	HAL_Init();

	/* Initialize the LCD */
	lcd_init();

	/* Draw text on LCD */
	lcd_draw_text_at_line("   Hello World!", 4, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Welcome to", 6, BLACK, 2, WHITE);
	lcd_draw_text_at_line("       PES", 7, BLACK, 2, WHITE);
	lcd_draw_text_at_line("       LCD", 9, BLACK, 2, WHITE);

    /* Wait 3 seconds, clear screen and draw a centered blue cross */
	utils_delay_ms(3000);
    lcd_fill_screen(WHITE);
	lcd_draw_cross(120, 160, 100, BLUE);

	/* Wait 3 seconds and clear screen */
	utils_delay_ms(3000);
	lcd_fill_screen(WHITE);

	/* String buffer Bytes for countdown text */
	char buf[13];

    /* While loop: countdown with bargraph display */
	while(1) {

		/* countdown */
		for (int i = 10; i >= 1; i--)
		{
			/* Format countdown text into buffer */
		    sprintf(buf, "Countdown:%02d", i); //13 signs + 1 (\0 null terminator)

		    /* Display countdown text */
		    lcd_draw_text_at_line(buf, 5, RED, 3, WHITE);

		    /* Display bargraph proportional to countdown value */
		    my_lcd_draw_bargraph(20, 200, 200, 20, i*100, GREEN, RED);

		    /* Wait for 1 second */
		    utils_delay_ms(1000);
		}
	}
}
