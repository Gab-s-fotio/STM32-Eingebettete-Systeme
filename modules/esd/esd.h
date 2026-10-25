/**
  ******************************************************************************
  * @file    	esd.h
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		25.10.2025
  * @brief  	This module is for using the periphery Eight Segment Display (ESD)
  ******************************************************************************
		### Resources used ###
  			Module: ESD
  ******************************************************************************
*/

#ifndef ESD_ESD_H_
#define ESD_ESD_H_

/* Includes */
#include "stm32f4xx.h"


/* enum to save current digit */
typedef enum {
  ESD_DIGIT_0,
  ESD_DIGIT_1,
  ESD_DIGIT_2,
  ESD_DIGIT_3,
  ESD_DIGIT_4,
  ESD_DIGIT_5,
  ESD_DIGIT_6,
  ESD_DIGIT_7,
  ESD_DIGIT_8,
  ESD_DIGIT_9
} esd_digit_t;

/* enum to save current position */
typedef enum {
  ESD_POSITION_1,
  ESD_POSITION_2,
  ESD_POSITION_3,
  ESD_POSITION_4,
  ESD_POSITION_ALL
} esd_position_t;

/**
 * Public functions (prototypes)
 */
void esd_init(void);
void esd_show_digit(esd_digit_t digit, esd_position_t pos);

#endif /* ESD_ESD_H_ */


