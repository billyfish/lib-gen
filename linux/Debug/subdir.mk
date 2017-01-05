################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../byte_buffer.c \
../document_parser.c \
../string_utils.c 

OBJS += \
./byte_buffer.o \
./document_parser.o \
./string_utils.o 

C_DEPS += \
./byte_buffer.d \
./document_parser.d \
./string_utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/billy/Projects/libgen-linux" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


