#include "hmi_display.h"
#include "ili9341_driver.h"
#include "font.h"
#include "stdio.h"
#include "bsp_voltage_gear.h"
#include "app_fft.h"
#include "app_measure.h" 
#include "bsp_bluetooth.h"

uint8_t need_full_clear_flag = 0;

static uint32_t old_wave_ch1[WAVE_WIDTH] = {0}; 
static uint32_t old_wave_ch2[WAVE_WIDTH] = {0}; 
uint32_t new_wave_ch1[WAVE_WIDTH] = {0};
uint32_t new_wave_ch2[WAVE_WIDTH] = {0};

const char* GEAR_STRING[16] = {
    "10us/div", "20us/div", "50us/div", "100us/div",
    "0.2ms/div", "0.5ms/div", "1ms/div", "2ms/div",
    "5ms/div", "10ms/div", "20ms/div", "50ms/div",
    "0.1s/div", "0.2s/div", "0.5s/div", "1s/div"
};

void HMI_Init()
{
    ILI9341_init(BLACK);
}

// 绘制格子线，并加入红色警戒中轴线
void HMI_DrawGrid()
{
    uint16_t current_x, current_y;
    for(int i = 0; i <= 8; i++)
    {
        current_y = WAVE_START_Y + (i * 32);
        // 在 Y=128 的中心位置画一条红色分界线，隔离 CH1 和 CH2
        if (i == 4) ILI9341_draw_line(WAVE_START_X, current_y, WAVE_WIDTH, current_y, RED);
        else        ILI9341_draw_line(WAVE_START_X, current_y, WAVE_WIDTH, current_y, GRAYBLUE);
    }
    for(int i = 0; i <= 10; i++)
    {
        current_x = WAVE_START_X + (i * 20);
        ILI9341_draw_line(current_x, WAVE_START_Y, current_x, WAVE_HEIGHT, GRAYBLUE);
    }  
}

// 全新规划的 4 行高密度硬核仪表盘 (全浮点数打印)
void HMI_parameter_display()
{
    char buff[45]; 

    // L1 (Y=257): 全局状态 (模式、时基、触发源、触发电平)
    sprintf(buff, "DUAL %s CH1 %.2fV ", GEAR_STRING[g_data.time_gear], g_data.trigger_level_V);
    ILI9341_draw_string(2, 257, buff, WHITE);

    // L2 (Y=272): CH1 核心数据 (黄色)
    sprintf(buff, "CH1:%s %.2fkhz %.3fV", VOLT_GEAR_STRING[g_data.volt_gear], g_data.freq_Hz / 1000.0f, g_data.vpp_V);
    ILI9341_draw_string(2, 272, buff, YELLOW);

    if (system_dual_channel_flag) {
        sprintf(buff, "CH2:%s %.2fkHz %.2fV   ",VOLT_GEAR_STRING[g_data.volt_gear_ch2], g_data.freq_Hz_ch2 / 1000.0f, g_data.vpp_V_ch2);
        ILI9341_draw_string(2, 287, buff, GREEN);
        
        sprintf(buff, "Phase:%.1f Deg k1:%.2f ",g_data.phase_diff, vpp_cal_factor);
        ILI9341_draw_string(2, 302, buff, CYAN);
    } else {
        // 单通道模式下，擦除/归零这两行
        ILI9341_draw_string(2, 287, "CH2: CLOSED                    ", GRAY);
        sprintf(buff, "k1:%.2f                     ", vpp_cal_factor);
        ILI9341_draw_string(2, 302, buff, CYAN);
    }
}

// 双通道波形高速刷新算法
void HMI_UpdateWaveform(void)
{
    // ========================================================
    // 状态切换时的“暴力全刷”逻辑
    // ========================================================
    if (need_full_clear_flag) 
    {
        // 1. 暴力清屏并重绘网格
        ILI9341_fill(WAVE_START_X, WAVE_START_Y, WAVE_START_X + WAVE_WIDTH - 1, WAVE_HEIGHT - 1, BLACK);
        HMI_DrawGrid();
        
        // 2. 强制完整绘制切换后的第一帧波形，拒绝只画差分点！
        for(int i = 0; i < WAVE_WIDTH; i++) 
        { 
            uint16_t x = WAVE_START_X + i;
            uint16_t start_y, end_y;
            
            // --- 强绘 CH1 (黄色) ---
            uint16_t y1 = new_wave_ch1[i];
            uint16_t y1_prev = (i == 0) ? y1 : new_wave_ch1[i-1];
            if (y1_prev < y1) { start_y = y1_prev; end_y = y1; } else { start_y = y1; end_y = y1_prev; }
            ILI9341_Draw_Fast_VLine_With_Grid(x, start_y, end_y, 0, YELLOW);
            old_wave_ch1[i] = y1; // 同步历史记录
            
            // --- 强绘 CH2 (绿色) ---
            if (system_dual_channel_flag == 1) {
                uint16_t y2 = new_wave_ch2[i];
                uint16_t y2_prev = (i == 0) ? y2 : new_wave_ch2[i-1];
                if (y2_prev < y2) { start_y = y2_prev; end_y = y2; } else { start_y = y2; end_y = y2_prev; }
                ILI9341_Draw_Fast_VLine_With_Grid(x, start_y, end_y, 0, GREEN);
                old_wave_ch2[i] = y2; 
            }
        }
        need_full_clear_flag = 0;
        return; // 🚨 第一帧强绘完毕，直接退出！跳过下方的差分逻辑
    }

    // ========================================================
    // 日常的高速差分渲染逻辑 (保持原样)
    // ========================================================
    uint16_t x, old_y1, new_y1, old_y1_prev, new_y1_prev;
    uint16_t old_y2, new_y2, old_y2_prev, new_y2_prev;
    uint16_t start_y, end_y;

    uint16_t real_old_y1_prev = old_wave_ch1[0];
    uint16_t real_old_y2_prev = old_wave_ch2[0];

    for(int i = 0; i < WAVE_WIDTH; i++) 
    {
        x = WAVE_START_X + i;
        
        // --- 渲染 CH1 (上半屏或全屏) ---
        old_y1 = old_wave_ch1[i];  new_y1 = new_wave_ch1[i];
        if (i == 0) {
            old_y1_prev = old_y1;  new_y1_prev = new_y1;
        } else {
            old_y1_prev = real_old_y1_prev; new_y1_prev = new_wave_ch1[i-1];
        }
        real_old_y1_prev = old_y1;

        if(!(old_y1 == new_y1 && old_y1_prev == new_y1_prev)) 
        {
            if (old_y1_prev < old_y1) { start_y = old_y1_prev; end_y = old_y1; } else { start_y = old_y1; end_y = old_y1_prev; }
            ILI9341_Draw_Fast_VLine_With_Grid(x, start_y, end_y, 1, BLACK); // 擦旧

            if (new_y1_prev < new_y1) { start_y = new_y1_prev; end_y = new_y1; } else { start_y = new_y1; end_y = new_y1_prev; }
            ILI9341_Draw_Fast_VLine_With_Grid(x, start_y, end_y, 0, YELLOW); // 画新
            
            old_wave_ch1[i] = new_y1; 
        }

        // --- 渲染 CH2 (仅双通道时执行) ---
        if (system_dual_channel_flag == 1)
        {
            old_y2 = old_wave_ch2[i];  new_y2 = new_wave_ch2[i];
            if (i == 0) { old_y2_prev = old_y2; new_y2_prev = new_y2; } 
            else { old_y2_prev = real_old_y2_prev; new_y2_prev = new_wave_ch2[i-1]; }
            real_old_y2_prev = old_y2;

            if(!(old_y2 == new_y2 && old_y2_prev == new_y2_prev)) 
            {
                if (old_y2_prev < old_y2) { start_y = old_y2_prev; end_y = old_y2; } else { start_y = old_y2; end_y = old_y2_prev; }
                ILI9341_Draw_Fast_VLine_With_Grid(x, start_y, end_y, 1, BLACK);
                if (new_y2_prev < new_y2) { start_y = new_y2_prev; end_y = new_y2; } else { start_y = new_y2; end_y = new_y2_prev; }
                ILI9341_Draw_Fast_VLine_With_Grid(x, start_y, end_y, 0, GREEN);
                old_wave_ch2[i] = new_y2; 
            }
        }
    }
}


static void format_fft_percent(char* buff, float val)
{
    if (val > 999.0f) {
        sprintf(buff, "MAX ");         // 超过极限，显示 MAX
    } else if (val >= 100.0f) {
        sprintf(buff, "%3.0f", val); // 100~999，舍弃小数
    } else {
        sprintf(buff, "%4.1f", val);   // 0~99.9，保留一位小数
    }
}
// FFT 瀑布流 (恢复浮点数打印)
void HMI_FFT_display(void)
{
    char buff[10];
    uint16_t x_pos = 200; 
    
    ILI9341_draw_string(x_pos, 5, "FFT", CYAN);
    
    ILI9341_draw_string(x_pos, 25, "U1", YELLOW);
    ILI9341_draw_string(x_pos, 40, "100%", WHITE); // 基波固定100%，4个字符完美占据空间

    ILI9341_draw_string(x_pos, 60, "U2", YELLOW);
    format_fft_percent(buff, g_fft.u2_percent);
    ILI9341_draw_string(x_pos, 75, buff, WHITE);

    ILI9341_draw_string(x_pos, 95, "U3", YELLOW);
    format_fft_percent(buff, g_fft.u3_percent);
    ILI9341_draw_string(x_pos, 110, buff, WHITE);

    ILI9341_draw_string(x_pos, 130, "U4", YELLOW);
    format_fft_percent(buff, g_fft.u4_percent);
    ILI9341_draw_string(x_pos, 145, buff, WHITE);

    ILI9341_draw_string(x_pos, 165, "U5", YELLOW);
    format_fft_percent(buff, g_fft.u5_percent);
    ILI9341_draw_string(x_pos, 180, buff, WHITE);
    
    ILI9341_draw_string(x_pos, 205, "THD", BRED);
    format_fft_percent(buff, g_fft.thd); // THD 也享受同等待遇保护
    ILI9341_draw_string(x_pos, 220, buff, WHITE);
}