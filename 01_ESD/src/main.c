/**
  ******************************************************************************
  * @file    	main.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		25.10.2025
  * @brief  	Template using the Eight Segment Display (ESD)
  ******************************************************************************
		### Resources used ###
  			Modules: LCD, ESD, Utils
  ******************************************************************************
*/

/* Includes */
#include <lcd/lcd.h>
#include "esd/esd.h"
#include "stm32f4xx.h"
#include "utils/utils.h"

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
	lcd_draw_text_at_line("  Eight  Segment", 9, BLACK, 2, WHITE);
	lcd_draw_text_at_line("     Display", 10, BLACK, 2, WHITE);

	/* Initialize the ESD*/
	esd_init();

	while (1)
	{
		 	/* loop for countdown and position of digit */
		for (int pos = ESD_POSITION_1; pos <= ESD_POSITION_4; pos++)
		{
			/* loop for digit */
		    for (int digit = ESD_DIGIT_9; digit >= ESD_DIGIT_0; digit--)
		    {
		        /* shows digit on current position */
		    	esd_show_digit(digit, pos);
		        utils_delay_ms(1000);
		    }
		}

		/* loop for showing countdown on all positions simultaneously */
		for (int digit = ESD_DIGIT_9; digit >= ESD_DIGIT_0; digit--)
		{
			/* shows digit on all positions */
		    esd_show_digit(digit, ESD_POSITION_ALL);
		    utils_delay_ms(1000);
		}
	}
}
