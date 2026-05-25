#ifndef BSP_VOLTAGE_GEAR_H
#define BSP_VOLTAGE_GEAR_H

#include "ti_msp_dl_config.h"
#include "hmi_display.h"

// 导出给外部使用的参数表
extern const float VOLT_GEAR_SPAN[3];
extern const char* VOLT_GEAR_STRING[3];

void BSP_VoltGear_Init(void);
void BSP_VoltGear_Scan(void);
void BSP_VoltGear_SetTarget(uint8_t target_gear);

#endif