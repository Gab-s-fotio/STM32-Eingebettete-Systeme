/**
  ******************************************************************************
  * @file    	main.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		24.10.2025
  * @brief  	Template using the LCD
  ******************************************************************************
		### Resources used ###
  			GPIO: PG13, PG14
  ******************************************************************************
*/

/* Includes */
#include <lcd/lcd.h>
#include "stm32f4xx.h"

/**
 * @brief  Main program entry point
 * @return none
 */
int main(void)
{
	/* Initialize the HAL (configures SysTick and system clock) */
	HAL_Init();

	/* Enable the clock for GPIO port G */
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/* Declaration of gpio_init */
	GPIO_InitTypeDef gpio_init;
	gpio_init.Pin = GPIO_PIN_13 | GPIO_PIN_14; /* two LED pins on Port G */
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP; /* push-pull output mode to set pin low or high (Port as digital output) */
	gpio_init.Pull = GPIO_NOPULL; /* resistance deactivated */
	gpio_init.Speed = GPIO_SPEED_MEDIUM; /* speed of the pin driver */

	/* Initialization of GPIO Port G */
	HAL_GPIO_Init(GPIOG, &gpio_init);


	/* Initialize the LCD */
	lcd_init();

	/* Draw text on LCD */
	lcd_draw_text_at_line("   Hello World!", 4, BLACK, 2, WHITE);
	lcd_draw_text_at_line("    Welcome to", 6, BLACK, 2, WHITE);
	lcd_draw_text_at_line("       PES", 7, BLACK, 2, WHITE);
	lcd_draw_text_at_line("   Introduction", 9, BLACK, 2, WHITE);

	while(1) {
		/* activate the green LED */
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(3000);

		/* activate both LED's */
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_SET);
		HAL_Delay(3000);

		/* deactivate the red LED */
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_Delay(3000);

		/* deactivate both LED's */
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13| GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_Delay(3000);

		/* activate the green LED */
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(1000);

		/* toggle both LED's
		 * Inverts current state of pin*/
		for (int i = 0; i < 10; i++) {
			HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14);
			HAL_Delay(500);
		}
	}
}
