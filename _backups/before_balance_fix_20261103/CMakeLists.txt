cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Project_BalanceCar)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}

        HardWare/OLED/OLED.c
        HardWare/OLED/OLED.h
        HardWare/OLED/OLED_Data.c
        HardWare/OLED/OLED_Data.h
        Test_Program/OLED/Test_oled.h
        Test_Program/OLED/Test_oled.c
        Test_Program/BlueTooth/Test_Bluetooth.c
        Test_Program/BlueTooth/Test_Bluetooth.h
        HardWare/MPU6050/MPU6050.c
        HardWare/MPU6050/MPU6050.h
        HardWare/MPU6050/MPU6050_Head.h
        Test_Program/MPU6050/Test_MPU6050.c
        Test_Program/MPU6050/Test_MPU6050.h
        HardWare/Motor/Motor.c
        HardWare/Motor/Motor.h
        HardWare/Encoder/Encoder.c
        HardWare/Encoder/Encoder.h
        Test_Program/Test_Encoder/Test_Encoder.c
        Test_Program/Test_Encoder/Test_Encoder.h
        Test_Program/Test_Motor/Test_Motor.c
        Test_Program/Test_Motor/Test_Motor.h
        System/Tick/Tick.h
        System/Tick/Tick.c
        HardWare/Key/Key.c
        HardWare/Key/Key.h
        HardWare/LED/LED.h
        HardWare/LED/LED.c
        System/TIM_Counter/TIM_Counter.h
        System/TIM_Counter/TIM_Counter.c
        Test_Program/Test_Key/Test_Key.c
        Test_Program/Test_Key/Test_Key.h
        HardWare/Motor/Motor_Head.h
        HardWare/BlueTooth/BlueTooth.h
        HardWare/BlueTooth/BlueTooth.c
        HardWare/BlueTooth/BluTooth_Head.h
        System/PID_Control/PID.c
        System/PID_Control/PID.h
        System/SpeedLoop/SpeedLoop.c
        System/SpeedLoop/SpeedLoop.h
        System/AngleLoop/AngleLoop.c
        System/AngleLoop/AngleLoop.h
        System/SteeringLoop/SteeringLoop.c
        System/SteeringLoop/SteeringLoop.h
        Core/Src/main.c

)

target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        -u _printf_float

)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        HardWare/BlueTooth
        HardWare/Encoder
        HardWare/Key
        HardWare/MPU6050
        HardWare/Motor
        HardWare/OLED
        HardWare/LED


        Test_Program/OLED
        Test_Program/BlueTooth
        Test_Program/MPU6050
        Test_Program/Test_Encoder
        Test_Program/Test_Motor
        Test_Program/Test_Key


        System/PID_Control
        System/Tick
        System/TIM_Counter
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
