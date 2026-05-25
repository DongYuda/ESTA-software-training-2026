#ifndef APP_FFT_H
#define APP_FFT_H

#include "ti_msp_dl_config.h"
#include "stdint.h"

#define FFT_LENGTH 128

typedef struct {
    float u1_amp;      // 基波绝对幅值
    float u2_percent;  // 二次谐波占比 (%)
    float u3_percent;  // 三次谐波占比 (%)
    float u4_percent;  // 四次谐波占比 (%)
    float u5_percent;  // 五次谐波占比 (%)
    float thd;         // 总谐波失真 (THD)
} FFT_Data_t;

static float get_single_freq_mag(uint16_t *buffer, int num_points, float fs, float target_freq);
extern FFT_Data_t g_fft;
void APP_FFT_Calculate(uint16_t *buffer, uint16_t length);

#endif