/**
  ******************************************************************************
  * @file    	dot.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		27.11.2025
  * @brief  	Implementation of dot-segment PWM control using TIM1 and TIM2
  @verbatim
  ==============================================================================
		### Resources used ###
  			GPIO: Port D (PD11, PD14), Port E (PE11)
  			Timer (TIM1, TIM2)
  ==============================================================================
  		###     Usage      ###
  	(#) Call 'timer1_init()' to initialize TIM1 as PWM time base
  	(#) Call 'timer2_init()' to initialize TIM2 as free-running counter
  	(#) Call 'dot_init()' to configure GPIO pins for PWM and control signals
  	(#) Call 'oc_init()' to configure TIM1 OC channel in PWM mode
  	(#) Call 'set_frequency_hz(freq_hz)' to adjust PWM frequency at runtime
  	(#) Call 'set_brightness(adc_val)' to adjust PWM duty cycle (brightness)
  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include <dot/dot.h>

/* Defines */
#define STOPWATCH_TICK_FREQUENCY 10000
#define STOPWATCH_PERIOD 10000

/* Global timer and PWM configuration handles */
TIM_HandleTypeDef timer1_handle_struct, timer2_handle_struct;
TIM_OC_InitTypeDef tim_oc_handle_struct;
GPIO_InitTypeDef init;
GPIO_InitTypeDef init2;

/**
 * @brief   Initialize TIM1 as base timer with prescaler for 10kHz and period of 1 s
 * @param   none
 * @return  none
 */
void timer1_init(void)
{
    /* Enable TIM1 peripheral clock */
	__HAL_RCC_TIM1_CLK_ENABLE();

	timer1_handle_struct.Instance = TIM1;
	timer1_handle_struct.Init.Prescaler = (SystemCoreClock / STOPWATCH_TICK_FREQUENCY) - 1;
	timer1_handle_struct.Init.Period = STOPWATCH_PERIOD - 1;
	timer1_handle_struct.Init.CounterMode = TIM_COUNTERMODE_UP;
	timer1_handle_struct.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timer1_handle_struct.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	timer1_handle_struct.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&timer1_handle_struct);
	HAL_TIM_Base_Start(&timer1_handle_struct);
}

/**
 * @brief   Initialize TIM2 as time time with prescaler for 10kHz and period of 1 s
 * @param   none
 * @return  none
 */
void timer2_init(void)
{
    /* Enable TIM2 peripheral clock */
	__HAL_RCC_TIM2_CLK_ENABLE();

	timer2_handle_struct.Instance = TIM2;
	timer2_handle_struct.Init.Prescaler = ((SystemCoreClock/2) / STOPWATCH_TICK_FREQUENCY) - 1;
	timer2_handle_struct.Init.Period = STOPWATCH_PERIOD - 1;
	timer2_handle_struct.Init.CounterMode = TIM_COUNTERMODE_UP;
	timer2_handle_struct.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timer2_handle_struct.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	timer2_handle_struct.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&timer2_handle_struct);
	HAL_TIM_Base_Start(&timer2_handle_struct);
}

/**
 * @brief   Initialize GPIO pins for dot-segment and PWM output.
 * @param   none
 * @return  none
 * @notes   - PE11 as TIM1_CH2 in alternate function mode (PWM output)
 *   		- PD11, PD14 as push-pull outputs (control / enable signals)
 */
void dot_init(void)

{
    /* Enable clocks for GPIOD and GPIOE */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

    /* Configure PE11 as alternate function TIM1_CH2 */
	init.Pin = GPIO_PIN_11;
	init.Mode = GPIO_MODE_AF_PP;
	init.Pull = GPIO_NOPULL;
	init.Speed = GPIO_SPEED_MEDIUM;
	init.Alternate = GPIO_AF1_TIM1;

    /* Configure PD11 and PD14 as general-purpose outputs */
	init2.Pin = GPIO_PIN_14 | GPIO_PIN_11;
	init2.Mode = GPIO_MODE_OUTPUT_PP;
	init2.Pull = GPIO_NOPULL;
	init2.Speed = GPIO_SPEED_MEDIUM;

	HAL_GPIO_Init(GPIOE, &init);
	HAL_GPIO_Init(GPIOD, &init2);

    /* Set PD11 and PD14 high (enable / status outputs) */
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET);
}

/**
 * @brief   Initialize TIM1 Output-Compare channel in PWM mode.
 * @param   none
 * @return  none
 * @notes   Configures TIM1 Channel 2 (on PE11) as PWM1
 */
void oc_init(void)
{
	tim_oc_handle_struct.OCMode = TIM_OCMODE_PWM1;
	tim_oc_handle_struct.Pulse = 5000;
	tim_oc_handle_struct.OCIdleState = TIM_OCIDLESTATE_SET;
	tim_oc_handle_struct.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim_oc_handle_struct.OCFastMode = TIM_OCFAST_DISABLE;

	/* Configure TIM1 Channel 2 with the above settings */
	HAL_TIM_OC_ConfigChannel(&timer1_handle_struct, &tim_oc_handle_struct,
	TIM_CHANNEL_2);

	/* starts PWM on TIM1 Channel 2 */
	HAL_TIM_PWM_Start(&timer1_handle_struct, TIM_CHANNEL_2);
}

/**
 * @brief   Set the PWM frequency (via TIM1 prescaler)
 * @param   u32_freq_hz
 * @return  none
 */
void set_frequency_hz(uint32_t u32_freq_hz){
	/* Base prescaler for 10 kHz is (SystemCoreClock / 10000) - 1.
	 * Dividing by u32_freq_hz changes dynamically the prescaler of TIM1 to get PWM frequency. */
    __HAL_TIM_SET_PRESCALER(&timer1_handle_struct,((SystemCoreClock / STOPWATCH_TICK_FREQUENCY) - 1) / u32_freq_hz);
}

/**
 * @brief   Set brightness based on ADC value
 * @param   u32_adc_val
 * @return  none
 */
void set_brightness(uint32_t u32_adc_val){
	uint32_t u32_period = timer1_handle_struct.Init.Period + 1;
	uint32_t u32_pulse = (u32_adc_val * u32_period) / 4095;
	__HAL_TIM_SET_COMPARE(&timer1_handle_struct, TIM_CHANNEL_2, u32_pulse);
}

