/**
  ******************************************************************************
  * @file     fan.h
  * @author   Gaby Fotio
  * @version  V1.0
  * @date     19.12.2025
  * @brief	  This module initializes a PI-controlled fan using PWM output and
  *           tachometer-based speed feedback
  ******************************************************************************
*/

#ifndef FAN_FAN_H_
#define FAN_FAN_H_

/* Includes */
#include "stm32f4xx.h"

/**
 * Public functions (prototypes)
 */
void fan_Init(void);
void fan_timer3_init(void);
void fan_timer4_init(void);
void fan_oc_init(void);
void fan_nvic_init(void);
void EXTI9_5_IRQHandler(void);
void TIM4_IRQHandler(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void fan_setDuty(uint8_t duty_percent);
void fan_controlDuty(void);

/* Extern declarations of global fan variables */
extern volatile uint32_t u32_rpm_actual;
extern volatile uint32_t u32_rpm_target;

#endif /* FAN_FAN_H_ */
