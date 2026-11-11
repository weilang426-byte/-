#include "main.h"
#include "stm32f1xx_hal_tim.h"

//
// Created by LWadmin on 2026/8/24.
//
void MotorTest() {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);//AIN1
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);//AIN2

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);//开启PWM 输出

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 000);//设置CCR的值
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);//读取到的是CNT的值 累计计数了多少次
    /*右轮*/

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);//开启PWM 输出
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,3000);//设置CCR的值
}