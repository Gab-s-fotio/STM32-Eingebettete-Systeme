/**
  ******************************************************************************
  * @file    	esd.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		25.10.2025
  * @brief  	This module is for using the periphery Eight Segment Display (ESD)
  @verbatim
  ==============================================================================
		### Resources used ###
  			Module: ESD
  ==============================================================================
  		###     Usage      ###
  	(#) Call 'esd_init()' to initialize the module and all the necessary peripheries
  	(#) Call 'esd_show_digit(digit, pos)' displays a specific digit on a selected position
  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include <esd/esd.h>

/* defines */
/* Segment pin definitions (a, b, c, d, e, f, g, dot, point)
 *  These are configured as LOW-active outputs. */
#define ESD_PIN_A      GPIO_PIN_7
#define ESD_PIN_B      GPIO_PIN_4
#define ESD_PIN_C      GPIO_PIN_5
#define ESD_PIN_D      GPIO_PIN_6
#define ESD_PIN_E      GPIO_PIN_12
#define ESD_PIN_F      GPIO_PIN_11
#define ESD_PIN_G      GPIO_PIN_12
#define ESD_PIN_DOT    GPIO_PIN_11
#define ESD_PIN_POINT  GPIO_PIN_7

/* defines */
/* Digit position control pins (CNTL1-4)
 *  These are configured as HIGH-active outputs. */
#define ESD_PIN_CNTL1  GPIO_PIN_14
#define ESD_PIN_CNTL2  GPIO_PIN_15
#define ESD_PIN_CNTL3  GPIO_PIN_0
#define ESD_PIN_CNTL4  GPIO_PIN_1

/**
 * @brief initialize esd display
 * @param none
 * @return none
 */
void esd_init(void){

		/* Enable GPIO clocks */
		__HAL_RCC_GPIOD_CLK_ENABLE();   // Port D for segments and control lines (digit)
	    __HAL_RCC_GPIOE_CLK_ENABLE();   // Port E for additional segment pins (positions)

	    /* Declaration of init */
	    GPIO_InitTypeDef init = {0};
	    init.Mode  = GPIO_MODE_OUTPUT_PP; /* push-pull output mode to set pin low or high (Port as digital output) */
	    init.Pull  = GPIO_NOPULL; /* resistance deactivated */
	    init.Speed = GPIO_SPEED_MEDIUM; /* speed of the pin driver */

	    /* Configure Port D: segments + control pins */
	    init.Pin = ESD_PIN_A | ESD_PIN_B | ESD_PIN_C | ESD_PIN_D | ESD_PIN_E | ESD_PIN_F | ESD_PIN_POINT | ESD_PIN_CNTL1 | ESD_PIN_CNTL2 | ESD_PIN_CNTL3 | ESD_PIN_CNTL4;

	    /* Initialize Port D */
	    HAL_GPIO_Init(GPIOD, &init);

	    /* Configure Port E: remaining segment pins */
	    init.Pin = ESD_PIN_G | ESD_PIN_DOT;

	    /* Initialize Port E */
	    HAL_GPIO_Init(GPIOE, &init);

}

/**
 * @brief displays a single digit on specified position on ESD
 * @param digit: number to display (0–9)
 * @param pos: position on the display (1–4 or ALL)
 * @return none
 */
void esd_show_digit(esd_digit_t digit, esd_position_t pos)
{
	    /* bitmask of active (ON) segment pins on Port D & Port E
	     * saves which segments will be activated later */
		uint16_t seg_on_d = 0;
	    uint16_t seg_on_e = 0;

	    /* Define which segments are ON for each digit (low-active) */
	    switch(digit)
	    {
	    	case ESD_DIGIT_0: seg_on_d = ESD_PIN_A|ESD_PIN_B|ESD_PIN_C|ESD_PIN_D|ESD_PIN_E|ESD_PIN_F;
	                          seg_on_e = 0;
	                          break;
	    	case ESD_DIGIT_1: seg_on_d = ESD_PIN_B|ESD_PIN_C;
	    		              seg_on_e = 0;
	    		              break;
	    	case ESD_DIGIT_2: seg_on_d = ESD_PIN_A|ESD_PIN_B|ESD_PIN_D|ESD_PIN_E;
	    		              seg_on_e = ESD_PIN_G;
	    		              break;
	    	case ESD_DIGIT_3: seg_on_d = ESD_PIN_A|ESD_PIN_B|ESD_PIN_C|ESD_PIN_D;
	    		    		  seg_on_e = ESD_PIN_G;
	    		    		  break;
	    	case ESD_DIGIT_4: seg_on_d = ESD_PIN_B|ESD_PIN_C|ESD_PIN_F;
	    		              seg_on_e = ESD_PIN_G;
	    		              break;
	    	case ESD_DIGIT_5: seg_on_d = ESD_PIN_A|ESD_PIN_C|ESD_PIN_D|ESD_PIN_F;
	    		    		  seg_on_e = ESD_PIN_G;
	    		    		  break;
	    	case ESD_DIGIT_6: seg_on_d = ESD_PIN_A|ESD_PIN_C|ESD_PIN_D|ESD_PIN_E|ESD_PIN_F;
	    		    		  seg_on_e = ESD_PIN_G;
	    		    		  break;
	    	case ESD_DIGIT_7: seg_on_d = ESD_PIN_A|ESD_PIN_B|ESD_PIN_C;
	    		    		  seg_on_e = 0;
	    		    		  break;
	    	case ESD_DIGIT_8: seg_on_d = ESD_PIN_A|ESD_PIN_B|ESD_PIN_C|ESD_PIN_D|ESD_PIN_E|ESD_PIN_F;
	    		    		  seg_on_e = ESD_PIN_G;
	    		    		  break;
	    	case ESD_DIGIT_9: seg_on_d = ESD_PIN_A|ESD_PIN_B|ESD_PIN_C|ESD_PIN_D|ESD_PIN_F;
	    		    		  seg_on_e = ESD_PIN_G;
	    		    		  break;
	    	default:
	    		break;
	    }

	    /* Turn OFF all segments to avoid overlaps (set to HIGH = off because low-active) */
	    HAL_GPIO_WritePin(GPIOD, ESD_PIN_A | ESD_PIN_B | ESD_PIN_C | ESD_PIN_D | ESD_PIN_E | ESD_PIN_F | ESD_PIN_POINT, GPIO_PIN_SET);
	    HAL_GPIO_WritePin(GPIOE, ESD_PIN_G | ESD_PIN_DOT, GPIO_PIN_SET);

	    /* Activate the required segments (set to LOW = on because low-active) */
	    HAL_GPIO_WritePin(GPIOD, seg_on_d, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(GPIOE, seg_on_e, GPIO_PIN_RESET);

	    /* Enable the selected position (CNTL1–4) */
	    switch (pos)
	       {
	           case ESD_POSITION_1: HAL_GPIO_WritePin(GPIOD, ESD_PIN_CNTL1, GPIO_PIN_SET);
	           	   	   	   	   	   	HAL_GPIO_WritePin(GPIOD, ESD_PIN_CNTL2 | ESD_PIN_CNTL3 | ESD_PIN_CNTL4, GPIO_PIN_RESET);
	           	   	   	   	   	   	break;
	           case ESD_POSITION_2: HAL_GPIO_WritePin(GPIOD, ESD_PIN_CNTL2, GPIO_PIN_SET);
	           	   	   	   	   	    HAL_GPIO_WritePin(GPIOD, ESD_PIN_CNTL1 | ESD_PIN_CNTL3 | ESD_PIN_CNTL4, GPIO_PIN_RESET);
	           	   	   	   	   	    break;
	           case ESD_POSITION_3: HAL_GPIO_WritePin(GPIOD, ESD_PIN_CNTL3, GPIO_PIN_SET);
	           	   	   	   	   	    HAL_GPIO_WritePin(GPIOD, ESD_PIN_CNTL1 | ESD_PIN_CNTL2 | ESD_PIN_CNTL4, GPIO_PIN_RESET);
	           	   	   	   	   	    break;
	           case ESD_POSITION_4: HAL_GPIO_WritePin(GPIOD, ESD_PIN_CNTL4, GPIO_PIN_SET);
	           	   	   	   	   	    HAL_GPIO_WritePin(GPIOD, ESD_PIN_CNTL1 | ESD_PIN_CNTL2 | ESD_PIN_CNTL3, GPIO_PIN_RESET);
	           	   	   	   	   	    break;
	           case ESD_POSITION_ALL:
	               	   	   	   	    HAL_GPIO_WritePin(GPIOD, ESD_PIN_CNTL1 | ESD_PIN_CNTL2 | ESD_PIN_CNTL3 | ESD_PIN_CNTL4, GPIO_PIN_SET);
	               	   	   	   	    break;
	           default: break;
	       }
}
