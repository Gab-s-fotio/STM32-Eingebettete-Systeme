/**
  ******************************************************************************
  * @file     dot.h
  * @author   Gaby Fotio
  * @version  V1.0
  * @date     27.11.2025
  * @brief	  This module initializes the dot-segment hardware, Timer1, Timer2,
  * 			the GPIO pins for PWM output and defines functions for control
  * 			of PWM frequency and brightness
  ******************************************************************************
*/

#ifndef DOT_DOT_H_
#define DOT_DOT_H_

/* Includes */
#include "stm32f4xx.h"
#include <stdint.h>

/**
 * Public functions (prototypes)
 */
void timer1_init(void);
void timer2_init(void);
void dot_init(void);
void oc_init(void);
void set_frequency_hz(uint32_t freq_hz);
void set_brightness(uint32_t adc_val);

/* Extern declarations for timer handles */
extern TIM_HandleTypeDef timer1_handle_struct, timer2_handle_struct;


#endif /* DOT_DOT_H_ */
