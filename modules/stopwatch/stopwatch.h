/**
  ******************************************************************************
  * @file     stopwatch.h
  * @author   Gaby Fotio
  * @version  V1.0
  * @date     27.11.2025
  * @brief	  Custom of stopwatch module using TIM1 as time base
  * 		  and EXTI0 (user button on PA0) for start/lap event handling
  ******************************************************************************
*/

#ifndef STOPWATCH_STOPWATCH_H_
#define STOPWATCH_STOPWATCH_H_

/* Includes */
#include "stm32f4xx.h"
#include <stdint.h>

/* Extern declarations of global stopwatch variables */
extern TIM_HandleTypeDef timer1_handle_struct1;
extern volatile uint8_t  u8_sw_running;   // 0 = stopwatch is stopped, 1 = stopwatch is running
extern volatile uint32_t u32_tick_counter;   // 1 Tick = 1/10.000 s = 0,1 ms -> Anzahl Ticks
extern volatile uint32_t u32_last_lap_ticks; // Zeitpunkt letzte Lap-Auslösung
extern volatile uint32_t u32_lap_counter;	// Gesamt Rundenanzahl
extern volatile uint8_t u8_lap_list_size; // Anzahl gespeicherter Runden (max. 10)
extern volatile uint8_t  u8_lap_refresh; // Soll Anzeige aktualisiert werden? 0 = Nein, 1 = Ja
extern volatile uint32_t u32_lap_durations[10];      // Rundendauern der letzten max. 10 Runden
extern volatile uint32_t u32_current_lap_duration_ticks;
extern volatile uint8_t u8_last_index;

/**
 * Public functions (prototypes)
 */
void button_init(void);
void nvic_init(void);
void timer1_init_stopwatch(void);
void TIM1_UP_TIM10_IRQHandler(void);
void EXTI0_IRQHandler(void);

#endif /* STOPWATCH_STOPWATCH_H_ */
