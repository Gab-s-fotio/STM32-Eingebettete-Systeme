/**
  ******************************************************************************
  * @file     joystick.h
  * @author   Gaby Fotio
  * @version  V1.0
  * @date     26.10.2025
  * @brief    Module for using the joystick
  ******************************************************************************
*/

#ifndef JOYSTICK_JOYSTICK_H_
#define JOYSTICK_JOYSTICK_H_

/* Includes */
#include "stm32f4xx.h"

/* enum representing all possible joystick actions */
typedef enum {
	UP,
	RIGHT,
	DOWN,
	LEFT,
	PRESS,
	NO_INPUT
} joystick_action_t;

/**
 * Public functions (prototypes)
 */
void joystick_init(void);
joystick_action_t joystick_get_action(void);

#endif /* JOYSTICK_JOYSTICK_H_ */
