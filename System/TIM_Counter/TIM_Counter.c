//
// Created by LWadmin on 2026/8/27.
#include "main.h"
#include "TIM_Counter.h"

extern uint32_t counter;

/**
 *
 * @retval 测试程序，测试定时器是否1秒递增1次
 * @return 返回counter的值
 */
uint32_t TIM_GetCounter(void) {

    return counter;
}

/**
 * @retval 初始化TIM
 */
void TIM1_Init(void) {
    HAL_TIM_Base_Start_IT(&htim1); //开启计数！ 不要忘记
}