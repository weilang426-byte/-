

#include "MPU6050.h"
static uint8_t buffer[14];//存放读取到的数据 14个字节的数据

/**
 * @retval 初始化MPU6050，目的是为了调整加速度计和陀螺仪的量程
 * 首先复位MPU6050后，解除休眠模式，然后设置加速度计和陀螺仪计的量程
 */
void MPU6050_Init(void) {

    uint8_t data_PWR1 = 0x80;
    uint8_t data_PWR2 = 0x01;
    uint8_t data_PWR3 = 0x00;
    uint8_t gyro = 0x18;//量程调整 由 bit4、bit3决定   正负2000°
    uint8_t accel = 0x18;// 正负16g

//后续调参重点，目前频率1000HZ
    uint8_t smplrt = 0x07;
    uint8_t config = 0x00;


    //写入寄存器PWR1，内容0x80，复位MPU6050,设备复位
     HAL_I2C_Mem_Write(&hi2c2,MPU6050_ADDREESS,PWR_MGMT_1,I2C_MEMADD_SIZE_8BIT,&data_PWR1,1,100);
    HAL_Delay(100);

    //内容0x01，原来0x00 关闭睡眠模式加时钟选择（最后2位）
     HAL_I2C_Mem_Write(&hi2c2,MPU6050_ADDREESS,PWR_MGMT_1,I2C_MEMADD_SIZE_8BIT,&data_PWR2,1,100);

    //电源模式2
    HAL_I2C_Mem_Write(&hi2c2,MPU6050_ADDREESS,PWR_MGMT_2,I2C_MEMADD_SIZE_8BIT,&data_PWR3,1,100);

    //设置加速度计的量程
     HAL_I2C_Mem_Write(&hi2c2,MPU6050_ADDREESS,ACCEL_ADDREESS,I2C_MEMADD_SIZE_8BIT,&accel,1,100);

    //设置陀螺仪的量程
     HAL_I2C_Mem_Write(&hi2c2,MPU6050_ADDREESS,GYRO_ADDREESS,I2C_MEMADD_SIZE_8BIT,&gyro,1,100);

    //采样和滤波 原来0x00 现在0x07
    HAL_I2C_Mem_Write(&hi2c2,MPU6050_ADDREESS,MPU6050_SMPLRT_DIV,I2C_MEMADD_SIZE_8BIT,&smplrt,1,100);

    HAL_I2C_Mem_Write(&hi2c2,MPU6050_ADDREESS,MPU6050_CONFIG,I2C_MEMADD_SIZE_8BIT,&config,1,100);

    HAL_Delay(100);

}




/**
 *
 * @param mpu 参数为一个结构体，存放加速度计 陀螺仪计和温度的原始数据
 * @retval 得到一组原始数据，放到结构体中 通过I2C一次性读取全部数据
 */
void MPU6050_Update(MPU6050_Data_t *mpu ) {

    if (mpu == NULL) { return; }
    HAL_I2C_Mem_Read(&hi2c2,MPU6050_ADDREESS,ACCEL_XOUT_HIGH,I2C_MEMADD_SIZE_8BIT ,buffer,14,20);
    //对数据进行处理
    //accx
    mpu->AccX =(int16_t)(((uint16_t)buffer[0] << 8) | buffer[1]);

    //accy
    mpu->AccY = (int16_t)(((uint16_t)buffer[2] << 8) | buffer[3]);

    //accz
    mpu->AccZ = (int16_t)(((uint16_t)buffer[4] << 8) | buffer[5]);

    //tempt
    mpu->tempt =(int16_t)(((uint16_t)buffer[6] << 8) | buffer[7]);

    //gyrox
    mpu->GyroX = (int16_t)(((uint16_t)buffer[8] << 8) | buffer[9]);

    //gyroy
    mpu->GyroY = (int16_t)(((uint16_t)buffer[10] << 8) | buffer[11] ) ;

    //gyroz
    mpu->GyroZ = (int16_t)(((uint16_t)buffer[12] << 8) | buffer[13]);

    // int32_t gyroy = (int32_t)(mpu->GyroY-25);
    // mpu->GyroY = gyroy;

}



/**
 *
 * @param mpu 存放原始数据的结构体
 * @param mpu_f 存放处理后的加速度和角速度以及温度
 */
void MPU6050_Calculate(MPU6050_Data_t *mpu,MPU6050_Data_f * mpu_f) {//int16_t
    //目前加速度计的量程为正负16g 陀螺仪为正负2000° 对应


    mpu_f->AccX = (float)mpu->AccX * 16.0f / 32768.0f;
    //mpu_f->AccY = (float)mpu->AccY * 16.0f / 32768.0f;//不需要
    mpu_f->AccZ = (float)mpu->AccZ * 16.0f / 32768.0f;

    //mpu_f->GyroX= (float)(mpu->GyroX+90) * 4000.0f / 65536.0f;//
    mpu_f->GyroY= (float)(mpu->GyroY - ZERO_OFFSET_RAW) /16.384f;//零漂校准
    //mpu_f->GyroZ= (float)mpu->GyroZ * 4000.0f / 65536.0f;

    //mpu_f->tempt= (float)mpu->tempt / 340.0f +36.53;

}


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