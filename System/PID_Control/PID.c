
#include "PID.h"




/**
 *
 * @param PID PID控制器
 * @param Kp 比例系数
 * @param Ki 积分系数
 * @param Kd 微分系数
 * @retval 初始化PID控制器，设置初始的三个参数
 */
void PID_Init(PID_Base_variables *PID,float Kp,float Ki,float Kd,float comax,float comin,float intgralmax) {
    PID->Kp = Kp;
    PID->Ki = Ki;
    PID->Kd = Kd;
    PID->SP = 0;//设定值初始化为0
    PID->outmax = comax;
    PID->outmin = comin;
    PID->integral_max = intgralmax;
}


/**
 *
 * @param PID PID控制器
 * @param SP 修改的设定值
 */
void PID_SetSP(PID_Base_variables *PID,float SP) {
    PID->SP = SP;
}


/**
 *
 * @param PID 控制器
 * @param PID_Comp 上一次计算留下来的历史数据
 * @param FB 反馈值
 * @return 返回要输出的控制值CO
 */
float PID_Compute(PID_Base_variables *PID,
                  PID_Compute_variables *PID_Comp,
                  float FB)
{
    const float OUTPUT_MAX = PID->outmax;//输出限幅上限
    const float OUTPUT_MIN = PID->outmin;//输出限幅下限
    const float INTEGRAL_OUTPUT_MAX = PID->integral_max;//I项最多只能贡献 ±30 的控制输出。这里对所有的PID都使用，速度的上限就会比较低了

    float err = PID->SP - FB;
    uint32_t now = HAL_GetTick();

    if (PID_Comp->t_k_1 == 0) {//第一次进行PID计算
        PID_Comp->t_k_1 = now;
        PID_Comp->err_k_1 = err;
        PID_Comp->err_int_k_1 = 0.0f;
        PID_Comp->err_dev_k_1 = 0.0f;
        return 0.0f;
    }

    float deltaT = (now - PID_Comp->t_k_1) * 0.001f;//计算时间间隔t，用来计算积分项和微分项

    if (deltaT <= 0.0f) {
        return 0.0f;
    }

    float err_dev =
        (err - PID_Comp->err_k_1) / deltaT;//微分项计算，计算误差变化率施加反向控制进行抑制

    float COp = PID->Kp * err;//比例项
    float COd = PID->Kd * err_dev;//微分项

    /*计算积分项*/
    float err_int = 0.0f;

    if (PID->Ki != 0.0f) {
        float err_int_candidate =
            PID_Comp->err_int_k_1
            + 0.5f * (err + PID_Comp->err_k_1) * deltaT;//积分累计计算

        //限制积分值
        float integral_limit =
            INTEGRAL_OUTPUT_MAX / fabsf(PID->Ki);

        //积分限幅
        if (err_int_candidate > integral_limit) {
            err_int_candidate = integral_limit;
        } else if (err_int_candidate < -integral_limit) {
            err_int_candidate = -integral_limit;
        }

        float CO_candidate =
            COp + PID->Ki * err_int_candidate + COd;

        float integral_direction = PID->Ki * err;

        if ((CO_candidate > OUTPUT_MAX &&
             integral_direction > 0.0f) ||
            (CO_candidate < OUTPUT_MIN &&
             integral_direction < 0.0f)) {
            err_int = PID_Comp->err_int_k_1;
             } else {
                 err_int = err_int_candidate;
             }
    }

    float COi = PID->Ki * err_int;

    float CO = COp + COi + COd;
    if (CO > OUTPUT_MAX) {
        CO = OUTPUT_MAX;
    } else if (CO < OUTPUT_MIN) {
        CO = OUTPUT_MIN;
    }
    /*保存历史数据*/
    PID_Comp->t_k_1 = now;
    PID_Comp->err_k_1 = err;
    PID_Comp->err_dev_k_1 = err_dev;
    PID_Comp->err_int_k_1 = err_int;

    return CO;
}

//平均PWM 差分PWM 左右PWM
//优先完成角度环。

//设置平均PWM和差分PWM
