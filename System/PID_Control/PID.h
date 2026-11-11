
#ifndef PROJECT_BALANCECAR_PID_H
#define PROJECT_BALANCECAR_PID_H

#include "main.h"
#include "Encoder.h"

#include <math.h>
#include <sys/types.h>

//PID变量，保存Kp Ki Kd SP设定值
typedef struct {
    float Kp;//比例
    float Ki;//积分
    float Kd;//微分
    float SP;//设定值
    float outmax;//积分限幅
    float outmin;
    float integral_max;
}PID_Base_variables;


typedef struct {//保存上一次计算得到的值

    uint64_t t_k_1;//上一次计算的时间
    float err_k_1;//上次计算得到的误差
    float err_int_k_1;//积分
    float err_dev_k_1;//微分

}PID_Compute_variables;;


void PID_test(void);
void PID_Init(PID_Base_variables *PID,float Kp,float Ki,float Kd,float comax,float comin,float intgralmax);
void PID_SetSP(PID_Base_variables *PID,float SP);
float PID_Compute(PID_Base_variables *PID,PID_Compute_variables *PID_Comp,float FB);



#endif //PROJECT_BALANCECAR_PID_H
