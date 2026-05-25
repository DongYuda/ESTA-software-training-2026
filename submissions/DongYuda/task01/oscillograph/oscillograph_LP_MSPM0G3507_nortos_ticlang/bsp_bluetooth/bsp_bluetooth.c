#include "bsp_bluetooth.h"
#include "bsp_adc_dma.h"
#include "bsp_trigger.h"
#include "ili9341_driver.h"
#include "string.h"
#include "stdlib.h"
#include "ti_msp_dl_config.h"

uint16_t save_wave_A[WAVE_WIDTH] = {0};
uint16_t save_wave_B[WAVE_WIDTH] = {0};
uint16_t save_wave_C[WAVE_WIDTH] = {0};

uint16_t save_wave_A_ch2[WAVE_WIDTH] = {0};
uint16_t save_wave_B_ch2[WAVE_WIDTH] = {0};
uint16_t save_wave_C_ch2[WAVE_WIDTH] = {0};

uint8_t save_mode_A = 0;
uint8_t save_mode_B = 0;
uint8_t save_mode_C = 0;

volatile uint8_t system_display_mode = 0;
volatile uint8_t system_auto_flag = 0;

volatile uint8_t system_dual_channel_flag = 0; // 默认关闭双通道
volatile uint8_t system_fft_source = 1;        // 默认 FFT 计算 CH1

char bt_rx_buffer[64] = {0};        
volatile uint8_t bt_rx_idx = 0;     
volatile uint8_t bt_rx_complete = 0;// 1代表完整收到了一句话

volatile float target_volt;

void BSP_Bluetooth_Init(void)
{
    NVIC_EnableIRQ(UART_BT_INST_INT_IRQN);
}
void BSP_Bluetooth_Process(void)
{
    // 只有当标志位被中断置 1 时，才说明一串字符完整到位了
    if (bt_rx_complete == 1)
    {
        // ------------------ 纯粹的字符串比较环节 ------------------
        if (strcmp(bt_rx_buffer, "AUTO") == 0) {
            system_auto_flag = 1; 
        }
        else if (strcmp(bt_rx_buffer, "ESC") == 0) {
            system_display_mode = 0; 
        }
        else if (strcmp(bt_rx_buffer, "A") == 0) { 
            memcpy(save_wave_A, adc_buffer, sizeof(adc_buffer)); 
            memcpy(save_wave_A_ch2, adc_buffer_ch2, sizeof(adc_buffer_ch2));
            save_mode_A = system_dual_channel_flag;
        }
        else if (strcmp(bt_rx_buffer, "B") == 0) { 
            memcpy(save_wave_B, adc_buffer, sizeof(adc_buffer)); 
            memcpy(save_wave_B_ch2, adc_buffer_ch2, sizeof(adc_buffer_ch2));
            save_mode_B = system_dual_channel_flag;
        }
        else if (strcmp(bt_rx_buffer, "C") == 0) { 
            memcpy(save_wave_C, adc_buffer, sizeof(adc_buffer)); 
            memcpy(save_wave_C_ch2, adc_buffer_ch2, sizeof(adc_buffer_ch2));
            save_mode_C = system_dual_channel_flag;
        }
        else if (strcmp(bt_rx_buffer, "a") == 0) { system_display_mode = 1; }
        else if (strcmp(bt_rx_buffer, "b") == 0) { system_display_mode = 2; }
        else if (strcmp(bt_rx_buffer, "c") == 0) { system_display_mode = 3; }
        else if (strcmp(bt_rx_buffer, "OPEN_2") == 0)  { system_dual_channel_flag = 1; }
        else if (strcmp(bt_rx_buffer, "CLOSE_2") == 0) { system_dual_channel_flag = 0; }
        else if (strcmp(bt_rx_buffer, "FFT1") == 0)    { system_fft_source = 1; }
        else if (strcmp(bt_rx_buffer, "FFT2") == 0)    { system_fft_source = 2; }
        else {
            char *ptr = strstr(bt_rx_buffer, "trigger,"); // 寻找特征关键字
            
            if (ptr != NULL) 
            {
                // ptr 现在指向了 "trigger,1.92]" 这个位置
                // 我们把指针向后偏移 8 个字节（也就是跳过 "trigger," 这 8 个字符）
                // 直接让 atof 函数从 '1' 开始读取，它会自动忽略最后的 ']'
                target_volt = atof(ptr + 8); 
                
                BSP_Trigger_SetVoltage(target_volt); // 下发给 DAC 并同步 UI
            }
            // ========================================================
            // 保留纯数字发送模式
            // ========================================================
            else if ((bt_rx_buffer[0] >= '0' && bt_rx_buffer[0] <= '9') || bt_rx_buffer[0] == '.') 
            {
                float target_volt = atof(bt_rx_buffer); 
                BSP_Trigger_SetVoltage(target_volt); 
            }
        }

        // 处理完毕，清空字符串容器，重置状态，等待APP发下一句话
        memset(bt_rx_buffer, 0, sizeof(bt_rx_buffer));
        bt_rx_idx = 0;
        bt_rx_complete = 0;
    }
}
void UART3_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_BT_INST))
    {
        case DL_UART_IIDX_RX:
        {
            char rx_data = DL_UART_receiveData(UART_BT_INST);

            // ========================================================
            // 过滤包头：碰到 '[' 直接清空重来，但【坚决不保存 '['】
            // 这样无论 APP 发送 "A" 还是 "[A]"，我们提取出的开头都是 "A"
            // ========================================================
            if (rx_data == '[') 
            {
                bt_rx_idx = 0;
                bt_rx_complete = 0; 
            }
            // ========================================================
            // 识别包尾或终止符：']', 换行(\n), 回车(\r), 空格(' ')
            // 把空格加回来，完美兼容各种 APP 习惯的结尾符
            // ========================================================
            else if (rx_data == ']' || rx_data == '\n' || rx_data == '\r' || rx_data == ' ')
            {
                if (bt_rx_idx > 0)
                {
                    bt_rx_buffer[bt_rx_idx] = '\0'; // 加上C语言字符串专属封口标志
                    bt_rx_complete = 1;             // 发出暗号，通知 Process 函数去处理
                }
            }
            // ========================================================
            // 正常接收中间的纯净字符 (例如 A, B, AUTO, slider,trigger,1.92 等)
            // ========================================================
            else
            {
                if (bt_rx_idx < 63 && bt_rx_complete == 0)
                {
                    bt_rx_buffer[bt_rx_idx++] = rx_data;
                }
            }
            break;
        }
        default:
            break;
    }
}