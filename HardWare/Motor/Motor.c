
#include "Motor.h"
#include <math.h>

/* No HAL/tick dependency: safe from a watchdog or fault handler. */
void Motor_EmergencyStop(void)
{
    TIM2->CCR1 = 0;
    TIM2->CCR2 = 0;
    TIM2->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC2E);
}

/**
 * @retval 初始化两个电机初始PWM波形,且初始的速度均为0
 */
HAL_StatusTypeDef Motor_Init(void) {
    HAL_StatusTypeDef state1 = HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_StatusTypeDef state2 = HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    Motor_SetPWM(Motor_left,0);
    Motor_SetPWM(Motor_right,0);
    return (state1 == HAL_OK && state2 == HAL_OK) ? HAL_OK : HAL_ERROR;
}


/**
 *
 * @param PSC TIM2预分频器的值
 * @param ARR TIM2自动装载重装器的值
 * @retval 设置PSC和ARR，改变分辨率
 */
void Motor_TIM2_SetARR_PSC(uint16_t PSC, uint16_t ARR) {
    __HAL_TIM_SET_PRESCALER(&htim2, PSC);
    __HAL_TIM_SET_AUTORELOAD(&htim2, ARR);

}


/**
 *
 * @param motor 选择某个电机来设置
 * @param PWM 设置某个点击的占空比和方向，正负决定方向，占空比决定转速
 */
void Motor_SetPWM(Motor motor,float PWM) {//-100 100
    if (!isfinite(PWM)) { Motor_EmergencyStop(); return; }
    if (PWM > 100.0f) PWM = 100.0f;
    else if (PWM < -100.0f) PWM = -100.0f;
    if (motor == Motor_left ) {//左电机 M1通道1

        if (PWM >0.0f) {//左轮正转
            Motor_SetRotation(Motor_left,MOTOR_CCW);//CNTL是小于0的此时
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,(int16_t)(PWM * MOTOR_CCR_MAX * 0.01));
        }else{//反转
            Motor_SetRotation(Motor_left,MOTOR_CW);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,(int16_t)(-PWM * MOTOR_CCR_MAX * 0.01));
        }
    }
    else if (motor == Motor_right) {//右电机
        if (PWM >0.0f) {
            Motor_SetRotation(Motor_right,MOTOR_CW);//CNTR是大于0的此时
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,(int16_t)(PWM * MOTOR_CCR_MAX * 0.01));
        }else{//反转
            Motor_SetRotation(Motor_right,MOTOR_CCW);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,(int16_t)(-PWM * MOTOR_CCR_MAX * 0.01));
        }
    }
    else { return; }
    if (motor == Motor_left) TIM2->CCER |= TIM_CCER_CC1E;
    else if (motor == Motor_right) TIM2->CCER |= TIM_CCER_CC2E;
}

/**
 *
 * @param motor 选择哪个电机
 * @param state 该电机的状态/方向
 */
void Motor_SetRotation(Motor motor,Motor_State state) {
    if (motor == Motor_left ) {//左电机 M1通道1

        if (state ==MOTOR_STOP) {//停车
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);//AIN1
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);//AIN2

        }else if (state ==MOTOR_CW) {//正转
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);

        }else if (state ==MOTOR_CCW) {//反转
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);

        }else {//制动
            //MOTOR_BRAKE
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
        }

    }
    else if (motor == Motor_right) {//右电机

        if (state ==MOTOR_STOP) {//停车
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);//AIN1
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);//AIN2

        }else if (state ==MOTOR_CW) {//正转
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);

        }else if (state ==MOTOR_CCW) {//反转
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);

        }else {//制动
            //MOTOR_BRAKE
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
        }
    }

}


