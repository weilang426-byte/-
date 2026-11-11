//
// Created by LWadmin on 2026/8/24.
//
#include <stdio.h>
#include <string.h>

#include "main.h"


void Test_Encoder() {
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);//读取到的是CNT的值 累计计数了多少次

    static int16_t encoder_A;
    static int16_t encoder_B;
    static char str1[50],str2[50];

    encoder_A = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);//得到CNT的值 可以转换为转速s
    encoder_B = (int16_t)__HAL_TIM_GET_COUNTER(&htim4);//得到CNT的值 可以转换为转速s

    snprintf(str1,sizeof(str1),"电机M1的速度为:%04d\r\n",encoder_A);
    snprintf(str2,sizeof(str2),"电机M2的速度为:%04d\r\n",encoder_B);

    HAL_UART_Transmit(&huart1,(uint8_t*)str1,strlen(str1),100);
    HAL_UART_Transmit(&huart1,(uint8_t*)str2,strlen(str2),100);
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);//按下速度为0
    HAL_Delay(100);
}