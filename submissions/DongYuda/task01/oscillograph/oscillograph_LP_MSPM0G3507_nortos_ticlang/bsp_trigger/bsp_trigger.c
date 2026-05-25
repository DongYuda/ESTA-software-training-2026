#include "bsp_trigger.h"
#include "bsp_voltage_gear.h" // 引入档位倍率
#include "app_measure.h"

volatile uint32_t dac_val = 0; 

//触发电平初始化，默认初始触发电平1.65v
void BSP_Trigger_Init(void)
{
    BSP_Trigger_SetVoltage(0.0f); 
}
//设定触发电压
void BSP_Trigger_SetVoltage(float real_voltage)
{
    g_data.trigger_level_V = real_voltage;

    // 提取出引脚上的实际物理偏差电压
    float pin_offset_V = real_voltage / (VOLT_GEAR_SPAN[g_data.volt_gear] / 3.3f) / vpp_cal_factor;
    
    float v_dac;
    if (g_data.volt_gear == 0) {
        v_dac = 1.65f - pin_offset_V;
    } else {
        v_dac = 1.65f + pin_offset_V;
    }

    if (v_dac < 0.0f) v_dac = 0.0f;
    if (v_dac > DAC_VREF) v_dac = DAC_VREF;

    dac_val = (uint32_t)((v_dac / DAC_VREF) * DAC_MAX_VAL);
    DL_DAC12_output12(DAC0, dac_val); 
    DL_DAC12_enable(DAC0);
}