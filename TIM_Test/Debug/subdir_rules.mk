################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-76198913: ../TIM_Test.sysconfig
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"D:/Softwares/ccs1220/ccs/utils/sysconfig_1.16.1/sysconfig_cli.bat" --script "D:/GitHub/TI_Projects/TIM_Test/TIM_Test.sysconfig" -o "syscfg" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/error.h: build-76198913 ../TIM_Test.sysconfig
syscfg/: build-76198913

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/Softwares/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="D:/Softwares/ccs1220/ccs/ccs_base/arm/include" --include_path="D:/Softwares/ccs1220/ccs/ccs_base/arm/include/CMSIS" --include_path="D:/GitHub/TI_Projects/TIM_Test" --include_path="D:/Softwares/ccs1220/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --advice:power=all --define=__MSP432P401R__ --define=ccs -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="D:/GitHub/TI_Projects/TIM_Test/Debug/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


