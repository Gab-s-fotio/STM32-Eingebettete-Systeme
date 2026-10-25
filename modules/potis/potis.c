/**
  ******************************************************************************
  * @file    	potis.c
  * @author		Gaby Fotio
  * @version 	V1.0
  * @date		09.11.2025
  * @brief  	Implementation of analog potentiometer module using ADC & DMA
  @verbatim
  ==============================================================================
		### Resources used ###
  			GPIO: Port A (PA6, PA7)
  			Analog-Digital-Converter (ADC1, ADC2)
  			Direct Memory Access (DMA2)
  ==============================================================================
  		###     Usage      ###
  	(#) Call 'potis_init()' to initialize the module and all the necessary
  		peripheries for analog input
  	(#) Call 'potis_get_val(poti_num)' to get value from a single ADC conversion
  	(#) Call 'dma_init()' to initialize DMA for ADC1 and ADC2 in circular mode
  	(#) Call 'potis_get_avg(*ADC_AVG_values)' calculates the arithmetic mean of
  		the last 100 ADC samples
  ==============================================================================
  @endverbatim
  ******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include <potis/potis.h>

/* Global ADC handle structures for ADC1 & ADC2. */
ADC_HandleTypeDef ADC_handle_structure1, ADC_handle_structure2;

/* save the arithmetic mean of the last 100 ADC samples for ADC1 & ADC2 in array -> circular buffer */
uint16_t u16_ADC1_val[TASTING], u16_ADC2_val[TASTING];

/* save values for ADC1 & ADC2 from single ADC conversion in array */
uint32_t u32_ADC_values[2];

/**
 * @brief	Initialize GPIOA pins, ADC1 & ADC2 peripherals for analog input configuration
 * @param	none
 * @return	none
 */
void potis_init(void)
{
		/* Enable clock for GPIO Port A, ADC1 & ADC2 */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_ADC1_CLK_ENABLE();
		__HAL_RCC_ADC2_CLK_ENABLE();

		/* Declaration of init */
	    GPIO_InitTypeDef init;
	    init.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	    init.Mode  = GPIO_MODE_ANALOG;
	    init.Pull  = GPIO_NOPULL;

		/* Initialize of GPIO Port A */
	    HAL_GPIO_Init(GPIOA, &init);

	    /* Declaration/configuration of ADC1 */
	    ADC_handle_structure1.Instance = ADC1;
	    ADC_handle_structure1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	    ADC_handle_structure1.Init.Resolution = ADC_RESOLUTION_12B;
	    ADC_handle_structure1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	    ADC_handle_structure1.Init.ScanConvMode = DISABLE;
	    ADC_handle_structure1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	    ADC_handle_structure1.Init.ContinuousConvMode = ENABLE;
	    ADC_handle_structure1.Init.NbrOfConversion = 1;
	    ADC_handle_structure1.Init.ExternalTrigConv = ADC_SOFTWARE_START;

		/* Initialize of ADC1 */
	    HAL_ADC_Init(&ADC_handle_structure1);

	    /* Declaration/configuration of ADC1 channel */
	    ADC_ChannelConfTypeDef ADC_channel_structure1;
	    ADC_channel_structure1.Channel = ADC_CHANNEL_6;
	    ADC_channel_structure1.Rank = 1;
	    ADC_channel_structure1.SamplingTime = ADC_SAMPLETIME_84CYCLES;

	    /* Apply the configuration to ADC1 */
	    HAL_ADC_ConfigChannel(&ADC_handle_structure1, &ADC_channel_structure1);

	    /* Declaration/configuration of ADC2 */
	    ADC_handle_structure2.Instance = ADC2;
	    ADC_handle_structure2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	    ADC_handle_structure2.Init.Resolution = ADC_RESOLUTION_12B;
	    ADC_handle_structure2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	    ADC_handle_structure2.Init.ScanConvMode = DISABLE;
	    ADC_handle_structure2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	    ADC_handle_structure2.Init.ContinuousConvMode = ENABLE;
	    ADC_handle_structure2.Init.NbrOfConversion = 1;
	    ADC_handle_structure2.Init.ExternalTrigConv = ADC_SOFTWARE_START;

		/* Initialize of ADC2 */
	    HAL_ADC_Init(&ADC_handle_structure2);

	    /* Declaration/configuration of ADC2 channel */
	    ADC_ChannelConfTypeDef ADC_channel_structure2;
	    ADC_channel_structure2.Channel = ADC_CHANNEL_7;
	    ADC_channel_structure2.Rank = 1;
	    ADC_channel_structure2.SamplingTime = ADC_SAMPLETIME_84CYCLES;

	    /* Apply the configuration to ADC2 */
	    HAL_ADC_ConfigChannel(&ADC_handle_structure2, &ADC_channel_structure2);
}

/**
 * @brief	performs a single ADC conversion manuelly to get value
 * @param	poti_num: potentiometer number (1 or 2)
 * @return	ADC_val: 12-bit ADC value (0–4095)
 */
uint32_t potis_get_val(uint8_t u8_poti_num){
	 /* Variable to store ADC conversion result (12-bit value: 0–4095) */
	 uint32_t u32_ADC_val = 0;

	 /* Variable to store return status of HAL functions (TIMEOUT) */
	 HAL_StatusTypeDef ret;

	 if (u8_poti_num == 1)
	 {
		/* Start ADC1 conversion and wait for result */
	    HAL_ADC_Start(&ADC_handle_structure1);
	    /* Wait (poll) until conversion is finished or timeout after 1000 ms */
	    ret = HAL_ADC_PollForConversion(&ADC_handle_structure1, 1000);
	    /* If function not return TIMEOUT, valid conversion result is available */
	    if(ret != HAL_TIMEOUT)
	    {
	    	/* Read converted digital value from ADC1’s data register (0–4095) */
	    	u32_ADC_val = HAL_ADC_GetValue(&ADC_handle_structure1);
	    	/* Stop the ADC */
	    	HAL_ADC_Stop(&ADC_handle_structure1);
	    	/* Return measured ADC value for poti 1 */
	    	return u32_ADC_val;
	    }
	 }
	 if (u8_poti_num == 2) {
		/* Start ADC2 conversion and wait for result */
	    HAL_ADC_Start(&ADC_handle_structure2);
	    ret = HAL_ADC_PollForConversion(&ADC_handle_structure2, 1000);
	    if(ret != HAL_TIMEOUT)
	    {
	    	u32_ADC_val = HAL_ADC_GetValue(&ADC_handle_structure2);
	        HAL_ADC_Stop(&ADC_handle_structure2);
	        return u32_ADC_val;
	    }
	 }
	 /* Invalid potentiometer number -> return 0 */
	 return 0;
}

/**
  * @brief  Initialize DMA for ADC1 and ADC2 in circular mode.
  *         The DMA continuously transfers ADC results into 100-element buffers.
  * @param  none
  * @retval none
  */
void dma_init(void){

	/* Enable DMA2 peripheral clock used by ADC1 & ADC2 */
	__HAL_RCC_DMA2_CLK_ENABLE();

    /* Declaration/configuration of DMA2 for ADC1 */
	DMA_HandleTypeDef DMA_handle_structure_ADC1;
	DMA_handle_structure_ADC1.Instance = DMA2_Stream4;
	DMA_handle_structure_ADC1.Init.Channel = DMA_CHANNEL_0;
	DMA_handle_structure_ADC1.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMA_handle_structure_ADC1.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_handle_structure_ADC1.Init.MemInc = DMA_MINC_ENABLE;
	DMA_handle_structure_ADC1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DMA_handle_structure_ADC1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	DMA_handle_structure_ADC1.Init.Mode = DMA_CIRCULAR;
	DMA_handle_structure_ADC1.Init.Priority = DMA_PRIORITY_HIGH;
	DMA_handle_structure_ADC1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	/* initialize the DMA for ADC1 */
	HAL_DMA_Init(&DMA_handle_structure_ADC1);

    /* Declaration/configuration of DMA2 for ADC2 */
	DMA_HandleTypeDef DMA_handle_structure_ADC2;
	DMA_handle_structure_ADC2.Instance = DMA2_Stream3;
	DMA_handle_structure_ADC2.Init.Channel = DMA_CHANNEL_1;
	DMA_handle_structure_ADC2.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMA_handle_structure_ADC2.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_handle_structure_ADC2.Init.MemInc = DMA_MINC_ENABLE;
	DMA_handle_structure_ADC2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DMA_handle_structure_ADC2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	DMA_handle_structure_ADC2.Init.Mode = DMA_CIRCULAR;
	DMA_handle_structure_ADC2.Init.Priority = DMA_PRIORITY_HIGH;
	DMA_handle_structure_ADC2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	/* initialize the DMA for ADC2 */
	HAL_DMA_Init(&DMA_handle_structure_ADC2);

	/* Link DMA handles to ADC handles and enable continuous DMA requests */
	ADC_handle_structure1.DMA_Handle = &DMA_handle_structure_ADC1;
	ADC_handle_structure1.Init.DMAContinuousRequests = ENABLE;

	/* Reinitialize ADC1 to apply updated DMA settings */
	HAL_ADC_Init(&ADC_handle_structure1);

	ADC_handle_structure2.DMA_Handle = &DMA_handle_structure_ADC2;
	ADC_handle_structure2.Init.DMAContinuousRequests = ENABLE;

	/* Reinitialize ADC2 to apply updated DMA settings */
	HAL_ADC_Init(&ADC_handle_structure2);

	/* Start DMA for ADC1 & ADC2, single value only */
	//HAL_ADC_Start_DMA(&ADC_handle_structure1, &u32_ADC_values[0], 1);
	//HAL_ADC_Start_DMA(&ADC_handle_structure2, &u32_ADC_values[1], 1);

	/* Start ADC conversions with DMA transfers into 100-element circular buffers */
	HAL_ADC_Start_DMA(&ADC_handle_structure1, u16_ADC1_val, TASTING);
	HAL_ADC_Start_DMA(&ADC_handle_structure2, u16_ADC2_val, TASTING);
}

/**
  * @brief  calculates the arithmetic mean of the last 100 ADC samples.
  *         Used to reduce noise and stabilize ADC output.
  * @param  ADC_AVG_values: pointer to the 100-sample DMA buffer
  * @retval avg: averaged ADC value (0–4095)
  */
uint32_t potis_get_avg(uint16_t *u16_ADC_AVG_values)
{
	/* Variable to save total sum of all samples */
    uint32_t u32_sum = 0;

    /* Variable to store calculated average (0–4095 range) */
    uint16_t u16_avg = 0;

    /* Loop through all 100 samples in DMA buffer */
    for (int i = 0; i < TASTING; ++i){
    	/* Add each ADC value to running sum */
    	u32_sum += u16_ADC_AVG_values[i];
    }
    /* Calculate arithmetic mean (average) of 100 samples */
    u16_avg = u32_sum / TASTING;
    return u16_avg;
}
