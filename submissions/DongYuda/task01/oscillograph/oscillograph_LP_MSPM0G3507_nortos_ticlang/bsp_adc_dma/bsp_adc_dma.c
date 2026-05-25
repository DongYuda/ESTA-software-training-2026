#include "bsp_adc_dma.h"
#include "app_measure.h"

volatile uint8_t dma_transfer_done_flag = 0;
uint16_t adc_buffer[WAVE_WIDTH];
uint16_t adc_buffer_ch2[WAVE_WIDTH]; // 新增：CH2 数据阵列

// =========================================================
// 16档位 1-2-5 序列的定时器重装载值 (32MHz主频)
// 计算公式: Load_Value = (32MHz / 目标采样率) - 1
// =========================================================
const uint32_t TIMEBASE_LUT[16] = {
    15,         // 0:  10us/div  (fs=2MHz)  -> 物理极限 2MSPS
    31,         // 1:  20us/div  (fs=1MHz)
    79,         // 2:  50us/div  (fs=400kHz)
    159,        // 3:  100us/div (fs=200kHz)
    319,        // 4:  0.2ms/div (fs=100kHz)
    799,        // 5:  0.5ms/div (fs=40kHz)
    1599,       // 6:  1ms/div   (fs=20kHz)
    3199,       // 7:  2ms/div   (fs=10kHz)
    7999,       // 8:  5ms/div   (fs=4kHz)
    15999,      // 9:  10ms/div  (fs=2kHz)
    31999,      // 10: 20ms/div  (fs=1kHz)
    79999,      // 11: 50ms/div  (fs=400Hz)
    159999,     // 12: 0.1s/div  (fs=200Hz)
    319999,     // 13: 0.2s/div  (fs=100Hz)
    799999,     // 14: 0.5s/div  (fs=40Hz)
    1599999     // 15: 1s/div    (fs=20Hz)
};

// 当前全局档位索引 (默认选 0.2ms/div，对应索引 4)
volatile int8_t current_timebase_idx = 4;


//打开通道，完成初始化
void BSP_ADC_DMA_Init(void)
{

    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, 0x40556280);//DMA寄存器
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &adc_buffer[0]);//DMA数据传入adc_buffer
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, WAVE_WIDTH);

    // CH2 (ADC1) 初始化 
    DL_DMA_setSrcAddr(DMA, DMA_CH1_CHAN_ID, 0x40558280); 
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &adc_buffer_ch2[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, WAVE_WIDTH);

    DL_TimerG_setLoadValue(TIMER_1_INST, TIMEBASE_LUT[current_timebase_idx]);
    g_data.time_gear = current_timebase_idx;

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);// 开启ADC中断

    NVIC_EnableIRQ(GPIOB_INT_IRQn);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
}
//使能ADC+DMA
void BSP_ADC_DMA_Start(void)
{
    dma_transfer_done_flag = 0; 

    // 1. 彻底停下定时器与 ADC
    DL_TimerG_stopCounter(TIMER_1_INST); 
    // 重置定时器的计数值，防止它卡死在 0 无法自拔！
    DL_TimerG_setTimerCount(TIMER_1_INST, DL_TimerG_getLoadValue(TIMER_1_INST));
    
    DL_ADC12_disableConversions(ADC12_0_INST);
    DL_DMA_disableChannel(DMA, DMA_CH0_CHAN_ID);
    DL_ADC12_disableConversions(ADC12_1_INST);   // 关闭 CH2
    DL_DMA_disableChannel(DMA, DMA_CH1_CHAN_ID); // 关闭 CH2 DMA

    // ==========================================================
    volatile uint32_t flush_dummy = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
    volatile uint32_t flush_dummy2 = DL_ADC12_getMemResult(ADC12_1_INST, DL_ADC12_MEM_IDX_0); // 排空 CH2
    (void)flush_dummy; // 强行读走并丢弃，消除编译器未使用警告

    // 清洗所有硬件标志位，保证纯净的待命状态
    DL_DMA_clearInterruptStatus(DMA, (1 << DMA_CH0_CHAN_ID));
    DL_ADC12_clearInterruptStatus(ADC12_0_INST, 0xFFFFFFFF);
    DL_DMA_clearInterruptStatus(DMA, (1 << DMA_CH1_CHAN_ID));
    DL_ADC12_clearInterruptStatus(ADC12_1_INST, 0xFFFFFFFF);
    //  重新装填 DMA 弹药
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t) &adc_buffer[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, WAVE_WIDTH);
    DL_DMA_setDestAddr(DMA, DMA_CH1_CHAN_ID, (uint32_t) &adc_buffer_ch2[0]);
    DL_DMA_setTransferSize(DMA, DMA_CH1_CHAN_ID, WAVE_WIDTH);

    //  重新启动
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    DL_ADC12_enableConversions(ADC12_0_INST);
    DL_DMA_enableChannel(DMA, DMA_CH1_CHAN_ID); // 启动 CH2
    DL_ADC12_enableConversions(ADC12_1_INST);   // 启动 CH2

    // 给 ADC 硬件事件接收器上膛，等待 Timer 脉冲
    DL_ADC12_startConversion(ADC12_0_INST); 
    DL_ADC12_startConversion(ADC12_1_INST);
}
// 改变定时器重装载值
void BSP_ADC_SetTimebase(uint8_t idx)
{
    if (idx > 15) return;
    current_timebase_idx = idx;
    DL_TimerG_setLoadValue(TIMER_1_INST, TIMEBASE_LUT[current_timebase_idx]);
    g_data.time_gear = current_timebase_idx;
}

void GROUP1_IRQHandler(void) 
{
    // --------------------------------------------------------
    // 1. 处理 GPIOB 
    // 使用 0xFFFFFFFF 读取全部 32 个引脚的状态！
    // --------------------------------------------------------
    uint32_t gpioB_status = DL_GPIO_getEnabledInterruptStatus(GPIOB, 0xFFFFFFFF); 
    
    // 如果是触发电平中断 (PB9)
    if (gpioB_status & GPIO_TRIGGER_LEVEL_PIN_DETECTION_PIN) 
    {
        DL_GPIO_disableInterrupt(GPIO_TRIGGER_LEVEL_PORT, GPIO_TRIGGER_LEVEL_PIN_DETECTION_PIN);
        DL_TimerG_startCounter(TIMER_1_INST);
        // 手动清除标志
        DL_GPIO_clearInterruptStatus(GPIOB, GPIO_TRIGGER_LEVEL_PIN_DETECTION_PIN);
    }
    
    // 如果是旋转编码器 A 相下降沿 (PB17)
    if (gpioB_status & GPIO_ENCODER_PIN_TEST_A_PIN) 
    {
        delay_cycles(32000); 
        if (DL_GPIO_readPins(GPIO_ENCODER_PORT, GPIO_ENCODER_PIN_TEST_A_PIN) == 0) 
        {
            bool is_B_high = DL_GPIO_readPins(GPIO_ENCODER_PORT, GPIO_ENCODER_PIN_TEST_B_PIN);
            if (is_B_high) {
                if (current_timebase_idx < 15) current_timebase_idx++;
            } else {
                if (current_timebase_idx > 0) current_timebase_idx--;
            }
            DL_TimerG_setLoadValue(TIMER_1_INST, TIMEBASE_LUT[current_timebase_idx]);
            g_data.time_gear = current_timebase_idx;
        }
        // 手动清除标志
        DL_GPIO_clearInterruptStatus(GPIOB, GPIO_ENCODER_PIN_TEST_A_PIN);
    }
}

void GROUP0_IRQHandler(void)
{
    
}
// ADC_0中断函数 (1通道)
void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST))
    {   
        case DL_ADC12_IIDX_DMA_DONE:
            // 200个点采完，立刻关掉定时器，防止误触发
            DL_TimerG_stopCounter(TIMER_1_INST); 
            DL_ADC12_disableConversions(ADC12_0_INST); 
            DL_ADC12_disableConversions(ADC12_1_INST);
            dma_transfer_done_flag = 1; 
            break;
        default:
            break;
    }
}