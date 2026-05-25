#include "app_auto.h"
#include "app_measure.h"
#include "bsp_trigger.h"
#include "bsp_voltage_gear.h"
#include "bsp_adc_dma.h"

// 真实时间表 (单位：秒) 静态隐藏在文件内部，不污染全局
static const float TIME_DIV_S[16] = {
    10e-6f, 20e-6f, 50e-6f, 100e-6f,
    0.2e-3f, 0.5e-3f, 1e-3f, 2e-3f,
    5e-3f, 10e-3f, 20e-3f, 50e-3f,
    0.1f, 0.2f, 0.5f, 1.0f
};

void APP_AUTO_Execute(void)
{
    // 垂直灵敏度自适应 (最高优先级，解决震荡问题)
    uint8_t target_volt_gear = g_data.volt_gear;
    
    // 计算此时在 ADC 引脚上的真实物理电压摆幅 (0~3.3V，代表屏幕占比)
    float raw_pin_vpp = g_data.vpp_V / (VOLT_GEAR_SPAN[g_data.volt_gear] / 3.3f) / vpp_cal_factor;

    if (raw_pin_vpp > 3.1f) 
    {
        // 削顶防爆：只要快破表了，无脑回退到最大量程 (0档，16V满量程)，最安全
        target_volt_gear = 0; 
    }
    else if (g_data.volt_gear == 0 && raw_pin_vpp < 0.031f) 
    {
        // 极小信号：哪怕放大 100 倍也才 3.1V，直接一步到位跳到最高灵敏度 (2档)
        target_volt_gear = 2;
    }
    else if (raw_pin_vpp < 0.31f) 
    {
        // 偏小信号：放大 10 倍绝对安全，不会削顶
        if (target_volt_gear < 2) target_volt_gear++;
    }
    
    // 执行换挡
    if (target_volt_gear != g_data.volt_gear) {
        BSP_VoltGear_SetTarget(target_volt_gear); 
    }

    // 触发电平归中

    BSP_Trigger_SetVoltage(g_data.avg_V);

    // 水平时基自适应
    if (g_data.freq_Hz > 10.0f) 
    {
        // 要求显示 1~5 个波形，我们直接取中值 3.5 个！
        // 10格总时间 = 3.5 / freq_Hz，所以每一格目标时间 = 0.35f / freq_Hz
        float target_time_div = 0.35f / g_data.freq_Hz;
        int best_gear = 15;
        
        for (int i = 0; i < 16; i++) 
        {
            if (TIME_DIV_S[i] >= target_time_div) {
                best_gear = i;
                break;
            }
        }
        if (best_gear != g_data.time_gear) {
            BSP_ADC_SetTimebase(best_gear); 
        }
    }
    else 
    {
        // 屏幕波形 <= 1，测不出频率！
        // 暴力向外推 3 个档位（减慢扫描速度），让掩藏的波形全部露出来！
        int target_gear = g_data.time_gear + 3;
        if (target_gear > 15) target_gear = 15;
        
        if (target_gear != g_data.time_gear) {
            BSP_ADC_SetTimebase(target_gear);
        }
    }
}
