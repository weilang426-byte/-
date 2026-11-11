#include "MPU6050.h"
#include <stdlib.h>

/* Startup calibration replaces this fallback only after a stationary test passes. */
float mpu6050_gyro_bias_raw = ZERO_OFFSET_RAW;

HAL_StatusTypeDef MPU6050_Init(void)
{
    uint8_t value = 0x80;
    if (HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDREESS, PWR_MGMT_1,
                         I2C_MEMADD_SIZE_8BIT, &value, 1, 20) != HAL_OK) return HAL_ERROR;
    HAL_Delay(100);
    /* Preserve the existing 1 kHz, +/-2000 dps, +/-16g configuration. */
    const uint8_t registers[][2] = {
        {PWR_MGMT_1, 0x01}, {PWR_MGMT_2, 0x00}, {ACCEL_ADDREESS, 0x18},
        {GYRO_ADDREESS, 0x18}, {MPU6050_SMPLRT_DIV, 0x07}, {MPU6050_CONFIG, 0x00}
    };
    for (unsigned i = 0; i < sizeof(registers) / sizeof(registers[0]); ++i) {
        value = registers[i][1];
        if (HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDREESS, registers[i][0],
                             I2C_MEMADD_SIZE_8BIT, &value, 1, 20) != HAL_OK) return HAL_ERROR;
    }
    HAL_Delay(100);
    if (HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDREESS, 0x75,
                        I2C_MEMADD_SIZE_8BIT, &value, 1, 3) != HAL_OK ||
        (value & 0x7eU) != 0x68U) return HAL_ERROR;
    for (unsigned i = 0; i < sizeof(registers) / sizeof(registers[0]); ++i) {
        if (HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDREESS, registers[i][0],
                            I2C_MEMADD_SIZE_8BIT, &value, 1, 3) != HAL_OK ||
            value != registers[i][1]) return HAL_ERROR;
    }
    return HAL_OK;
}

HAL_StatusTypeDef MPU6050_Update(MPU6050_Data_t *mpu)
{
    uint8_t bytes[14];
    if (mpu == NULL) return HAL_ERROR;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDREESS,
        ACCEL_XOUT_HIGH, I2C_MEMADD_SIZE_8BIT, bytes, sizeof(bytes), 3);
    if (status != HAL_OK) return status; /* Never publish a partial/stale sample. */
    MPU6050_Data_t fresh;
    fresh.AccX = (int16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
    fresh.AccY = (int16_t)((uint16_t)bytes[2] << 8 | bytes[3]);
    fresh.AccZ = (int16_t)((uint16_t)bytes[4] << 8 | bytes[5]);
    fresh.tempt = (int16_t)((uint16_t)bytes[6] << 8 | bytes[7]);
    fresh.GyroX = (int16_t)((uint16_t)bytes[8] << 8 | bytes[9]);
    fresh.GyroY = (int16_t)((uint16_t)bytes[10] << 8 | bytes[11]);
    fresh.GyroZ = (int16_t)((uint16_t)bytes[12] << 8 | bytes[13]);
    *mpu = fresh;
    return HAL_OK;
}

HAL_StatusTypeDef MPU6050_CalibrateGyro(void)
{
    /* Motors must remain disabled. Do not hold/move the car during this 1.3 s test. */
    float mean = 0.0f, m2 = 0.0f;
    float initial_ax = 0.0f, initial_ay = 0.0f, initial_az = 0.0f;
    for (unsigned n = 1; n <= 256; ++n) {
        MPU6050_Data_t sample;
        if (MPU6050_Update(&sample) != HAL_OK) return HAL_ERROR;
        float ax = sample.AccX / 2048.0f, ay = sample.AccY / 2048.0f;
        float az = sample.AccZ / 2048.0f;
        if (n == 1) { initial_ax = ax; initial_ay = ay; initial_az = az; }
        if (fabsf(ax - initial_ax) > 0.03f || fabsf(ay - initial_ay) > 0.03f ||
            fabsf(az - initial_az) > 0.03f) return HAL_ERROR;
        float norm2 = ax * ax + ay * ay + az * az;
        if (norm2 < 0.81f || norm2 > 1.21f ||
            abs(sample.GyroX) > 1638 || abs(sample.GyroZ) > 1638 ||
            abs(sample.GyroY) > 1638) return HAL_ERROR;
        float delta = sample.GyroY - mean;
        mean += delta / (float)n;
        m2 += delta * (sample.GyroY - mean);
        HAL_Delay(5);
    }
    /* Y-axis standard deviation below 1 dps. Constant slow motion remains unobservable. */
    if (m2 / 255.0f > 16.384f * 16.384f) return HAL_ERROR;
    mpu6050_gyro_bias_raw = mean;
    return HAL_OK;
}

void MPU6050_Calculate(MPU6050_Data_t *mpu, MPU6050_Data_f *mpu_f)
{
    mpu_f->AccX = mpu->AccX / 2048.0f;
    mpu_f->AccY = mpu->AccY / 2048.0f;
    mpu_f->AccZ = mpu->AccZ / 2048.0f;
    mpu_f->GyroX = mpu->GyroX / 16.384f;
    mpu_f->GyroY = (mpu->GyroY - mpu6050_gyro_bias_raw) / 16.384f;
    mpu_f->GyroZ = mpu->GyroZ / 16.384f;
    mpu_f->tempt = mpu->tempt / 340.0f + 36.53f;
}

/* Legacy test helpers below use fixed TIM_DT; the live loop uses Balance_Update. */
/**
 *
 * @param mpu_f 存放处理后的加速度和角速度以及温度
 * @param mpu_e 经过陀螺仪处理得到的欧拉角，放在结构体中
 */
void MPU6050_GetEuler_g(MPU6050_Data_f *mpu_f,MPU6050_Euler* mpu_e) {

    mpu_e->yaw =  mpu_e->yaw + mpu_f->GyroZ * TIM_DT;
    mpu_e->pitch =  mpu_e->pitch + mpu_f->GyroX * TIM_DT;
    mpu_e->roll =  mpu_e->roll + mpu_f->GyroY * TIM_DT;//间隔DT时间读取一次陀螺仪测算的值，要与定时器对应。

}


/**
 *
 * @param mpu_f 形参1为计算后的加速度和角速度
 * @param mpu_e 经过加速度计处理得到的欧拉角，放在结构体中
 * @retval row翻滚角和pitch俯仰角采用加速度计得到，yaw偏航角采用陀螺仪得到,会抖动
 */
void MPU6050_GetEuler_a(MPU6050_Data_f *mpu_f,MPU6050_Euler* mpu_e) {

    //mpu_e->yaw = mpu_e->yaw + TIM_DT * mpu_f->GyroZ;//得到的是角度
    mpu_e->pitch = -(float)(atan2(mpu_f->AccX, mpu_f->AccZ) * 180.f / 3.14159f +PITCH_ACC_ZERO_OFFSET); //得到的是弧度 需要转为角度  也就是这里
    //mpu_e->roll = atan2(mpu_f->AccX, mpu_f->AccZ) * 180.f / 3.14159f;

}


/**
 *
 * @param mpu_f 形参1为计算后的加速度和角速度
 * @param mpu_e 形参2为最终计算的欧拉角！
 * @retval 使用互补滤波函数计算欧拉角
 */
void MPU6050_GetEulerComplementary(MPU6050_Data_f *mpu_f,MPU6050_Euler *mpu_e) {
    //yaw,只能通过陀螺仪得到
    static float yaw,pitchg,rollg,pitcha,rolla;
    // yaw = yaw +  TIM_DT * mpu_f->GyroZ;
    //
    // //俯仰角两个 陀螺仪 加速度
     //rolla = atan2(mpu_f->AccY, mpu_f->AccZ) * 180.f / 3.1415926f;
     //rollg =COMPLEMENTARY_ALPHA * (rollg + TIM_DT * (mpu_f->GyroX))+(1-COMPLEMENTARY_ALPHA) * rolla;

    //翻滚角两个
    pitcha =  -(float)(atan2f(mpu_f->AccX, mpu_f->AccZ) * 180.f / 3.14159f +PITCH_ACC_ZERO_OFFSET);
    pitchg =  COMPLEMENTARY_ALPHA * (pitchg +  TIM_DT * mpu_f->GyroY)+(1-COMPLEMENTARY_ALPHA) * pitcha;

    //互补滤波融合
    //mpu_e->yaw = yaw;//得到的是欧拉角
    mpu_e->pitch = pitchg+BALANCE_ANGLE_OFFSET;
   // mpu_e->roll =rollg;

}


/**
 *
 * @param mpu_e 1
 * @return 得到经过处理的俯仰角
 */
float GetPitchAngle(MPU6050_Euler *mpu_e) {
    return mpu_e->pitch;
}
