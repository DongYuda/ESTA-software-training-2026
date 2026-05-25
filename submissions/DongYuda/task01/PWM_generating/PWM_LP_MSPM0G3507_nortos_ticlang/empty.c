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
#include "ili9341_driver.h"
#include "font.h"
#include "stdlib.h"

#define ADC_times 100

volatile float target_duty = 50.0f;
volatile float current_duty = 0.0f;
volatile float current_voltage = 0.0f;
volatile uint32_t ADC_sum = 0;
volatile uint32_t ADC_count = 0;
volatile char rx_buffer[20];
uint8_t rx_index = 0;
//  PID 变量
//===================================================================

volatile float Kp = 0.1f;
volatile float Ki = 0.1f;
volatile float Kd = 0.0f;
float error  =  0.0f;
float output  = 0.0f;
float integral = 0.0f;
float last_error = 0.0f;
float last_dac_output = 0.0f;
float real_val = 0.0f;

//===================================================================

void PID_control(void)
{
    //============================Kp=================================
    error =  target_duty - current_duty;

    //============================Ki=================================  
    integral += error;
    if(integral > 100000.0f) integral = 100000.0f;
    if(integral < -100000.0f) integral = -100000.0f;

    //============================Kd=================================
    float derivative = error - last_error;
    last_error = error;
    output = (Kp * error) + (Ki * integral) + (Kd * derivative);
    output = 100 - output;

    if(output > 100.0f)
    {
        output = 100.0f;
    }
    if(output < 0.0f)
    {
        output = 0.0f;
    }
    real_val = output/100.0f * 3.3 ;
    uint32_t dac_value = (uint32_t)((output / 100.0f) * 4095);
    //uint32_t dac_value = 4095 * ( (100 - target_duty) / 100);
    DL_DAC12_output12(DAC0, dac_value);

}

void Screen_display(void)
{
    static bool labels_drawn = false;
    if (!labels_drawn) {
        ILI9341_draw_string(10, 20,  "Target Duty:        %", WHITE);
        ILI9341_draw_string(10, 50,  "Measure Vol:        V", WHITE);
        ILI9341_draw_string(10, 80,  "Current Duty:       %", WHITE);
        labels_drawn = true;
    }

    // 实时更新数值部分
    // 参数含义：X坐标, Y坐标, 浮点数值, 颜色
    // 注意：坐标位置需要微调以对齐标签，库中函数已封装好 sprintf 逻辑
    
    // 1. 显示当前设定的占空比 (target_duty)
    ILI9341_float_3_display(110, 20, target_duty, GRED); // 黄色显示

    // 2. 显示采集得到的直流电压值 (current_voltage)
    ILI9341_float_3_display(110, 50, current_voltage, GREEN); // 绿色显示

    // 3. 显示计算得到的实际对应占空比值 (current_duty)
    ILI9341_float_3_display(110, 80, current_duty, GBLUE); // 浅蓝色显示
}



void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerA_getPendingInterrupt(TIMER_0_INST))
    {
        case DL_TIMER_IIDX_ZERO:
            PID_control();

            break;
        default :
            break;

    }
}

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST))
    {
        case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            ADC_sum += DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
            ADC_count++;

            if (ADC_count >= ADC_times) 
            {
                current_duty = ((float)ADC_sum / (ADC_times * 4095.0f)) * 100.0f;
                current_voltage = (current_duty / 100.0f) * 3.3f;
                
                ADC_sum = 0;
                ADC_count = 0;
            }
            break;
        default :
            break;
    }
}
void UART_0_INST_IRQHandler(void)
{
    switch(DL_UART_Main_getPendingInterrupt(UART_0_INST))
    {
        case DL_UART_MAIN_IIDX_RX:
            uint8_t data = DL_UART_Main_receiveData(UART_0_INST);
            if (data == '\n' || data == '\r') 
            {
                if (rx_index > 0) 
                {
                    rx_buffer[rx_index] = '\0';
                    float temp_val = atof(rx_buffer);
                    if (temp_val >= 0.0f && temp_val <= 100.0f) 
                    {
                        target_duty = temp_val;
                    }
                    rx_index = 0;
                }
            } 
            else 
            {
                if (rx_index < 19) {
                    rx_buffer[rx_index++] = (char)data;
                }
            }
            
            break;
        default:
            break;

    } 
}

int main(void)
{
    SYSCFG_DL_init();

    ILI9341_init(BLACK);

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    DL_ADC12_enableConversions(ADC12_0_INST);

    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    DL_TimerA_startCounter(TIMER_0_INST);

    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    DL_DAC12_enable(DAC0);
    
    while (1)
    {  
        DL_ADC12_startConversion(ADC12_0_INST);
        Screen_display();
    }
}
