/**
  ******************************************************************************
  * @file    	main.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		27.11.2025
  * @brief  	Template using the LCD, control LED brightness and PWM frequency
  * 			 on ESD (dot) using potentiometer
  ******************************************************************************
		### Resources used ###
  			Modules: LCD, My_lcd, Dot, Potis, Utils
  ******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include <lcd/lcd.h>
#include <my_lcd/my_lcd.h>
#include <dot/dot.h>
#include <stdio.h>
#include <utils/utils.h>
#include <potis/potis.h>

/**
 * @brief  Main program entry point
 * @return none
 */
int main(void)
{

	/* Initialize the HAL (configures SysTick and system clock). */
	HAL_Init();

	/* Initialization of the LCD */
	lcd_init();

	/* Draw text on LCD */
	lcd_draw_text_at_line("   Hello World!", 4, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Welcome to", 6, BLACK, 2, WHITE);
	lcd_draw_text_at_line("       PES", 7, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Dimming Dot", 9, BLACK, 2, WHITE);

	/* Initialization of TIM1 & TIM2 */
	timer1_init();
	timer2_init();

	/* Initialization of the output-compare (OC) */
	oc_init();

	/* Initialization of the dot-segment on the ESD */
	dot_init();

	/* Initialize ADCs for potentiometer inputs */
	potis_init();

	/* Text buffer for LCD output */
	char buf[32];

	/* clear screen */
	utils_delay_ms(500);
	lcd_fill_screen(WHITE);

    /* While loop */
	while(1)
	{
		/* --------------------------------------------------------------------
		 *  TASK 2: Display the current counter values of TIM1 and TIM2
		 *  TIM1 is a 16-bit timer on APB2 → range: 0…65535
		 *  TIM2 is a 32-bit timer on APB1 → range: 0…4294967295
		 * -------------------------------------------------------------------- */
		uint32_t u32_timer1 = __HAL_TIM_GET_COUNTER(&timer1_handle_struct);
		uint32_t u32_timer2 = __HAL_TIM_GET_COUNTER(&timer2_handle_struct);
		sprintf(buf, "TIMER1 = %5lu", u32_timer1);
		lcd_draw_text_at_line(buf, 6, BLACK, 2, WHITE);
		sprintf(buf, "TIMER2 = %5lu", u32_timer2);
		lcd_draw_text_at_line(buf, 10, BLACK, 2, WHITE);

		 /* --------------------------------------------------------------------
		  *  TASK 4: Control PWM frequency using potentiometer 1
		  *  - ADC resolution: 0…4095
		  *  - Frequency range: 1…10 Hz
		  * -------------------------------------------------------------------- */
        uint32_t u32_poti_val1 = potis_get_val(1);
        uint32_t u32_freq_hz = 1 + (9 * u32_poti_val1) / 4095; // 1–10 Hz
        set_frequency_hz(u32_freq_hz);

        /* --------------------------------------------------------------------
         *  TASK 5: Control LED brightness using potentiometer 2
         *  - ADC value directly controls PWM duty cycle
         * -------------------------------------------------------------------- */
        uint32_t u32_poti_val2 = potis_get_val(2);
        set_brightness(u32_poti_val2);


	}
}

