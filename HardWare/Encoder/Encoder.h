//
// Created by LWadmin on 2026/8/24.
//

#ifndef PROJECT_BALANCECAR_ENCODER_H
#define PROJECT_BALANCECAR_ENCODER_H
#include "main.h"
#include "Motor.h"

extern  int16_t CNTL,CNTR;//用来指示TIM3 TIM4的CNT的值，用全局变量记录


HAL_StatusTypeDef Encoder_Init(void);
int16_t Encoder_GetCounter(Motor motor);//选择某个电机测得CNT
float Encoder_GetSpeed(Motor motor);//选择某个电机


#endif //PROJECT_BALANCECAR_ENCODER_H
