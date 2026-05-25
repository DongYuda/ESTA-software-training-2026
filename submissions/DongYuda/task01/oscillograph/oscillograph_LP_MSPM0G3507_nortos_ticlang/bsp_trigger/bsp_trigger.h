#ifndef BSP_TRIGGER_H
#define BSP_TRIGGER_H

#include "ti_msp_dl_config.h"
#include "hmi_display.h"
#include "stdint.h"

#define DAC_VREF        3.3f 
#define DAC_MAX_VAL     4095.0f

void BSP_Trigger_Init(void);
void BSP_Trigger_SetVoltage(float voltage);


#endif