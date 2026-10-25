/**
  ******************************************************************************
  * @file    	main.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		19.12.2025
  * @brief  	Main application for fan speed control with LCD output
  ******************************************************************************
*/

/* Includes */
#include <lcd/lcd.h>
#include "stm32f4xx.h"
#include <fan/fan.h>
#include <stdio.h>
#include <potis/potis.h>
#include <my_lcd/my_lcd.h>

/* Defines */
#define MAX_RPM 4500
#define MIN_RPM 1000 //20 %

/**
 * @brief  Main program entry point
 * @return none
 */
int main(void)
{
	/* --- Bargraph scaling (0..1000) for RPM values --- */
	uint32_t u32_target_bar;
	uint32_t u32_actual_bar;

	/* Initialize the HAL (configures SysTick and system clock). */
	HAL_Init();

	/* Initialization of the LCD */
	lcd_init();

	/* Initialize fan control module:
	 * - GPIO pins (PWM output and tachometer input)
	 * - Timer 3 for PWM generation
	 * - Timer 4 for tachometer time measurement
	 * - Output Compare channel for PWM
	 * - NVIC interrupt configuration
	 */
	fan_Init();
	fan_timer3_init();
	fan_oc_init();
	fan_timer4_init();
	fan_nvic_init();

	/* Initialize potentiometer module and start ADC with DMA */
	potis_init();
	dma_init();

	/* Draw text to LCD */
	lcd_draw_text_at_line("   Hello World!", 4, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Welcome to", 6, BLACK, 2, WHITE);
	lcd_draw_text_at_line("       PES", 7, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Fan Control", 9, BLACK, 2, WHITE);

	/* clear screen */
	lcd_fill_screen(WHITE);
    //fan_setDuty(100);

	/* Character buffer for formatted LCD output strings */
	char buf[32];

    /* While loop */
	while(1) {
		/* Calculate target RPM from potentiometer position.
		 * Allowed RPM range defined by MIN_RPM and MAX_RPM */
		u32_rpm_target = MIN_RPM + (potis_get_avg(u16_ADC1_val) * (MAX_RPM - MIN_RPM)) / 4095;

		if (u32_rpm_target < MIN_RPM) u32_rpm_target = MIN_RPM;
		if (u32_rpm_target > MAX_RPM) u32_rpm_target = MAX_RPM;

		if (u32_rpm_actual < MIN_RPM) u32_rpm_actual = MIN_RPM;
		if (u32_rpm_actual > MAX_RPM) u32_rpm_actual = MAX_RPM;

		/* Map MIN_RPM..MAX_RPM -> 0..1000 */
		u32_target_bar = ((u32_rpm_target - MIN_RPM) * 1000) / (MAX_RPM - MIN_RPM);
		u32_actual_bar = ((u32_rpm_actual - MIN_RPM) * 1000) / (MAX_RPM - MIN_RPM);

		/* Draw bargraphs */
		my_lcd_draw_bargraph(10, 100, 200, 20, u32_target_bar, BLUE,  WHITE);  // Target RPM
		my_lcd_draw_bargraph(10, 200, 200, 20, u32_actual_bar, GREEN, WHITE);  // Actual RPM

		/* Display target RPM on the LCD */
		sprintf(buf, "Target RPM = %5lu", u32_rpm_target);
		lcd_draw_text_at_line(buf, 1, BLUE, 2, WHITE);

		/* Display actual measured RPM on the LCD */
		sprintf(buf, "Actual RPM = %5lu", u32_rpm_actual);
		lcd_draw_text_at_line(buf, 2, GREEN, 2, WHITE);
	}
}
