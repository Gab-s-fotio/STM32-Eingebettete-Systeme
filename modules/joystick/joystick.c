/**
  ******************************************************************************
  * @file     joystick.c
  * @author   Gaby Fotio
  * @version  V1.0
  * @date     26.10.2025
  * @brief    Implementation file for using the joystick input module
  @verbatim
  ==============================================================================
		### Resources used ###
  			GPIO: PG6, PG9, PG10, PG11, PG12
  ==============================================================================
  ###     Usage      ###
  	(#) Call 'joystick_init()' initializes GPIO pins used for the joystick
  	(#) Call 'joystick_get_action()' reads the current joystick state
  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include <utils/utils.h>
#include <joystick/joystick.h>

/* defines */
/* Pin mapping for joystick directions */
#define JOYSTICK_UP GPIO_PIN_11 //PG11 D
#define JOYSTICK_RIGHT GPIO_PIN_10 //PG10 C
#define JOYSTICK_DOWN  GPIO_PIN_6 //PG6 A
#define JOYSTICK_LEFT GPIO_PIN_9 //PG9 B
#define JOYSTICK_PRESS GPIO_PIN_12 //PG12 CTR

/**
  * @brief 	Initializes GPIO pins used for the joystick
  * @param 	None
  * @return None
  */
void joystick_init(void)
{
	/* Enable clock for GPIO port G */
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/*Declaration of init */
    GPIO_InitTypeDef init;
    init.Mode  = GPIO_MODE_INPUT; /* input mode specifies that the configured pins are to be used as inputs. */
    init.Pull  = GPIO_PULLUP; /* resistance deactivated: pressed = 0 (LOW) = active, released = 1 (HIGH) = inactive */
    init.Speed = GPIO_SPEED_MEDIUM; /* speed of the pin driver */

    /* Initialize all directions and press pins */
    init.Pin = JOYSTICK_UP | JOYSTICK_RIGHT | JOYSTICK_DOWN | JOYSTICK_LEFT | JOYSTICK_PRESS;

	/* Initialize of GPIO port G */
    HAL_GPIO_Init(GPIOG, &init);
  }

/**
  * @brief 	Reads the current joystick state
  * @param	None
  * @return	current joystick state
  */
joystick_action_t joystick_get_action(void)
{
	/* Read the current GPIO input register of port G */
    uint32_t port_state = utils_gpio_port_read(GPIOG);

    /* pressed/released (low-active) if bit = 0 on port_state in place of PG.. */
    uint8_t up    = !(port_state & JOYSTICK_UP);
    uint8_t right = !(port_state & JOYSTICK_RIGHT);
    uint8_t down  = !(port_state & JOYSTICK_DOWN);
    uint8_t left  = !(port_state & JOYSTICK_LEFT);
    uint8_t press = !(port_state & JOYSTICK_PRESS);

    /* Check single directions */
    if (up)           return UP;
    if (down)         return DOWN;
    if (left)         return LEFT;
    if (right)        return RIGHT;
    if (press)        return PRESS;

    /* if no joystick activity */
    return NO_INPUT;
   }

