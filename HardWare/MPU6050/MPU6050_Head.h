

#ifndef TEST_MPU6050_MAIN_MPU6050_HEAD_H
#define TEST_MPU6050_MAIN_MPU6050_HEAD_H
/**
 *定义一些宏
 *
 */

/*地址*/
#define MPU6050_ADDREESS 0xD0


/*寄存器地址-初始化设置*/
#define PWR_MGMT_1     0x6B //电源管理模式
#define PWR_MGMT_2     0x6C //电源管理模式
#define GYRO_ADDREESS  0x1B   //陀螺仪
#define ACCEL_ADDREESS 0x1C  //加速度计
#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A

/*寄存器地址-参数读取*/
#define ACCEL_XOUT_HIGH 0x3B //加速度计 8bit
#define ACCEL_XOUT_LOW  0x3C
#define ACCEL_YOUT_HIGH 0x3D
#define ACCEL_YOUT_LOW  0x3E
#define ACCEL_ZOUT_HIGH 0x3F
#define ACCEL_ZOUT_LOW  0x40

#define MPU6050_TEMP_OUT_HIGH 0x41 //MPU6050读取的温度，用MPU6050的算法
#define MPU6050_TEMP_OUT_LOW  0x42

#define GYRO_XOUT_HIGH 0x43//陀螺仪
#define GYRO_XOUT_LOW  0x44
#define GYRO_YOUT_HIGH 0x45
#define GYRO_YOUT_LOW  0x46
#define GYRO_ZOUT_HIGH 0x47
#define GYRO_ZOUT_LOW  0x48

#define COMPLEMENTARY_ALPHA      0.95f  //互补滤波系数
#define TIM_DT          0.01f

#endif //TEST_MPU6050_MAIN_MPU6050_HEAD_H
