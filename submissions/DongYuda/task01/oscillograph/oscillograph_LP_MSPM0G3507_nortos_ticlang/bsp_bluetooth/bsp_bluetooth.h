#ifndef BSP_BLUETOOTH_H
#define BSP_BLUETOOTH_H

#include "ti_msp_dl_config.h"
#include "stdint.h"
#include "hmi_display.h"

extern uint16_t save_wave_A[WAVE_WIDTH];
extern uint16_t save_wave_B[WAVE_WIDTH];
extern uint16_t save_wave_C[WAVE_WIDTH];

extern uint16_t save_wave_A_ch2[WAVE_WIDTH];
extern uint16_t save_wave_B_ch2[WAVE_WIDTH];
extern uint16_t save_wave_C_ch2[WAVE_WIDTH];

extern uint8_t save_mode_A;
extern uint8_t save_mode_B;
extern uint8_t save_mode_C;

extern volatile uint8_t system_display_mode;
extern volatile uint8_t system_auto_flag;


extern volatile uint8_t system_dual_channel_flag; // 0: 单通道, 1: 双通道
extern volatile uint8_t system_fft_source;        // 1: 分析CH1, 2: 分析CH2


void BSP_Bluetooth_Init(void);
void BSP_Bluetooth_Process(void);

#endif