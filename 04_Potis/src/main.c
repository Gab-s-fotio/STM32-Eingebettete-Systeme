/**
  ******************************************************************************
  * @file    	main.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		09.11.2025
  * @brief  	Display of analog potentiometer values using LCD bargraphs
  ******************************************************************************
		### Resources used ###
  			Modules: LCD, My_lcd, Potis, Utils
  ******************************************************************************
*/

/* Includes */
#include <lcd/lcd.h>
#include "stm32f4xx.h"
#include <potis/potis.h>
#include <my_lcd/my_lcd.h>
#include <utils/utils.h>
#include <stdio.h>

/**
 * @brief  Main program entry point
 * @return none
 */
int main(void)
{
	/* Variables for converted voltages & bargraph scaling. */
 	uint32_t u32_ADC_val1_bargraph;
	uint32_t u32_ADC_val2_bargraph;
	uint32_t u32_ADC_val1_MV;
	uint32_t u32_ADC_val2_MV;

	/* String buffer Bytes for Poti1 & Poti2 text */
	char buf1[32];
	char buf2[32];

	/* Initialize the HAL (configures SysTick and system clock) */
	HAL_Init();

	/* Initialize the LCD */
	lcd_init();

	/* Draw text on LCD */
	lcd_draw_text_at_line("   Hello World!", 4, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Welcome to", 6, BLACK, 2, WHITE);
	lcd_draw_text_at_line("       PES", 7, BLACK, 2, WHITE);
	lcd_draw_text_at_line("  Potentiometer", 9, BLACK, 2, WHITE);

    /* Wait 3 seconds, clear screen */
	utils_delay_ms(3000);
	lcd_fill_screen(WHITE);

	/* Initialize ADCs for potentiometer inputs */
	potis_init();

	while(1) {

	/* Convert ADC values to millivolts */
	u32_ADC_val1_MV = (3300 * potis_get_val(1)) / 4095;
	u32_ADC_val2_MV = (3300 * potis_get_val(2)) / 4095;

	/* Scale voltage values for bargraph display (0–1000 range) */
	u32_ADC_val1_bargraph = (u32_ADC_val1_MV * 1000) / 3300;
	u32_ADC_val2_bargraph = (u32_ADC_val2_MV * 1000) / 3300;

	/* Draw bargraphs for both potentiometers */
	my_lcd_draw_bargraph(10, 100, 200, 20, u32_ADC_val1_bargraph, BLUE, WHITE);
	my_lcd_draw_bargraph(10, 200, 200, 20, u32_ADC_val2_bargraph, GREEN, WHITE);

	/* Draw text to LCD for Poti1 & Poti2 */
	sprintf(buf1, "Poti 1: %4ld mV", u32_ADC_val1_MV);
	lcd_draw_text_at_line(buf1, 1, BLUE, 2, WHITE);

	sprintf(buf2, "Poti 2: %4ld mV", u32_ADC_val2_MV);
	lcd_draw_text_at_line(buf2, 2, GREEN, 2, WHITE);

	}
}
