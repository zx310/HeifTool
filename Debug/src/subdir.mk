################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/heif_encoder_dummy.cc 

CPP_SRCS += \
../src/heif_mux.cpp 

CC_DEPS += \
./src/heif_encoder_dummy.d 

CPP_DEPS += \
./src/heif_mux.d 

OBJS += \
./src/heif_encoder_dummy.o \
./src/heif_mux.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cc src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/zhouxin/eclipse-workspace/heif_enc_x86/src" -I"/home/zhouxin/eclipse-workspace/heif_enc_x86/third/heif/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/zhouxin/eclipse-workspace/heif_enc_x86/src" -I"/home/zhouxin/eclipse-workspace/heif_enc_x86/third/heif/include" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/heif_encoder_dummy.d ./src/heif_encoder_dummy.o ./src/heif_mux.d ./src/heif_mux.o

.PHONY: clean-src

