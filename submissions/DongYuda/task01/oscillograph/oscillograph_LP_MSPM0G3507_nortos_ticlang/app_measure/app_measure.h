#ifndef APP_MEASURE_H
#define APP_MEASURE_H

#include "ti_msp_dl_config.h"
#include "stdint.h"
#include "hmi_display.h"

#define ADC_VREF          3.3f
#define ADC_RESOLUTION    4095.0f

extern float vpp_cal_factor;

extern float vpp_cal_factor_ch2;

void APP_Measure_Calculate(uint16_t *buffer, uint16_t length);
void APP_Measure_Calculate_CH2(uint16_t *buffer, uint16_t length);
void APP_Measure_Calculate_Phase(uint16_t *buf1, uint16_t *buf2, uint16_t length);
void APP_Measure_DoCalibration(void);
int cmp_uint16(const void *a, const void *b);

#endif