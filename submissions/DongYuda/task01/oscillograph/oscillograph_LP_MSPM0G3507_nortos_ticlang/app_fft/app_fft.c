#include "app_fft.h"
#include "math.h"
#include "hmi_display.h" 

FFT_Data_t g_fft;

#define PI 3.1415926535f

// ========================================================
// 改进版：带去直流与 Hanning 窗的单频点 DFT
// ========================================================
static float get_single_freq_mag(uint16_t *buffer, int num_points, float fs, float target_freq)
{
    if (num_points <= 0) return 0.0f;

    // 先算出这批数据的直流偏置 (DC Offset)
    float dc_offset = 0.0f;
    for (int i = 0; i < num_points; i++) {
        dc_offset += (float)buffer[i];
    }
    dc_offset /= (float)num_points;

    float sum_re = 0.0f;
    float sum_im = 0.0f;
    float omega = 2.0f * PI * target_freq / fs;

    for (int i = 0; i < num_points; i++) {
        // 去除可怕的直流分量，防止它在非整数周期时污染谐波
        float val = (float)buffer[i] - dc_offset;
        
        // 加汉宁窗 (Hanning Window)
        // 消除首尾不连续带来的频谱泄露
        float window = 0.5f * (1.0f - cosf(2.0f * PI * i / (num_points - 1)));
        val = val * window;

        // 正交相关提取能量
        sum_re += val * cosf(omega * i);
        sum_im += val * sinf(omega * i);
    }
    
    // 汉宁窗会损失一半的能量（相干增益为0.5），所以归一化时要额外乘以 2
    // 原始公式是 2.0 * sqrt / N，加窗后变成 4.0 * sqrt / N
    return 4.0f * sqrtf(sum_re*sum_re + sum_im*sum_im) / num_points;
}

// ========================================================
// FFT 主计算调度函数
// ========================================================
void APP_FFT_Calculate(uint16_t *buffer, uint16_t length)
{
    // 如果频率太低或者没测出频率，直接清零
    if (g_data.freq_Hz < 10.0f) goto CLEAR_FFT;

    extern const float SAMPLE_RATE_LUT[16];
    float fs = SAMPLE_RATE_LUT[g_data.time_gear]; 
    
    // 奈奎斯特定理：采样率必须大于目标频率的 2 倍
    // 我们要算 5 次谐波，理论上最好 fs > freq * 10
    if (fs <= g_data.freq_Hz * 2.0f) goto CLEAR_FFT; 

    // 【彻底抛弃整数周期截断法！】
    // 既然我们加了汉宁窗，就再也不用去痛苦地算 actual_points 了。
    // 直接把整个 200 个点的 buffer 全部扔进去算，汉宁窗会自动磨平边缘！
    
    float u1 = get_single_freq_mag(buffer, length, fs, g_data.freq_Hz);
    float u2 = get_single_freq_mag(buffer, length, fs, g_data.freq_Hz * 2.0f);
    float u3 = get_single_freq_mag(buffer, length, fs, g_data.freq_Hz * 3.0f);
    float u4 = get_single_freq_mag(buffer, length, fs, g_data.freq_Hz * 4.0f);
    float u5 = get_single_freq_mag(buffer, length, fs, g_data.freq_Hz * 5.0f);

    // 赋值与计算百分比
    if (u1 > 10.0f) // 避开底噪干扰
    {
        g_fft.u1_amp = u1;
        g_fft.u2_percent = (u2 / u1) * 100.0f;
        g_fft.u3_percent = (u3 / u1) * 100.0f;
        g_fft.u4_percent = (u4 / u1) * 100.0f;
        g_fft.u5_percent = (u5 / u1) * 100.0f;
        
        g_fft.thd = sqrtf(u2*u2 + u3*u3 + u4*u4 + u5*u5) / u1 * 100.0f;
        return;
    } 

CLEAR_FFT:
    g_fft.u1_amp = 0;
    g_fft.u2_percent = g_fft.u3_percent = g_fft.u4_percent = g_fft.u5_percent = 0.0f;
    g_fft.thd = 0.0f;
}