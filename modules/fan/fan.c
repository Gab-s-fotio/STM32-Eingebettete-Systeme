/**
  ******************************************************************************
  * @file    	fan.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		19.12.2025
  * @brief  	Implementation of a PI-controlled fan using PWM output and tachometer
  * 			feedback
  @verbatim
  ==============================================================================
		### Resources used ###
  			GPIO: Port B (PB4, PB7)
  			Timer: TIM3, TIM4
  			Hardware: 4-pin PWM fan (GND, +5V, Tacho, PWM), LCD
  ==============================================================================
  		###     Usage      ###
  	(#) Call 'fan_Init()' to configure GPIO pins for PWM output and tacho input
  	(#) Call 'fan_timer3_init()' to initialize TIM3 PWM time base (25 kHz)
  	(#) Call 'fan_timer4_init()' to initialize TIM4 as free-running counter for timing
  	(#) Call 'fan_oc_init()' to Configure TIM3 output compare channel for PWM
  	(#) Call 'fan_nvic_init()' to enable EXTI and timer interrupts in NVIC
  	(#) Call 'fan_setDuty(duty_percent)' to set PWM duty cycle in percent (0–100)
  	(#) Call 'fan_controlDuty()' compute PI controller output (duty cycle)
  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include <fan/fan.h>
#include <median/median.h>
#include <dot/dot.h>

/* Defines */
#define TICK_FREQUENCY_TIMER3 10000000
#define PERIOD 400
#define TICK_FREQUENCY_TIMER4 10000
#define TACHO_PULSES_PER_REV 2
#define RPM_NUMERATOR (60 * TICK_FREQUENCY_TIMER4)
#define MAX_PWM 100
#define MIN_PWM 20

/* Global fan state variables (allocated here, declared extern in fan.h) */
GPIO_InitTypeDef init_1, init_2;
TIM_HandleTypeDef timer3_handle_struct, timer4_handle_struct;
TIM_OC_InitTypeDef timer3_oc_handle_struct;
volatile uint32_t u32_rpm_actual = 0; //actual value
volatile uint32_t u32_rpm_target = 0; //target value
uint32_t u32_tacho_pulse = 0; 	// Current timer value  at tachometer
uint32_t u32_tacho_pulse_prev = 0;	 // Previous timer value
uint32_t u32_tacho_pulse_difference = 0; // Time between two tachometer pulses
uint32_t u32_rpm_unfiltered = 0; // Raw calculated RPM value
uint32_t u32_rpm_filtered = 0;  // Median-filtered RPM value
uint32_t u32_timer3_period_ticks = 0; // Auto-reload value of TIM3
uint32_t u32_timer3_compare_value = 0; // Compare value for PWM duty cycle

/* PI-Controller parameters */
float f_kp = 0.2; // Proportional gain
float f_ki = 0.4; // Integral gain
float f_ta = 0.01; // Sampling time
volatile uint32_t u32_duty_cycle = 0; // Calculated PWM duty cycle in percent
volatile float f_e = 0; // Control error (target - actual RPM)
volatile float f_esum = 0; // Integral of the control error
volatile float f_y; // Controller output before saturation

/**
 * @brief   Initializes GPIO pins used for fan control
 * 			PB4 -> TIM3_CH1 (PWM output, open-drain, active low)
 * 			PB7 -> Fan tachometer input (EXTI rising edge, pull-up enabled)
 * @param   none
 * @return  none
 */
void fan_Init(void){

    /* Enable clock for GPIOB */
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/* Configure PB4 as alternate function TIM3 PWM output */
	init_1.Pin = GPIO_PIN_4;
	init_1.Mode = GPIO_MODE_AF_OD; // Open-drain output required by fan
	init_1.Pull = GPIO_NOPULL;
	init_1.Speed = GPIO_SPEED_MEDIUM;
	init_1.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOB, &init_1);

	/* Configure PB7 as external interrupt for tachometer signal */
	init_2.Pin = GPIO_PIN_7;
	init_2.Mode = GPIO_MODE_IT_RISING; // Rising edge of tachometer pulse
	init_2.Pull = GPIO_PULLUP; // Internal pull-up resistor
	init_2.Speed = GPIO_SPEED_MEDIUM;
	HAL_GPIO_Init(GPIOB, &init_2);
}

/**
 * @brief   Initialize TIM3 as PWM time base
 *          TIM3 is configured to generate a PWM signal with a frequency
 *          of 25 kHz (required for fan)
 * @param   none
 * @return  none
 */
void fan_timer3_init(void){

    /* Enable TIM3 peripheral clock */
	__HAL_RCC_TIM3_CLK_ENABLE();

	timer3_handle_struct.Instance = TIM3;
	timer3_handle_struct.Init.Prescaler = (SystemCoreClock / TICK_FREQUENCY_TIMER3) - 1; //25 kHz
	timer3_handle_struct.Init.Period = PERIOD - 1;
	timer3_handle_struct.Init.CounterMode = TIM_COUNTERMODE_UP;
	timer3_handle_struct.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timer3_handle_struct.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_PWM_Init(&timer3_handle_struct);
}

/**
 * @brief   Initialize TIM4 as a free-running timer
 *          TIM4 measures the time between two tachometer pulses (EXTI interrupt handler)
 * @param   none
 * @return  none
 */
void fan_timer4_init(void){

    /* Enable TIM4 peripheral clock */
	__HAL_RCC_TIM4_CLK_ENABLE();

	timer4_handle_struct.Instance = TIM4;
	timer4_handle_struct.Init.Prescaler = (SystemCoreClock / TICK_FREQUENCY_TIMER4) - 1; //10 kHz
	timer4_handle_struct.Init.Period = 0xFFFF; // Free-running 16-bit counter = 65535
	timer4_handle_struct.Init.CounterMode = TIM_COUNTERMODE_UP;
	timer4_handle_struct.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	timer4_handle_struct.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&timer4_handle_struct);

	HAL_TIM_Base_Start_IT(&timer4_handle_struct);
}

/**
 * @brief   Initialize TIM3 Output-Compare channel in PWM mode.
 * @param   none
 * @return  none
 * @notes   Configures TIM3 Channel 1 (on PB4) as PWM1
 */
void fan_oc_init(void)
{
	timer3_oc_handle_struct.OCMode = TIM_OCMODE_PWM1;
	timer3_oc_handle_struct.Pulse = 100;
	timer3_oc_handle_struct.OCIdleState = TIM_OCIDLESTATE_SET;
	timer3_oc_handle_struct.OCPolarity = TIM_OCPOLARITY_LOW; // Active-low PWM requirements of fan(open-drain PWM input)
	timer3_oc_handle_struct.OCFastMode = TIM_OCFAST_DISABLE; // low = Signal active

	/* Configure TIM3 Channel 1 with the above settings */
	HAL_TIM_PWM_ConfigChannel(&timer3_handle_struct, &timer3_oc_handle_struct,
	TIM_CHANNEL_1);

	/* starts PWM generation on TIM3 Channel 1 */
	HAL_TIM_PWM_Start(&timer3_handle_struct, TIM_CHANNEL_1);
}

/**
 * @brief   Configure NVIC priorities and enable interrupts
 * @param   none
 * @return  none
 */
void fan_nvic_init(void)
{
	/* TIM4 interrupt (used as time base for RPM measurement) */
	HAL_NVIC_SetPriority(TIM4_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM4_IRQn);

	/* EXTI interrupt for tachometer signal (PB7) */
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/**
 * @brief EXTI interrupt handler for GPIO pins 5 to 9.
 * Forwards the interrupt handling to the HAL EXTI callback.
 * PB7 lies on EXTI line 7, which belongs to group EXTI5–9.
 * @param none
 * @return none
 */
void EXTI9_5_IRQHandler(void) {
	    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
}

/**
 * @brief Interrupt handler for timer 4
 * @param none
 * @return none
 */
void TIM4_IRQHandler(void) {
	    HAL_TIM_IRQHandler(&timer4_handle_struct);
	    /* Required by the HAL library even though TIM4
	     * is only used as a free-running counter. */
}

/**
 * @brief EXTI callback for the fan tachometer signal
 *  	  function measures time between two tachometer pulses
 * 		  using TIM4 and calculates current fan speed in RPM.
 * 		  PI controller is executed after each new RPM measurement.
 * @param GPIO_Pin
 * @return none
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	 // Check if the interrupt was triggered by the fan tachometer pin (PB7)
    if (GPIO_Pin == GPIO_PIN_7)
    {
    	 // Read the current value of TIM4 (free-running timer)
    	u32_tacho_pulse  = __HAL_TIM_GET_COUNTER(&timer4_handle_struct);

        // Check if the timer counter did not overflow since the last pulse
        if (u32_tacho_pulse >= u32_tacho_pulse_prev)
        {
            // Calculate time difference between two pulses (no overflow)
        	u32_tacho_pulse_difference = u32_tacho_pulse - u32_tacho_pulse_prev;
        }
        else
        {
        	// Calculate time difference considering timer overflow
        	u32_tacho_pulse_difference = (0xFFFF - u32_tacho_pulse_prev + 1) + u32_tacho_pulse;
        }
        // Store current timer value for the next interrupt
        u32_tacho_pulse_prev = u32_tacho_pulse;

        // Ensure the measured time difference is valid
        if (u32_tacho_pulse_difference > 0)
        {
        	// Calculate raw RPM value based on pulse period and timer frequency ( 600000 / diff * 2)
        	u32_rpm_unfiltered = RPM_NUMERATOR / (u32_tacho_pulse_difference * TACHO_PULSES_PER_REV);

            // Apply median filter to suppress measurement noise and outliers
        	u32_rpm_filtered = median_get_median(u32_rpm_unfiltered) / 2;

            // Store the filtered RPM value as the actual fan speed
        	u32_rpm_actual = u32_rpm_filtered;
        }

        // Execute PI controller to calculate new PWM duty cycle
        fan_controlDuty();

        // Apply the calculated duty cycle to the PWM output
        fan_setDuty((uint8_t)u32_duty_cycle);
    }
}

/**
 * @brief sets the PWM duty cycle in percent
 * @param duty_percent
 * @return none
 */
void fan_setDuty(uint8_t duty_percent)
{
    // Limit duty cycle to a maximum of 100%
	 if (duty_percent > 100)
	        duty_percent = 100;
	  	//The duty cycle is inverted because the PWM signal is active-low
	    duty_percent = 100 - duty_percent;

		// Read the auto-reload value (ARR) of TIM3 (PWM period duration)
	    u32_timer3_period_ticks = __HAL_TIM_GET_AUTORELOAD(&timer3_handle_struct);

		// Calculate the compare value corresponding to the desired duty cycle
	    u32_timer3_compare_value = (((uint64_t)(u32_timer3_period_ticks + 1) * duty_percent) / 100);

		// Update TIM3 compare register to set the new PWM duty cycle
	    __HAL_TIM_SET_COMPARE(&timer3_handle_struct, TIM_CHANNEL_1, u32_timer3_compare_value);
}

/**
 * @brief PI controller for fan speed regulation
 * @param none
 * @return none
 */
void fan_controlDuty(void){

    // Calculate control error: difference between target RPM and actual RPM
	f_e = (float)u32_rpm_target - (float)u32_rpm_actual;

    // Accumulate the error for the integral term
    f_esum = f_esum + f_e;

    // Calculate proportional term of the PI controller and
    // add integral term scaled by the sampling time
    f_y = f_kp * f_e + f_ki * f_ta * f_esum;

    // Store controller output as duty cycle (%)
    u32_duty_cycle = (uint32_t)f_y;

    // Apply upper saturation limit to the actuator (PWM)
    if (f_y > MAX_PWM)
    {
        // Limit duty cycle to maximum allowed value
    	u32_duty_cycle = MAX_PWM;

        // Anti-windup: revert last integral step
    	f_esum = f_esum - f_e;
    }
    // Apply lower saturation limit to the actuator (PWM)
    else if (f_y < MIN_PWM)
    {
        // Limit duty cycle to minimum allowed value
    	u32_duty_cycle = MIN_PWM;

        // Anti-windup: revert last integral step
    	f_esum = f_esum - f_e;
    }
}


