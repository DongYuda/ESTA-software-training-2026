/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM_0 */
#define PWM_0_INST                                                         TIMG0
#define PWM_0_INST_IRQHandler                                   TIMG0_IRQHandler
#define PWM_0_INST_INT_IRQN                                     (TIMG0_INT_IRQn)
#define PWM_0_INST_CLK_FREQ                                             32000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_0_C0_PORT                                                 GPIOA
#define GPIO_PWM_0_C0_PIN                                         DL_GPIO_PIN_12
#define GPIO_PWM_0_C0_IOMUX                                      (IOMUX_PINCM34)
#define GPIO_PWM_0_C0_IOMUX_FUNC                     IOMUX_PINCM34_PF_TIMG0_CCP0
#define GPIO_PWM_0_C0_IDX                                    DL_TIMER_CC_0_INDEX

/* Defines for PWM_1 */
#define PWM_1_INST                                                         TIMA0
#define PWM_1_INST_IRQHandler                                   TIMA0_IRQHandler
#define PWM_1_INST_INT_IRQN                                     (TIMA0_INT_IRQn)
#define PWM_1_INST_CLK_FREQ                                             32000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_1_C0_PORT                                                 GPIOA
#define GPIO_PWM_1_C0_PIN                                          DL_GPIO_PIN_8
#define GPIO_PWM_1_C0_IOMUX                                      (IOMUX_PINCM19)
#define GPIO_PWM_1_C0_IOMUX_FUNC                     IOMUX_PINCM19_PF_TIMA0_CCP0
#define GPIO_PWM_1_C0_IDX                                    DL_TIMER_CC_0_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG6)
#define TIMER_0_INST_IRQHandler                                 TIMG6_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG6_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                         (31999U)
/* Defines for TIMER_1 */
#define TIMER_1_INST                                                     (TIMA1)
#define TIMER_1_INST_IRQHandler                                 TIMA1_IRQHandler
#define TIMER_1_INST_INT_IRQN                                   (TIMA1_INT_IRQn)
#define TIMER_1_INST_LOAD_VALUE                                            (15U)
#define TIMER_1_INST_PUB_0_CH                                                (1)
#define TIMER_1_INST_PUB_1_CH                                                (2)



/* Defines for UART_BT */
#define UART_BT_INST                                                       UART3
#define UART_BT_INST_FREQUENCY                                          32000000
#define UART_BT_INST_IRQHandler                                 UART3_IRQHandler
#define UART_BT_INST_INT_IRQN                                     UART3_INT_IRQn
#define GPIO_UART_BT_RX_PORT                                               GPIOA
#define GPIO_UART_BT_TX_PORT                                               GPIOB
#define GPIO_UART_BT_RX_PIN                                       DL_GPIO_PIN_25
#define GPIO_UART_BT_TX_PIN                                       DL_GPIO_PIN_12
#define GPIO_UART_BT_IOMUX_RX                                    (IOMUX_PINCM55)
#define GPIO_UART_BT_IOMUX_TX                                    (IOMUX_PINCM29)
#define GPIO_UART_BT_IOMUX_RX_FUNC                     IOMUX_PINCM55_PF_UART3_RX
#define GPIO_UART_BT_IOMUX_TX_FUNC                     IOMUX_PINCM29_PF_UART3_TX
#define UART_BT_BAUD_RATE                                                 (9600)
#define UART_BT_IBRD_32_MHZ_9600_BAUD                                      (208)
#define UART_BT_FBRD_32_MHZ_9600_BAUD                                       (21)





/* Defines for ADC12_0 */
#define ADC12_0_INST                                                        ADC0
#define ADC12_0_INST_IRQHandler                                  ADC0_IRQHandler
#define ADC12_0_INST_INT_IRQN                                    (ADC0_INT_IRQn)
#define ADC12_0_ADCMEM_0                                      DL_ADC12_MEM_IDX_0
#define ADC12_0_ADCMEM_0_REF                     DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC12_0_ADCMEM_0_REF_VOLTAGE_V                                       3.3
#define ADC12_0_INST_SUB_CH                                                  (1)
#define GPIO_ADC12_0_C6_PORT                                               GPIOB
#define GPIO_ADC12_0_C6_PIN                                       DL_GPIO_PIN_20
#define GPIO_ADC12_0_IOMUX_C6                                    (IOMUX_PINCM48)
#define GPIO_ADC12_0_IOMUX_C6_FUNC                (IOMUX_PINCM48_PF_UNCONNECTED)

/* Defines for ADC12_1 */
#define ADC12_1_INST                                                        ADC1
#define ADC12_1_INST_IRQHandler                                  ADC1_IRQHandler
#define ADC12_1_INST_INT_IRQN                                    (ADC1_INT_IRQn)
#define ADC12_1_ADCMEM_0                                      DL_ADC12_MEM_IDX_0
#define ADC12_1_ADCMEM_0_REF                     DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC12_1_ADCMEM_0_REF_VOLTAGE_V                                       3.3
#define ADC12_1_INST_SUB_CH                                                  (2)
#define GPIO_ADC12_1_C6_PORT                                               GPIOB
#define GPIO_ADC12_1_C6_PIN                                       DL_GPIO_PIN_19
#define GPIO_ADC12_1_IOMUX_C6                                    (IOMUX_PINCM45)
#define GPIO_ADC12_1_IOMUX_C6_FUNC                (IOMUX_PINCM45_PF_UNCONNECTED)



/* Defines for DMA_CH0 */
#define DMA_CH0_CHAN_ID                                                      (1)
#define ADC12_0_INST_DMA_TRIGGER                      (DMA_ADC0_EVT_GEN_BD_TRIG)
/* Defines for DMA_CH1 */
#define DMA_CH1_CHAN_ID                                                      (0)
#define ADC12_1_INST_DMA_TRIGGER                      (DMA_ADC1_EVT_GEN_BD_TRIG)


/* Port definition for Pin Group GPIO_TRIGGER_LEVEL */
#define GPIO_TRIGGER_LEVEL_PORT                                          (GPIOB)

/* Defines for PIN_DETECTION: GPIOB.9 with pinCMx 26 on package pin 61 */
// groups represented: ["GPIO_ENCODER","GPIO_TRIGGER_LEVEL"]
// pins affected: ["PIN_TEST_A","PIN_DETECTION"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define GPIO_TRIGGER_LEVEL_PIN_DETECTION_IIDX                (DL_GPIO_IIDX_DIO9)
#define GPIO_TRIGGER_LEVEL_PIN_DETECTION_PIN                     (DL_GPIO_PIN_9)
#define GPIO_TRIGGER_LEVEL_PIN_DETECTION_IOMUX                   (IOMUX_PINCM26)
/* Defines for ILI9341_CS: GPIOB.3 with pinCMx 16 on package pin 51 */
#define GPIO_ILI9341_CS_PORT                                             (GPIOB)
#define GPIO_ILI9341_CS_PIN                                      (DL_GPIO_PIN_3)
#define GPIO_ILI9341_CS_IOMUX                                    (IOMUX_PINCM16)
/* Defines for ILI9341_RST: GPIOB.2 with pinCMx 15 on package pin 50 */
#define GPIO_ILI9341_RST_PORT                                            (GPIOB)
#define GPIO_ILI9341_RST_PIN                                     (DL_GPIO_PIN_2)
#define GPIO_ILI9341_RST_IOMUX                                   (IOMUX_PINCM15)
/* Defines for ILI9341_RS: GPIOA.17 with pinCMx 39 on package pin 10 */
#define GPIO_ILI9341_RS_PORT                                             (GPIOA)
#define GPIO_ILI9341_RS_PIN                                     (DL_GPIO_PIN_17)
#define GPIO_ILI9341_RS_IOMUX                                    (IOMUX_PINCM39)
/* Defines for ILI9341_SDI: GPIOA.24 with pinCMx 54 on package pin 25 */
#define GPIO_ILI9341_SDI_PORT                                            (GPIOA)
#define GPIO_ILI9341_SDI_PIN                                    (DL_GPIO_PIN_24)
#define GPIO_ILI9341_SDI_IOMUX                                   (IOMUX_PINCM54)
/* Defines for ILI9341_SCK: GPIOA.18 with pinCMx 40 on package pin 11 */
#define GPIO_ILI9341_SCK_PORT                                            (GPIOA)
#define GPIO_ILI9341_SCK_PIN                                    (DL_GPIO_PIN_18)
#define GPIO_ILI9341_SCK_IOMUX                                   (IOMUX_PINCM40)
/* Defines for ILI9341_LED: GPIOB.18 with pinCMx 44 on package pin 15 */
#define GPIO_ILI9341_LED_PORT                                            (GPIOB)
#define GPIO_ILI9341_LED_PIN                                    (DL_GPIO_PIN_18)
#define GPIO_ILI9341_LED_IOMUX                                   (IOMUX_PINCM44)
/* Defines for ILI9341_SDO: GPIOA.22 with pinCMx 47 on package pin 18 */
#define GPIO_ILI9341_SDO_PORT                                            (GPIOA)
#define GPIO_ILI9341_SDO_PIN                                    (DL_GPIO_PIN_22)
#define GPIO_ILI9341_SDO_IOMUX                                   (IOMUX_PINCM47)
/* Port definition for Pin Group GPIO_ENCODER */
#define GPIO_ENCODER_PORT                                                (GPIOB)

/* Defines for PIN_TEST_A: GPIOB.17 with pinCMx 43 on package pin 14 */
#define GPIO_ENCODER_PIN_TEST_A_IIDX                        (DL_GPIO_IIDX_DIO17)
#define GPIO_ENCODER_PIN_TEST_A_PIN                             (DL_GPIO_PIN_17)
#define GPIO_ENCODER_PIN_TEST_A_IOMUX                            (IOMUX_PINCM43)
/* Defines for PIN_TEST_B: GPIOB.15 with pinCMx 32 on package pin 3 */
#define GPIO_ENCODER_PIN_TEST_B_PIN                             (DL_GPIO_PIN_15)
#define GPIO_ENCODER_PIN_TEST_B_IOMUX                            (IOMUX_PINCM32)
/* Port definition for Pin Group GPIO_RESPONSIVENESS */
#define GPIO_RESPONSIVENESS_PORT                                         (GPIOB)

/* Defines for PIN_BUTTON_TEST_A: GPIOB.6 with pinCMx 23 on package pin 58 */
#define GPIO_RESPONSIVENESS_PIN_BUTTON_TEST_A_PIN                (DL_GPIO_PIN_6)
#define GPIO_RESPONSIVENESS_PIN_BUTTON_TEST_A_IOMUX              (IOMUX_PINCM23)
/* Defines for PIN_BUTTON_TEST_B: GPIOB.7 with pinCMx 24 on package pin 59 */
#define GPIO_RESPONSIVENESS_PIN_BUTTON_TEST_B_PIN                (DL_GPIO_PIN_7)
#define GPIO_RESPONSIVENESS_PIN_BUTTON_TEST_B_IOMUX              (IOMUX_PINCM24)
/* Defines for PIN_BUTTON_TEST_C: GPIOB.8 with pinCMx 25 on package pin 60 */
#define GPIO_RESPONSIVENESS_PIN_BUTTON_TEST_C_PIN                (DL_GPIO_PIN_8)
#define GPIO_RESPONSIVENESS_PIN_BUTTON_TEST_C_IOMUX              (IOMUX_PINCM25)
/* Defines for PIN_CONTROL_A: GPIOB.16 with pinCMx 33 on package pin 4 */
#define GPIO_RESPONSIVENESS_PIN_CONTROL_A_PIN                   (DL_GPIO_PIN_16)
#define GPIO_RESPONSIVENESS_PIN_CONTROL_A_IOMUX                  (IOMUX_PINCM33)
/* Defines for PIN_CONTROL_B: GPIOB.0 with pinCMx 12 on package pin 47 */
#define GPIO_RESPONSIVENESS_PIN_CONTROL_B_PIN                    (DL_GPIO_PIN_0)
#define GPIO_RESPONSIVENESS_PIN_CONTROL_B_IOMUX                  (IOMUX_PINCM12)
/* Defines for PIN_BUTTON_TEST: GPIOA.13 with pinCMx 35 on package pin 6 */
#define GPIO_CORRECT_PIN_BUTTON_TEST_PORT                                (GPIOA)
#define GPIO_CORRECT_PIN_BUTTON_TEST_PIN                        (DL_GPIO_PIN_13)
#define GPIO_CORRECT_PIN_BUTTON_TEST_IOMUX                       (IOMUX_PINCM35)
/* Defines for PIN_OUTPUT_A: GPIOB.24 with pinCMx 52 on package pin 23 */
#define GPIO_CORRECT_PIN_OUTPUT_A_PORT                                   (GPIOB)
#define GPIO_CORRECT_PIN_OUTPUT_A_PIN                           (DL_GPIO_PIN_24)
#define GPIO_CORRECT_PIN_OUTPUT_A_IOMUX                          (IOMUX_PINCM52)
/* Defines for PIN_OUTPUT_B: GPIOA.26 with pinCMx 59 on package pin 30 */
#define GPIO_CORRECT_PIN_OUTPUT_B_PORT                                   (GPIOA)
#define GPIO_CORRECT_PIN_OUTPUT_B_PIN                           (DL_GPIO_PIN_26)
#define GPIO_CORRECT_PIN_OUTPUT_B_IOMUX                          (IOMUX_PINCM59)
/* Port definition for Pin Group GPIO_RELAY */
#define GPIO_RELAY_PORT                                                  (GPIOB)

/* Defines for PIN_RELAY_INPUT_A: GPIOB.4 with pinCMx 17 on package pin 52 */
#define GPIO_RELAY_PIN_RELAY_INPUT_A_PIN                         (DL_GPIO_PIN_4)
#define GPIO_RELAY_PIN_RELAY_INPUT_A_IOMUX                       (IOMUX_PINCM17)
/* Defines for PIN_RELAY_INPUT_B: GPIOB.1 with pinCMx 13 on package pin 48 */
#define GPIO_RELAY_PIN_RELAY_INPUT_B_PIN                         (DL_GPIO_PIN_1)
#define GPIO_RELAY_PIN_RELAY_INPUT_B_IOMUX                       (IOMUX_PINCM13)
/* Defines for PIN_RELAY_OUTPUT: GPIOB.13 with pinCMx 30 on package pin 1 */
#define GPIO_RELAY_PIN_RELAY_OUTPUT_PIN                         (DL_GPIO_PIN_13)
#define GPIO_RELAY_PIN_RELAY_OUTPUT_IOMUX                        (IOMUX_PINCM30)



/* Defines for DAC12 */
#define DAC12_IRQHandler                                         DAC0_IRQHandler
#define DAC12_INT_IRQN                                           (DAC0_INT_IRQn)
#define GPIO_DAC12_OUT_PORT                                                GPIOA
#define GPIO_DAC12_OUT_PIN                                        DL_GPIO_PIN_15
#define GPIO_DAC12_IOMUX_OUT                                     (IOMUX_PINCM37)
#define GPIO_DAC12_IOMUX_OUT_FUNC                   IOMUX_PINCM37_PF_UNCONNECTED


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_0_init(void);
void SYSCFG_DL_PWM_1_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_TIMER_1_init(void);
void SYSCFG_DL_UART_BT_init(void);
void SYSCFG_DL_ADC12_0_init(void);
void SYSCFG_DL_ADC12_1_init(void);
void SYSCFG_DL_DMA_init(void);

void SYSCFG_DL_DAC12_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
