/**
  ******************************************************************************
  * @file    	utils.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		25.10.2025
  * @brief  	Utility functions for timing and GPIO port access
  ******************************************************************************
		### Resources used ###

  ==============================================================================
  		###     Usage      ###
  	(#) Call 'utils_delay_ms(t)' for 't' milliseconds delay
  	(#) Call 'utils_gpio_port_write(port, value)' sets and unsets pins from specific GPIO port with a pin mask
  	(#) Call 'utils_gpio_port_read(port)' reads which pins are set or unset of a GPIO Port
  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* Include */
#include "utils.h"
#include <dot/dot.h>

/**
 * @brief Delays program execution for a given time in milliseconds
 * @param t: delay duration in milliseconds
 * @note HAL_Delay(t) uses the SysTick timer, configured in HAL_Init() to interrupt every 1 ms.
 * 		 Each interrupt increments the internal uwTick counter, and the function waits until it
 * 		 has increased by t ms * 10. Its accuracy depends on the system clock and correct HAL initialization.
 * @return none
 */
void utils_delay_ms(uint32_t t) {
		//HAL_Delay(t);
	uint32_t u32_ticks = t * 10;
	__HAL_TIM_SET_COUNTER(&timer2_handle_struct, 0);
	while (__HAL_TIM_GET_COUNTER(&timer2_handle_struct) < u32_ticks);
}

/**
 * @brief Function directly accesses the ODR register of the specified port and
 * 		  determines which pin is currently HIGH or LOW
 * @param port: pointer to the GPIO port
 * @param value: pin mask to define which bits set or not
 * @note Output Data Register (ODR)
 * @return none
 */
void utils_gpio_port_write(GPIO_TypeDef* port, uint16_t value){
	 port->ODR ^= value;
}

/**
 * @brief Function reads the IDR register of the specified port and
 * 		  shows which pins are currently HIGH or LOW
 * @param GPIO Port
 * @note Input Data Register (IDR)
 * @return current IDR from selected Port
 */
uint32_t utils_gpio_port_read(GPIO_TypeDef* port){
	return port->IDR;
}
