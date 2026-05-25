#include "bsp_voltage_gear.h"
#include "bsp_trigger.h"

// 定义不同档位下，ADC 满量程(0~3.3V)对应的真实电压跨度
const float VOLT_GEAR_SPAN[3] = {
    16.0f,   // 0: 1V/div   (满量程 16V)
    1.6f,    // 1: 0.1V/div (满量程 1.6V)
    0.16f    // 2: 0.01V/div(满量程 0.16V)
};

// 用于屏幕打印的档位字符串
const char* VOLT_GEAR_STRING[3] = {
    "1V/div   ", 
    "0.1V/div ", 
    "0.01V/div"
};

void BSP_VoltGear_Init(void)
{
    g_data.volt_gear = 0; // 默认初始化为 1V/div 档位
    DL_GPIO_clearPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_CONTROL_A_PIN);
    DL_GPIO_clearPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_CONTROL_B_PIN);
}

// 新增的 API 函数：专门负责操控硬件继电器/模拟开关换挡
void BSP_VoltGear_SetTarget(uint8_t target_gear)
{
    if (target_gear > 2) return;

    g_data.volt_gear = target_gear;
    switch (target_gear) 
    {
        case 0:
            DL_GPIO_clearPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_CONTROL_A_PIN);
            DL_GPIO_clearPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_CONTROL_B_PIN);
            break;
        case 1:
            DL_GPIO_setPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_CONTROL_A_PIN);
            DL_GPIO_clearPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_CONTROL_B_PIN);
            break;
        case 2:
            DL_GPIO_clearPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_CONTROL_A_PIN);
            DL_GPIO_setPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_CONTROL_B_PIN);
            break;
    }
    // 档位改变后，重新下发触发电压
    BSP_Trigger_SetVoltage(g_data.avg_V); 
}

// 扫描按键并换挡的函数，现在变得非常清爽
void BSP_VoltGear_Scan(void)
{
    bool is_gear0 = DL_GPIO_readPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_BUTTON_TEST_A_PIN);
    bool is_gear1 = DL_GPIO_readPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_BUTTON_TEST_B_PIN);
    bool is_gear2 = DL_GPIO_readPins(GPIO_RESPONSIVENESS_PORT, GPIO_RESPONSIVENESS_PIN_BUTTON_TEST_C_PIN);

    uint8_t target_gear = g_data.volt_gear;

    if (!is_gear0) target_gear = 0;      
    else if (!is_gear1) target_gear = 1; 
    else if (!is_gear2) target_gear = 2; 

    // 如果档位发生改变，调用我们封装好的 API 去执行动作
    if (target_gear != g_data.volt_gear) 
    {
        // 这里直接传变量名，绝对不能加 uint8_t
        BSP_VoltGear_SetTarget(target_gear);
        HMI_parameter_display();
    }
}