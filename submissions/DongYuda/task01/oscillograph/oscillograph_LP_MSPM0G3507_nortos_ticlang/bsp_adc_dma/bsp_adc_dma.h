#ifndef __BSP_ADC_DMA_H
#define __BSP_ADC_DMA_H

#include "ti_msp_dl_config.h"
#include "stdint.h"
#include "hmi_display.h"

extern uint16_t adc_buffer[WAVE_WIDTH];
extern uint16_t adc_buffer_ch2[WAVE_WIDTH];
extern volatile uint8_t dma_transfer_done_flag;

void BSP_ADC_DMA_Init(void);
void BSP_ADC_DMA_Start(void);
void BSP_ADC_SetTimebase(uint8_t idx);

#endif