/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "arm_math.h"
#include "bsp_adc_dma.h"
#include "hmi_display.h"
#include "bsp_bluetooth.h"
#include "bsp_trigger.h"
#include "app_measure.h"
#include "bsp_voltage_gear.h"
#include "app_fft.h"
#include "app_auto.h" 

extern const uint32_t TIMEBASE_LUT[16];
extern volatile int8_t current_timebase_idx;
extern float target_volt;

MeasureData_t g_data = {0};

void APP_MapDataToScreen(void)
{
    uint16_t *src_ch1 = adc_buffer;
    uint16_t *src_ch2 = adc_buffer_ch2;
    uint8_t render_dual_mode = system_dual_channel_flag;

    if (system_display_mode == 1) {
        src_ch1 = save_wave_A; src_ch2 = save_wave_A_ch2; render_dual_mode = save_mode_A;
    } else if (system_display_mode == 2) {
        src_ch1 = save_wave_B; src_ch2 = save_wave_B_ch2; render_dual_mode = save_mode_B;
    } else if (system_display_mode == 3) {
        src_ch1 = save_wave_C; src_ch2 = save_wave_C_ch2; render_dual_mode = save_mode_C;
    }

    static uint8_t currently_rendered_mode = 0; 
    if (currently_rendered_mode != render_dual_mode) {
        need_full_clear_flag = 1;
        currently_rendered_mode = render_dual_mode;
    }

    // =========================================================================
    // 剥离档位倍率和校准系数，求出 ADC 引脚上真实的 DC 偏置电压 (0~3.3V)
    float pin_offset_ch1 = g_data.avg_V / (VOLT_GEAR_SPAN[g_data.volt_gear] / 3.3f) / vpp_cal_factor;
    
    // 将电压映射回 0~4095 的数字量，算出真正的波形中心点
    int32_t center_ch1 = 2048 + (int32_t)(pin_offset_ch1 * (4095.0f / 3.3f));
    
    int32_t center_ch2 = 2048; 
    // =========================================================================

    for(int i = 0; i < WAVE_WIDTH; i++)
    {
        // 减去各自真实的波形中心点，提取出纯交流振幅，实现 UI 物理居中
        int32_t val1 = (int32_t)src_ch1[i] - center_ch1;
        int32_t val2 = (int32_t)src_ch2[i] - center_ch2;

        if (render_dual_mode == 1) 
        {
            // 双通道模式
            // 上半屏中心基准点为 63
            int32_t y1 = 63 - (val1 >> 4); 
            if(y1 < WAVE_START_Y) y1 = WAVE_START_Y; // 硬件限幅防崩溃
            if(y1 > 127) y1 = 127;
            new_wave_ch1[i] = y1;

            // 下半屏中心基准点为 191
            int32_t y2 = 191 - (val2 >> 4); 
            if(y2 < 128) y2 = 128;
            if(y2 > 255) y2 = 255;
            new_wave_ch2[i] = y2;
        }
        else 
        {
            // 单通道全屏模式：
            // 全屏中心基准点为 127
            int32_t y1 = 127 - (val1 >> 3); 
            if(y1 < WAVE_START_Y) y1 = WAVE_START_Y;
            if(y1 > 255) y1 = 255;
            new_wave_ch1[i] = y1;
        }
    }
}
int main(void)
{
        SYSCFG_DL_init();
        HMI_Init();
        HMI_DrawGrid();
        BSP_Trigger_Init();
        BSP_VoltGear_Init();
        HMI_parameter_display();
        BSP_ADC_DMA_Init();
        BSP_Bluetooth_Init();

        DL_TimerG_startCounter(PWM_0_INST);  
        DL_TimerG_startCounter(PWM_1_INST);
        __enable_irq();
        BSP_ADC_DMA_Start();

        DL_TimerG_startCounter(TIMER_1_INST);

        static uint8_t last_cal_btn_state = 0;
        static uint8_t last_dual_flag_state = 0;

        while (1) 
        {
            BSP_Bluetooth_Process();
            BSP_VoltGear_Scan();

            uint8_t current_cal_btn_state = (DL_GPIO_readPins(GPIOA, GPIO_CORRECT_PIN_BUTTON_TEST_PIN) != 0) ? 1 : 0;
            
            static uint8_t is_calibrating = 0; 
            
            if (last_cal_btn_state == 1 && current_cal_btn_state == 0)
            {
                delay_cycles(160000); 

                if (DL_GPIO_readPins(GPIOA, GPIO_CORRECT_PIN_BUTTON_TEST_PIN) == 0)
                {
                    if (is_calibrating == 0) 
                    {
                        // ----- 第一次按下：开启校准模式 -----
                        is_calibrating = 1;
                        
                        // 切换模拟开关，把校准方波接入 ADC 通道 (A置高，B置低)
                        DL_GPIO_setPins(GPIOB, GPIO_CORRECT_PIN_OUTPUT_A_PIN);
                        DL_GPIO_clearPins(GPIOA, GPIO_CORRECT_PIN_OUTPUT_B_PIN);
                        
                        // 可选：此时可以在屏幕上打个标记，告诉用户进入了校准模式
                        // ILI9341_draw_string(2, 2, "CAL MODE ON ", RED); 
                    } 
                    else 
                    {
                        // ----- 第二次按下：确认并退出校准模式 -----
                        
                        // 1. 执行计算！此时屏幕上的波形已经稳定了很久，数据是最准的
                        APP_Measure_DoCalibration(); 
                        
                        // 2. 状态重置
                        is_calibrating = 0;
                        
                        // 3. 切断模拟开关，恢复普通测量 (A置低，B置低)
                        DL_GPIO_clearPins(GPIOB, GPIO_CORRECT_PIN_OUTPUT_A_PIN);
                        DL_GPIO_clearPins(GPIOA, GPIO_CORRECT_PIN_OUTPUT_B_PIN);
                        
                        // 4. 刷新屏幕参数 (让刚算出来的 K 值上屏)
                        HMI_parameter_display();
                    }
                    
                    // 3. 等待按键抬起 (防止一直按着导致疯狂在两个状态之间横跳)
                    while(DL_GPIO_readPins(GPIOA, GPIO_CORRECT_PIN_BUTTON_TEST_PIN) == 0) {
                        // 也可以在这里加一点极短的延时防看门狗
                    }
                }
            }
            last_cal_btn_state = current_cal_btn_state; // 记录本次状态

            if (system_dual_channel_flag != last_dual_flag_state) 
            {
                need_full_clear_flag = 1;
                last_dual_flag_state = system_dual_channel_flag;
            }

            //继电器部分
            if(DL_GPIO_readPins(GPIO_RELAY_PORT, GPIO_RELAY_PIN_RELAY_INPUT_A_PIN) == 0)
            {
                DL_GPIO_setPins(GPIO_RELAY_PORT, GPIO_RELAY_PIN_RELAY_OUTPUT_PIN);
            }
            else if(DL_GPIO_readPins(GPIO_RELAY_PORT, GPIO_RELAY_PIN_RELAY_INPUT_B_PIN) == 0)
            {
                DL_GPIO_clearPins(GPIO_RELAY_PORT, GPIO_RELAY_PIN_RELAY_OUTPUT_PIN);
            }

            // 处理蓝牙 AUTO 指令
            if (system_auto_flag == 1)
            {
                system_auto_flag = 0; 
                APP_AUTO_Execute();       
                HMI_parameter_display();  
            }

            if (dma_transfer_done_flag == 1)
            {
                dma_transfer_done_flag = 0; 
                uint8_t INVERT_GEAR_INDEX = 0; //  1V/div 对应的实际档位索引
                
                if (g_data.volt_gear == INVERT_GEAR_INDEX) 
                {
                    for(int i = 0; i < WAVE_WIDTH; i++) {
                        // 以 1.65V (2048) 为中轴进行绝对翻转
                        adc_buffer[i] = 4095 - adc_buffer[i];
                    }
                }
                
                // CH2 也是同样的硬件架构，保持同步补偿
                if (system_dual_channel_flag == 1 && g_data.volt_gear_ch2 == INVERT_GEAR_INDEX) 
                {
                    for(int i = 0; i < WAVE_WIDTH; i++) {
                        adc_buffer_ch2[i] = 4095 - adc_buffer_ch2[i];
                    }
                }

                // 映射双通道数据到物理屏幕
                APP_MapDataToScreen();
            
                HMI_UpdateWaveform();     

                if (system_display_mode == 0) 
                {
                    APP_Measure_Calculate(adc_buffer, WAVE_WIDTH);
                
                    if (system_dual_channel_flag == 1) {
                        APP_Measure_Calculate_CH2(adc_buffer_ch2, WAVE_WIDTH);
                        APP_Measure_Calculate_Phase(adc_buffer, adc_buffer_ch2, WAVE_WIDTH);
                    }   
                    else 
                    {
                        g_data.freq_Hz_ch2 = 0; g_data.vpp_V_ch2 = 0; g_data.phase_diff = 0;
                    }

                    if (system_fft_source == 1) 
                    {
                        APP_FFT_Calculate(adc_buffer, WAVE_WIDTH); 
                    } 
                    else if (system_fft_source == 2 && system_dual_channel_flag == 1) 
                    {
                        APP_FFT_Calculate(adc_buffer_ch2, WAVE_WIDTH); 
                    } 
                    else   
                    {
                        g_fft.u1_amp = 0; g_fft.thd = 0;
                        g_fft.u2_percent = g_fft.u3_percent = g_fft.u4_percent = g_fft.u5_percent = 0;
                    }
                }

            HMI_parameter_display(); 
            HMI_FFT_display();

            BSP_ADC_DMA_Start();

            DL_GPIO_clearInterruptStatus(GPIO_TRIGGER_LEVEL_PORT, GPIO_TRIGGER_LEVEL_PIN_DETECTION_PIN); 
            DL_GPIO_enableInterrupt(GPIO_TRIGGER_LEVEL_PORT, GPIO_TRIGGER_LEVEL_PIN_DETECTION_PIN);
        }
    }
}

void NMI_Handler(void)
{

    __BKPT();

}void HardFault_Handler(void)

{

    __BKPT();

}void SVC_Handler(void)

{

    __BKPT();

}void PendSV_Handler(void)

{

    __BKPT();

}void SysTick_Handler(void)

{

    __BKPT();

}void TIMG8_IRQHandler(void)

{

    __BKPT();

}void CANFD0_IRQHandler(void)

{

    __BKPT();

}void DAC0_IRQHandler(void)

{

    __BKPT();

}void SPI0_IRQHandler(void)

{

    __BKPT();

}void SPI1_IRQHandler(void)

{

    __BKPT();

}