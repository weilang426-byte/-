

#ifndef TEST_MPU6050_MAIN_MPU6050_H
#define TEST_MPU6050_MAIN_MPU6050_H

#include "main.h"

#include <sys/types.h>
#include "MPU6050_Head.h"
#include  <math.h>

#define ZERO_OFFSET_RAW 25 //陀螺仪零漂校准
#define PITCH_ACC_ZERO_OFFSET 0.65f //传感器安装误差角度
#define BALANCE_ANGLE_OFFSET 1.5f //实际平衡角度校准值
typedef struct
{
    int16_t AccX;//加速度计
    int16_t AccY;
    int16_t AccZ;

    int16_t GyroX;//陀螺仪
    int16_t GyroY;
    int16_t GyroZ;

    int16_t tempt;//温度

} MPU6050_Data_t;

typedef struct
{
    float AccX;//加速度计
    float AccY;
    float AccZ;

    float GyroX;//陀螺仪
    float GyroY;
    float GyroZ;

    float tempt;//温度

} MPU6050_Data_f;


typedef struct
{
    float roll ;
    float pitch;
    float yaw;


} MPU6050_Euler;


void MPU6050_Update(MPU6050_Data_t *mpu);



void MPU6050_Calculate(MPU6050_Data_t *mpu,MPU6050_Data_f * mpu_f);
void MPU6050_Init(void);
void MPU6050_GetEuler_g(MPU6050_Data_f *mpu_f,MPU6050_Euler* mpu_e);
void MPU6050_GetEuler_a(MPU6050_Data_f *mpu_f,MPU6050_Euler* mpu_e);
void MPU6050_GetEulerComplementary(MPU6050_Data_f *mpu_f,
                                   MPU6050_Euler *mpu_e);
float GetPitchAngle(MPU6050_Euler *mpu_e);

#endif //TEST_MPU6050_MAIN_MPU6050_H
