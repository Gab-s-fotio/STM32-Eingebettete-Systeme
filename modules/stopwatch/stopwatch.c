/**
  ******************************************************************************
  * @file    	stopwatch.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		27.11.2025
  * @brief  	Implementation of stopwatch module using TIM1  as time base
  *          	 and EXTI0 (user button on PA0) for start/lap event handling
  @verbatim
  ==============================================================================
		### Resources used ###
  			GPIO: Port A (PA0)
  			Timer: TIM1
  			NVIC: EXTI0, TIM1
  ==============================================================================
  		###     Usage      ###
  	(#) Call 'timer1_init_stopwatch()' to configure TIM1 for stopwatch timing.
  	(#) Call 'button_init()' to configure PA0 as EXTI input.
  	(#) Call 'nvic_init()' to enable required interrupts.
  	(#) Call 'TIM1_UP_TIM10_IRQHandler()' TIM1 update interrupt service routine
  	(#) Call 'EXTI0_IRQHandler()' EXTI0 Interrupt (User-Button PA0)

  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include <stopwatch/stopwatch.h>

/* Defines */
#define STOPWATCH_TICK_FREQUENCY 1000000
#define STOPWATCH_PERIOD 100

/* Global stopwatch state variables (allocated here, declared extern in stopwatch.h) */
TIM_HandleTypeDef timer1_handle_struct1;
volatile uint8_t  u8_sw_running = 0;
volatile uint32_t u32_tick_counter = 0;
volatile uint32_t u32_last_lap_ticks = 0;
volatile uint32_t u32_lap_counter = 0;
volatile uint8_t u8_lap_list_size = 0;
volatile uint8_t  u8_lap_refresh = 0;
volatile uint32_t u32_lap_durations[10];
volatile uint32_t u32_current_lap_duration_ticks = 0;
volatile uint8_t u8_last_index = 0;
volatile uint32_t u32_last_button_ticks = 0;

/**
 * @brief   Configure PA0 as external interrupt (EXTI0) for the stopwatch button
 * @param   none
 * @return  none
 */
void button_init(void)
{
    /* Enable clock for GPIOA */
	 __HAL_RCC_GPIOA_CLK_ENABLE();

	/* Enable system configuration controller clock */
	 __HAL_RCC_SYSCFG_CLK_ENABLE();

	 GPIO_InitTypeDef init;
	 init.Pin  = GPIO_PIN_0;
	 init.Mode = GPIO_MODE_IT_RISING;
	 init.Pull = GPIO_NOPULL;
	 HAL_GPIO_Init(GPIOA, &init);
}

/**
 * @brief   Configure NVIC priorities and enable interrupts for EXTI0 (button)
 *          and TIM1 update event (stopwatch tick).
 * @param   none
 * @return  none
 */
void nvic_init(void)
{
	// Timer1 Update Interrupt (Zeitbasis)
	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

	// EXTI0 (User Button auf PA0)
	HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/**
 * @brief   Initialize TIM1 as the stopwatch base timer.
 *          TIM1 is configured to tick every  1 µs (1 MHz update rate).
 * @param   none
 * @return  none
 */
void timer1_init_stopwatch(void)
{
    /* Enable TIM1 peripheral clock */
    __HAL_RCC_TIM1_CLK_ENABLE();

    timer1_handle_struct1.Instance = TIM1;
    timer1_handle_struct1.Init.Prescaler = (SystemCoreClock / STOPWATCH_TICK_FREQUENCY) - 1;
    timer1_handle_struct1.Init.Period = STOPWATCH_PERIOD - 1;
    timer1_handle_struct1.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer1_handle_struct1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timer1_handle_struct1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    timer1_handle_struct1.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&timer1_handle_struct1);
}

/**
 * @brief   Interrupt handler for TIM1 update events.
 * @param   none
 * @return  none
 */void TIM1_UP_TIM10_IRQHandler(void)
{
	 /* Check if the update interrupt flag (UIF) is set */
	    if (__HAL_TIM_GET_FLAG(&timer1_handle_struct1, TIM_FLAG_UPDATE) != RESET)
	    {
	    	 /* Check if the update interrupt source is enabled */
	    	if (__HAL_TIM_GET_IT_SOURCE(&timer1_handle_struct1, TIM_IT_UPDATE) != RESET)
	    	{
	    		/* Clear the update interrupt flag */
	    		__HAL_TIM_CLEAR_IT(&timer1_handle_struct1, TIM_IT_UPDATE);

	    		/* If the stopwatch is running, increment the tick counter */
	    		if (u8_sw_running)
	    		{
	    			u32_tick_counter++;
	    		}
	    	}
	    }
}

 /**
  * @brief   Interrupt handler for the start/lap button (PA0 via EXTI0).
  *          First press starts the stopwatch.
  * @param   none
  * @return  none
  */void EXTI0_IRQHandler(void)
{
	    /* Check if EXTI line 0 has a pending interrupt */
	    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET)
	    {
	        /* Clear the EXTI pending flag */
	        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);

	        /* Debounce */
	        uint32_t now_ticks = u32_tick_counter;
	        if (u8_sw_running && (now_ticks - u32_last_button_ticks) < 1500)
	        {
	        	return;
	        }
	        u32_last_button_ticks = now_ticks;

	        /* If stopwatch not running yet -> start stopwatch */
	        if (!u8_sw_running)
	        {
	        	u8_sw_running  = 1;
	        	u32_last_lap_ticks = u32_tick_counter;
	            __HAL_TIM_ENABLE_IT(&timer1_handle_struct1, TIM_IT_UPDATE);
	            __HAL_TIM_ENABLE(&timer1_handle_struct1);
	        }
	        else
	        {
	        	 /* Stopwatch already running -> register a new lap */
	        	u32_current_lap_duration_ticks = u32_tick_counter - u32_last_lap_ticks;
	        	u32_last_lap_ticks = u32_tick_counter;
	        	u32_lap_counter++;

	        	/* Shift older lap times down Index 0 = newest round, Index 9 = oldest round */
	           	u8_last_index = (u8_lap_list_size < 10) ? u8_lap_list_size : 9;
	        	for (int i = u8_last_index; i > 0; --i)
	        	{
	        		u32_lap_durations[i] = u32_lap_durations[i - 1];
	        	}
	        	 /* Increase list size up to max of 10 */
	        	if (u8_lap_list_size < 10) {
	        		u8_lap_list_size++;
	        	}
	            /* Insert newest lap time at index 0 */
	        	u32_lap_durations[0] = u32_current_lap_duration_ticks;

	            /* Signal the main loop to refresh LCD */
	        	u8_lap_refresh = 1;
	        }
	    }
}



