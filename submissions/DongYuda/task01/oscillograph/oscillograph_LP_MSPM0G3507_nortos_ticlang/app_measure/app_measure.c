#include "app_measure.h"
#include "bsp_voltage_gear.h"
#include "stdlib.h"

float vpp_cal_factor = 1.0f;
float vpp_cal_factor_ch2 = 1.0f; // CH2的独立校准系数

const float SAMPLE_RATE_LUT[16] = {
    2000000.0f, // 0: 10us/div  (2MSPS)
    1000000.0f, // 1: 20us/div  (1MSPS)
    400000.0f,  // 2: 50us/div  (400kSPS)
    200000.0f,  // 3: 100us/div (200kSPS)
    100000.0f,  // 4: 0.2ms/div (100kSPS)
    40000.0f,   // 5: 0.5ms/div (40kSPS)
    20000.0f,   // 6: 1ms/div   (20kSPS)
    10000.0f,   // 7: 2ms/div   (10kSPS)
    4000.0f,    // 8: 5ms/div   (4kSPS)
    2000.0f,    // 9: 10ms/div  (2kSPS)
    1000.0f,    // 10: 20ms/div (1kSPS)
    400.0f,     // 11: 50ms/div (400Hz)
    200.0f,     // 12: 0.1s/div (200Hz)
    100.0f,     // 13: 0.2s/div (100Hz)
    40.0f,      // 14: 0.5s/div (40Hz)
    20.0f       // 15: 1s/div   (20Hz)
};

// 计算相关参数
void APP_Measure_Calculate(uint16_t *buffer, uint16_t length)
{
    uint16_t sort_buf[WAVE_WIDTH]; 
    uint32_t sum = 0;

    for (int i = 0; i < length; i++) 
    {
        sort_buf[i] = buffer[i];
        sum += buffer[i];
    }
    
    uint16_t avg_val = sum / length; // 算出这帧波形的平均高度 (DC基准线)

    // 调用 C 语言标准库的快速排序，O(N log N) 极速完成
    qsort(sort_buf, length, sizeof(uint16_t), cmp_uint16);

    // ---------------------------------------------------------
    // 取顶部 5% 和底部 5% 均值，抹平尖峰毛刺
    // ---------------------------------------------------------
    uint32_t sum_min = 0;
    uint32_t sum_max = 0;
    
    int avg_count = length / 20; // 5% 的数据量 (比如 200 个点取 10 个)
    if (avg_count == 0) avg_count = 1; // 防除零保护

    for (int i = 0; i < avg_count; i++) {
        sum_min += sort_buf[i];                // 累加最底部的点
        sum_max += sort_buf[length - 1 - i];   // 累加最顶部的点
    }

    // 算出平滑后的 ADC 上下限极值
    float smooth_min = (float)sum_min / avg_count;
    float smooth_max = (float)sum_max / avg_count;

    // ---------------------------------------------------------
    // 计算纯净的峰峰值 (Vpp) 并装载进屏幕 UI 结构体
    // ---------------------------------------------------------
    float pin_vpp = (smooth_max - smooth_min) * (ADC_VREF / ADC_RESOLUTION);

    g_data.vpp_V = pin_vpp * (VOLT_GEAR_SPAN[g_data.volt_gear] / 3.3f) * vpp_cal_factor;

    // ===== 新增：计算真实的平均电压 (DC 偏置) =====
    float smooth_mid = (smooth_max + smooth_min) / 2.0f; // 取波形数字中值，比算数平均更准
    float pin_offset_V = (smooth_mid - 2048.0f) * (3.3f / 4095.0f); // 相对 1.65V 的电压差
    g_data.avg_V = pin_offset_V * (VOLT_GEAR_SPAN[g_data.volt_gear] / 3.3f) * vpp_cal_factor;

    // ---------------------------------------------------------
    // 计算频率（多周期算频率）
    // ---------------------------------------------------------
    int cycle_count = 0;
    float first_cross_exact = -1.0f;
    float last_cross_exact = -1.0f;

    // 动态计算迟滞区间 (取平滑峰峰值的 5% 作为防抖容差)
    // 用 smooth_max 和 smooth_min 替换了旧的极值变量
    uint16_t hysteresis = (uint16_t)(smooth_max - smooth_min) / 20;
    if (hysteresis < 2) hysteresis = 2; // 保底迟滞值
    uint16_t thresh_high = avg_val + hysteresis;
    uint16_t thresh_low  = avg_val - hysteresis;

    // 初始化状态机：当前波形是在上半区(1)还是下半区(0)
    uint8_t state = (buffer[0] > avg_val) ? 1 : 0; 

    for (int i = 1; i < length; i++)
    {
        // 寻找有效的上升沿跨越
        if (state == 0 && buffer[i] > thresh_high)
        {
            state = 1; // 状态翻转为上半区
            
            // 【线性插值】：回溯寻找最近的一对跨越 avg_val 的点，计算精确的亚像素级交点
            int cross_idx = i;
            while(cross_idx > 0 && buffer[cross_idx] > avg_val) {
                cross_idx--;
            }
            // 此时 buffer[cross_idx] <= avg_val 且 buffer[cross_idx+1] > avg_val
            float y0 = (float)buffer[cross_idx];
            float y1 = (float)buffer[cross_idx + 1];
            
            // 相似三角形原理计算小数偏移量
            float fraction = ((float)avg_val - y0) / (y1 - y0);
            float exact_idx = (float)cross_idx + fraction; // 例如算出 10.35 这个精确位置

            if (first_cross_exact < 0.0f) {
                first_cross_exact = exact_idx; // 锁定第一个高精度起点
            }
            last_cross_exact = exact_idx;      // 不断刷新最后一个高精度起点
            cycle_count++;
        }
        // 寻找有效的下降沿跨越（仅为状态翻转，不计入周期数）
        else if (state == 1 && buffer[i] < thresh_low)
        {
            state = 0; // 状态翻转为下半区，准备抓捕下一个上升沿
        }
    }

    if (cycle_count > 1) 
    {
        // 使用带有小数点的总点数进行计算，精度直接拉满
        float avg_points_per_period = (last_cross_exact - first_cross_exact) / (float)(cycle_count - 1);
        g_data.freq_Hz = SAMPLE_RATE_LUT[g_data.time_gear] / avg_points_per_period;
    } 
    else 
    {
        g_data.freq_Hz = 0.0f; 
    }
}
void APP_Measure_Calculate_CH2(uint16_t *buffer, uint16_t length)
{
    //  求基本参数 (保持不变)
    uint16_t max_val = 0, min_val = 4095;
    uint32_t sum = 0;
    for (int i = 0; i < length; i++) {
        if (buffer[i] > max_val) max_val = buffer[i];
        if (buffer[i] < min_val) min_val = buffer[i];
        sum += buffer[i];
    }
    uint16_t avg_val = sum / length; 
    
    //  Vpp 抗干扰升级：剔除极值毛刺
    uint32_t top_sum = 0, bottom_sum = 0;
    int top_cnt = 0, bottom_cnt = 0;
    uint16_t top_threshold = max_val - ((max_val - min_val) / 20);
    uint16_t bottom_threshold = min_val + ((max_val - min_val) / 20);

    for (int i = 0; i < length; i++) {
        if (buffer[i] >= top_threshold) { top_sum += buffer[i]; top_cnt++; }
        if (buffer[i] <= bottom_threshold) { bottom_sum += buffer[i]; bottom_cnt++; }
    }
    float smooth_max = top_cnt > 0 ? (float)top_sum / top_cnt : max_val;
    float smooth_min = bottom_cnt > 0 ? (float)bottom_sum / bottom_cnt : min_val;

    float pin_vpp = (smooth_max - smooth_min) * (ADC_VREF / ADC_RESOLUTION);
    // 使用 CH2 自己的档位倍率和校准系数
    g_data.vpp_V_ch2 = pin_vpp * (VOLT_GEAR_SPAN[g_data.volt_gear_ch2] / 3.3f) * vpp_cal_factor_ch2;

    // 3. 终极频率算法：迟滞比较 + 线性插值 + 多周期平均
    int cycle_count = 0;
    float first_cross_exact = -1.0f;
    float last_cross_exact = -1.0f;

    uint16_t hysteresis = (max_val - min_val) / 20;
    if (hysteresis < 2) hysteresis = 2;
    uint16_t thresh_high = avg_val + hysteresis;
    uint16_t thresh_low  = avg_val - hysteresis;

    uint8_t state = (buffer[0] > avg_val) ? 1 : 0; 

    for (int i = 1; i < length; i++)
    {
        if (state == 0 && buffer[i] > thresh_high)
        {
            state = 1; 
            int cross_idx = i;
            while(cross_idx > 0 && buffer[cross_idx] > avg_val) cross_idx--;
            
            float y0 = (float)buffer[cross_idx];
            float y1 = (float)buffer[cross_idx + 1];
            float fraction = ((float)avg_val - y0) / (y1 - y0);
            float exact_idx = (float)cross_idx + fraction;

            if (first_cross_exact < 0.0f) first_cross_exact = exact_idx;
            last_cross_exact = exact_idx;
            cycle_count++;
        }
        else if (state == 1 && buffer[i] < thresh_low)
        {
            state = 0; 
        }
    }

    if (cycle_count > 1) {
        float avg_points_per_period = (last_cross_exact - first_cross_exact) / (float)(cycle_count - 1);
        g_data.freq_Hz_ch2 = SAMPLE_RATE_LUT[g_data.time_gear] / avg_points_per_period;
    } else {
        g_data.freq_Hz_ch2 = 0.0f; 
    }
}
void APP_Measure_Calculate_Phase(uint16_t *buf1, uint16_t *buf2, uint16_t length)
{
    if (g_data.freq_Hz < 10.0f) { g_data.phase_diff = 0; return; } // 无频率不计相位

    // 求各自的平均值、最大值、最小值 (用于计算各自的迟滞阈值)
    uint16_t max1 = 0, min1 = 4095; uint32_t sum1 = 0;
    uint16_t max2 = 0, min2 = 4095; uint32_t sum2 = 0;
    for(int i = 0; i < length; i++) {
        if(buf1[i] > max1) max1 = buf1[i]; if(buf1[i] < min1) min1 = buf1[i]; sum1 += buf1[i];
        if(buf2[i] > max2) max2 = buf2[i]; if(buf2[i] < min2) min2 = buf2[i]; sum2 += buf2[i];
    }
    uint16_t avg1 = sum1 / length;
    uint16_t avg2 = sum2 / length;

    uint16_t hyst1 = (max1 - min1) / 20; if(hyst1 < 2) hyst1 = 2;
    uint16_t hyst2 = (max2 - min2) / 20; if(hyst2 < 2) hyst2 = 2;

    // 寻找 CH1 的第一个精确上升沿 (亚像素级)
    float cross1_exact = -1.0f;
    uint8_t state1 = (buf1[0] > avg1) ? 1 : 0;
    for(int i = 1; i < length; i++) {
        if(state1 == 0 && buf1[i] > avg1 + hyst1) {
            int cross_idx = i;
            while(cross_idx > 0 && buf1[cross_idx] > avg1) cross_idx--;
            float y0 = buf1[cross_idx], y1 = buf1[cross_idx+1];
            cross1_exact = cross_idx + (avg1 - y0) / (y1 - y0);
            break; // 找到第一个立刻退出
        } 
        else if (state1 == 1 && buf1[i] < avg1 - hyst1) { state1 = 0; }
    }

    // 寻找 CH2 的第一个精确上升沿 (亚像素级)
    float cross2_exact = -1.0f;
    uint8_t state2 = (buf2[0] > avg2) ? 1 : 0;
    for(int i = 1; i < length; i++) {
        if(state2 == 0 && buf2[i] > avg2 + hyst2) {
            int cross_idx = i;
            while(cross_idx > 0 && buf2[cross_idx] > avg2) cross_idx--;
            float y0 = buf2[cross_idx], y1 = buf2[cross_idx+1];
            cross2_exact = cross_idx + (avg2 - y0) / (y1 - y0);
            break; // 找到第一个立刻退出
        } 
        else if (state2 == 1 && buf2[i] < avg2 - hyst2) { state2 = 0; }
    }

    // 利用两个带有小数的精确坐标，计算高精度相位差
    if(cross1_exact >= 0.0f && cross2_exact >= 0.0f) {
        // 利用刚刚测得的精确频率，算出当前频率下一个周期到底有几个点 (带小数)
        float points_per_period = SAMPLE_RATE_LUT[g_data.time_gear] / g_data.freq_Hz;
        
        // 算出两个边沿之间相差多少个点 (带小数)
        float diff = cross2_exact - cross1_exact; 
        
        // 映射到 360 度
        float phase = (diff / points_per_period) * 360.0f;
        
        // 归一化到 -180 ~ +180 之间 (防止抓到的边沿不是同一个周期里的)
        while(phase > 180.0f) phase -= 360.0f;
        while(phase < -180.0f) phase += 360.0f;
        
        g_data.phase_diff = fabsf(phase); // 取绝对值显示
    } else {
        g_data.phase_diff = 0.0f;
    }
}
void APP_Measure_DoCalibration(void)
{
    // 安全锁：防止探头悬空或者接在 GND 时误触导致系数爆炸
    if (g_data.vpp_V > 0.5f) 
    {
        // 剥离旧的补偿系数，还原出模拟前端(AFE)真实的物理测量电压
        float raw_vpp = g_data.vpp_V / vpp_cal_factor;
        
        // PA12 输出的基准方波峰峰值是 3.3V，强行计算新的补偿倍率
        vpp_cal_factor = 3.3f / raw_vpp;
    }
}
int cmp_uint16(const void *a, const void *b) {
    return (*(uint16_t*)a - *(uint16_t*)b);
}