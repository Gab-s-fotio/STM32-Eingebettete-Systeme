/**
  ******************************************************************************
  * @file     potis.h
  * @author   Gaby Fotio
  * @version  V1.0
  * @date     09.11.2025
  * @brief    Custom of analog potentiometer module for using ADC & DMA
  ******************************************************************************
*/

#ifndef POTIS_POTIS_H_
#define POTIS_POTIS_H_

/* Includes */
#include "stm32f4xx.h"
#include <stdint.h>

/* define */
#define TASTING 100

/**
 * Public functions (prototypes)
 */
void potis_init(void);
uint32_t potis_get_val(uint8_t poti_num);
void dma_init(void);
uint32_t potis_get_avg(uint16_t *ADC_AVG_values);

/* save the arithmetic mean of the last 100 ADC samples for ADC1 & ADC2 in array. */
extern uint16_t u16_ADC1_val[TASTING], u16_ADC2_val[TASTING];

/* save values for ADC1 & ADC2 from single ADC conversion in array */
extern uint32_t u32_ADC_values[2];

#endif /* POTIS_POTIS_H_ */


