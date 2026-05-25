#ifndef __HMI_DISPLAY_H
#define __HMI_DISPLAY_H

#include "stdint.h"

#define WAVE_START_X    0       // 波形显示区左上角的 X 坐标 (右边留白40)
#define WAVE_START_Y    0       // 波形显示区左上角的 Y 坐标 (下部留白64)
#define WAVE_WIDTH      200     // 波形区总宽度 (10格 * 20像素)
#define WAVE_HEIGHT     256     // 波形区总高度 (8格 * 32像素)

typedef struct {
    float    freq_Hz;        // CH1 频率 (Hz)
    float    vpp_V;          // CH1 电压峰值 (V)
    float    freq_Hz_ch2;    // CH2 频率 (Hz)
    float    vpp_V_ch2;      // CH2 电压峰值 (V)
    float    phase_diff;     // 相位差 (度)
    
    float    trigger_level_V;// 触发电平(V)
    float    avg_V;          // 真实平均电压 (供 AUTO 使用)
    
    uint8_t  time_gear;      // 扫描速度档位 
    uint8_t  volt_gear;      // CH1 垂直灵敏度档位
    uint8_t  volt_gear_ch2;  // CH2 垂直灵敏度档位
} MeasureData_t;

extern MeasureData_t g_data;

// 双通道独立显存
extern uint32_t new_wave_ch1[WAVE_WIDTH];
extern uint32_t new_wave_ch2[WAVE_WIDTH];
extern uint8_t need_full_clear_flag;

void HMI_Init(void);
void HMI_DrawGrid(void);
void HMI_parameter_display(void);
void HMI_UpdateWaveform(void);
static void format_fft_percent(char* buff, float val);
void HMI_FFT_display(void);

#endif