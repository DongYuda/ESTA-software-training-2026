################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Screen/%.o: ../Screen/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/Users/ASUS/workspace_ccstheia/PWM_LP_MSPM0G3507_nortos_ticlang/Screen" -I"C:/Users/ASUS/workspace_ccstheia/PWM_LP_MSPM0G3507_nortos_ticlang" -I"C:/Users/ASUS/workspace_ccstheia/PWM_LP_MSPM0G3507_nortos_ticlang/Debug" -I"D:/mspm0_sdk_2_09_00_01/source/third_party/CMSIS/Core/Include" -I"D:/mspm0_sdk_2_09_00_01/source" -gdwarf-3 -MMD -MP -MF"Screen/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


