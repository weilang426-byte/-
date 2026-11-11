
#ifndef PROJECT_BALANCECAR_MOTOR_H
#define PROJECT_BALANCECAR_MOTOR_H
#include "main.h"

#include "OLED.h"
#include "Motor_Head.h"
//电机初始化，设置占空比 设置方向
//PWM波形由TIM2输出，通道的ARR和PSC的值相同 在设置占空比之前先设置ARR和PSC的值

typedef enum {
    Motor_left = 0,//左电机
    Motor_right = 1//右电机
}Motor;

typedef enum {
    MOTOR_STOP = 0,    // 滑行/停止 (IN1=0, IN2=0)
    MOTOR_CW = 1,      // 正转 (IN1=1, IN2=0)
    MOTOR_CCW = 2,     // 反转 (IN1=0, IN2=1)
    MOTOR_BRAKE = 3    // 制动 (IN1=1, IN2=1)

}Motor_State;



/*Motor*/
extern int16_t CNTL,CNTR;//电机左右两侧CNT
extern float SpeedL,SpeedR;//电机速度








HAL_StatusTypeDef Motor_Init(void);//初始化电机，配置一下PSC和ARR ,后续用专用函数进行更改。
void Motor_TIM2_SetARR_PSC(uint16_t PSC,uint16_t ARR);//设置定时器的PSC和ARR
void Motor_EmergencyStop(void);
void Motor_SetPWM(Motor motor,float PWM);//设置电机的CCR，结合上个函数计算占空比
void Motor_SetRotation(Motor motor,Motor_State state);//设置电机正转反转



#endif //PROJECT_BALANCECAR_MOTOR_H
