/**
  ******************************************************************************
  * @file    	main.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		26.10.2025
  * @brief  	Joystick-driven control of the Eight Segment Display (ESD)
  ******************************************************************************
		### Resources used ###
  			Modules: LCD, ESD, Utils, Joystick
  ******************************************************************************
*/

/* Includes */
#include <lcd/lcd.h>
#include "stm32f4xx.h"
#include <esd/esd.h>
#include <utils/utils.h>
#include <joystick/joystick.h>

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
	lcd_draw_text_at_line("     Joystick", 9, BLACK, 2, WHITE);

	/* Initialize the ESD */
	esd_init();

	/* Initialize the joystick */
	joystick_init();

    /* start state: show digit 0 on position 1 */
	esd_digit_t digit = ESD_DIGIT_0;
	esd_position_t pos = ESD_POSITION_1;
    esd_show_digit(digit, pos);

    /* while loop: react to joystick actions */
	while(1) {

        /* Read current joystick action */
	    joystick_action_t action = joystick_get_action();

        if (action == UP) {
        	/* Increment digit */
            digit = (digit == ESD_DIGIT_9) ? ESD_DIGIT_0 : digit + 1;
            esd_show_digit(digit, pos);
        }
        else if (action == DOWN) {
        	/* Decrement digit */
            digit = (digit == ESD_DIGIT_0) ? ESD_DIGIT_9 : digit - 1;
            esd_show_digit(digit, pos);
        }
        else if (action == RIGHT) {
       	 /* Move position forward */
            pos = (pos == ESD_POSITION_4) ? ESD_POSITION_1 : pos + 1;
            esd_show_digit(digit, pos);
        }
        else if (action == LEFT) {
        	 /* Move position backward */
            pos = (pos == ESD_POSITION_1) ? ESD_POSITION_4 : pos - 1;
            esd_show_digit(digit, pos);
        }
        else if (action == PRESS) {
        	/* Run a blocking countdown from the current digit down to 0
        	    on the current position (1 s per step). Afterwards, restore
        	    the original digit. */
            for (int i = digit; i >= 0; i--) {
                esd_show_digit(i, pos);
                utils_delay_ms(1000);
            }
            /* Show old digit again */
            esd_show_digit(digit, pos);
        }
        /* Small polling delay:
           - prevents movement from joystick to be registered mulitple times
           - provides a minimal debounce effect for the joystick. */
        utils_delay_ms(100);

	}
}
