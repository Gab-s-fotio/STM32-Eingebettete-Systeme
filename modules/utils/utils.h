/**
  ******************************************************************************
  * @file     utils.h
  * @version  V1.0
  * @date     27.10.2025
  * @brief    Utility functions for timing and GPIO port access.
  ******************************************************************************

*/
#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

/* Includes */
#include <stdint.h>
#include "stm32f4xx.h"

/**
 * Public functions (prototypes)
 */
void utils_delay_ms(uint32_t t);
void utils_gpio_port_write(GPIO_TypeDef* GPIOx, uint16_t value);
uint32_t utils_gpio_port_read(GPIO_TypeDef* GPIOx);


#endif /* UTILS_UTILS_H_ */
