################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/hmi_display" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_trigger" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_bluetooth" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/app_measure" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_voltage_gear" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/app_fft" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/app_auto" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/Debug" -I"D:/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"D:/mspm0_sdk_2_09_00_01/source" -I"D:/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/DSP/Include" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_adc_dma" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/handware" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1544447896: ../empty.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"D:/ccs/utils/sysconfig_1.26.0/sysconfig_cli.bat" -s "D:/mspm0_sdk_2_09_00_01/.metadata/product.json" -s "D:/mspm0_sdk_2_09_00_01/.metadata/product.json" --script "C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/empty.syscfg" -o "." --compiler ticlang
	@echo 'Finished building: "$<"'
	@echo ' '

device_linker.cmd: build-1544447896 ../empty.syscfg
device.opt: build-1544447896
device.cmd.genlibs: build-1544447896
ti_msp_dl_config.c: build-1544447896
ti_msp_dl_config.h: build-1544447896
Event.dot: build-1544447896

%.o: ./%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/hmi_display" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_trigger" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_bluetooth" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/app_measure" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_voltage_gear" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/app_fft" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/app_auto" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/Debug" -I"D:/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"D:/mspm0_sdk_2_09_00_01/source" -I"D:/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/DSP/Include" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_adc_dma" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/handware" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: D:/mspm0_sdk_2_09_00_01/source/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/hmi_display" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_trigger" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_bluetooth" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/app_measure" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_voltage_gear" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/app_fft" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/app_auto" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/Debug" -I"D:/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"D:/mspm0_sdk_2_09_00_01/source" -I"D:/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/DSP/Include" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/bsp_adc_dma" -I"C:/Users/ASUS/workspace_ccstheia/oscillograph_LP_MSPM0G3507_nortos_ticlang/handware" -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


