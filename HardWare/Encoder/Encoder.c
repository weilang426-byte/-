
#include "main.h"
#include "Encoder.h"

/**
 *
 * @return 返回是否成功启动编码器状态
 * @retval 开启TIM3 TIM4的编码器模式，用来测速TIM1
 */
HAL_StatusTypeDef Encoder_Init(void) {

    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1 );//开启编码器模式
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2 );//开启编码器模式
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1 );//开启编码器模式
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2 );//开启编码器模式

    return HAL_OK;
}


/**
 *
 * @param motor 选中某个电机
 * @return 返回电机的CNT 是有正负的
 * @retval 直接读取返回电机的CNT，然后清空CNT，在某个时间间隔内（20ms），相除的平均值可以看作CNT的平均变化率，用来求转速
 */
int16_t Encoder_GetCounter(Motor motor) {
    static int16_t counter;
    if (motor == Motor_left) {

        counter = __HAL_TIM_GET_COUNTER(&htim3);
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        return counter;
    }else if (motor == Motor_right) {
        counter = __HAL_TIM_GET_COUNTER(&htim4);
        __HAL_TIM_SET_COUNTER(&htim4, 0);
        return counter;
    }else return -1;
}

/**
 *
 * @param motor 选择哪个电机
 * @return 返回值为某个电机的速度,单位是rpm
 * @retval 这里的0.02是TIM1定时器读取该函数的周期，44.0是1圈44个边沿，9.2.是减速比
 */
float Encoder_GetSpeed(Motor motor) {

    if (motor == Motor_left) {
        //反转CNTL>0 正转CNTL<0，统一在前进方向时，速度大于0.所以左轮前面加个负号。
        return  (float)(CNTL /44.0 * 60 / 0.02 /9.27666);//rpm
    }
    else if (motor == Motor_right) {
        //正转 CNTL>0 反转 CNTL<0
        return  (float)(CNTR /44.0 * 60 / 0.02 /9.27666);
    }
    else return 0;
}



