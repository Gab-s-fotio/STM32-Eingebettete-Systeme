/**
  ******************************************************************************
  * @file    	main.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		27.11.2025
  * @brief  	Stopwatch application using LCD and TIM1-based tick counter
  ******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include <lcd/lcd.h>
#include <my_lcd/my_lcd.h>
#include <dot/dot.h>
#include <stdio.h>
#include <utils/utils.h>
#include <stopwatch/stopwatch.h>

/**
 * @brief  Main program entry point
 * @return none
 */
int main(void)
{
	uint32_t u32_ticks;
	uint32_t u32_minutes;
	uint32_t u32_seconds;
	uint32_t u32_milliseconds;

	/* Initialize the HAL (configures SysTick and system clock). */
	HAL_Init();

	/* Initialization of the LCD */
	lcd_init();

	/* Initialization of TIM1 for stopwatch */
	timer1_init_stopwatch();

	/* Initialization of NVIC for priorities for TIM1 and EXTI0 */
	nvic_init();

	/* Initialization of user button (PA0) as external interrupt source */
	button_init();

	/* Draw text to LCD */
	lcd_draw_text_at_line("   Hello World!", 4, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Welcome to", 6, BLACK, 2, WHITE);
	lcd_draw_text_at_line("       PES", 7, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Stopwatch", 9, BLACK, 2, WHITE);

	/* clear screen */
	lcd_fill_screen(WHITE);

	/* Buffer for current total time & for lap time strings */
	char buf[32];
	char buf1[32];

    /* While loop */
	while(1) {
		if (u8_sw_running)
		{
			u32_ticks = u32_tick_counter;
			u32_milliseconds = u32_ticks / 10;
			u32_seconds = u32_milliseconds / 1000;
			u32_minutes = u32_seconds / 60;
			u32_milliseconds  %= 100;
			u32_seconds %= 60;
			sprintf(buf, "Time: %02lu:%02lu.%02lu", u32_minutes, u32_seconds, u32_milliseconds);
		    lcd_draw_text_at_line(buf, 0, BLACK, 2, WHITE);

		        if (u8_lap_refresh) // Set by EXTI-ISR when a new lap is recorded
		        {
		            u8_lap_refresh = 0;
	                /* Loop over stored laps, maximum 10 entries */
		            for (uint8_t i = 0; i < u8_lap_list_size && i < 10; i++)
		               {
		            	u32_ticks = u32_lap_durations[i];
		            	u32_milliseconds = u32_ticks / 10;
		            	u32_seconds = u32_milliseconds / 1000;
		            	u32_minutes = u32_seconds / 60;
		            	u32_milliseconds  %= 100;
		            	u32_seconds %= 60;
		            	sprintf(buf1, "Lap %02lu: %02lu:%02lu.%02lu", u32_lap_counter - i, u32_minutes, u32_seconds, u32_milliseconds);
		                lcd_draw_text_at_line(buf1, 2 + i, BLACK, 2, WHITE);
		            }

		        }

		 }
	}

}
