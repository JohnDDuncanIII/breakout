################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Ball.cpp \
../src/BaseApplication.cpp \
../src/Block.cpp \
../src/Breakout.cpp \
../src/HighScoreTest.cpp \
../src/HighScores.cpp \
../src/Paddle.cpp 

OBJS += \
./src/Ball.o \
./src/BaseApplication.o \
./src/Block.o \
./src/Breakout.o \
./src/HighScoreTest.o \
./src/HighScores.o \
./src/Paddle.o 

CPP_DEPS += \
./src/Ball.d \
./src/BaseApplication.d \
./src/Block.d \
./src/Breakout.d \
./src/HighScoreTest.d \
./src/HighScores.d \
./src/Paddle.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/OGRE -I/usr/local/include/cegui-0 -I/usr/local/include/OGRE/Overlay -I/usr/include/ois -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


