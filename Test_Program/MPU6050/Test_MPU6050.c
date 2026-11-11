//
// Created by LWadmin on 2026/8/23.
//
#include <stdio.h>
#include "main.h"
#include "MPU6050.h"
#include "stm32f1xx_hal.h"
#include "string.h"

/**
 * @retval 简单的测试程序，测试各个函数能否运行，是否可以向串口发送
 * 该测试得到的是加速度和角速度，并没有测算欧拉角
 */
void print_datas(void) {
    MPU6050_Init();
    MPU6050_Data_t data;
    MPU6050_Data_f data_f;
    MPU6050_Euler data_e;
    char str[200];
    MPU6050_Update(&data);
    MPU6050_Calculate(&data,&data_f);
    snprintf(str,sizeof(str),"accx:%.3fg,accy:%.3fg,accz:%.3fg,tempt:%.3f℃,gyrox:%.3f度/秒,gyroy:%.3f度/秒,gyroz:%.3f度/秒\r\n",
      data_f.AccX,data_f.AccY,data_f.AccZ,data_f.tempt,data_f.GyroX,data_f.GyroY,data_f.GyroZ);

    HAL_Delay(100);
    HAL_UART_Transmit(&huart1,(uint8_t *)str,strlen(str),10);
}


void print_datas_euler(void) {


    // char str[50];
    // MPU6050_Update(&data_t);
    // MPU6050_Calculate(&data_t,&data_f);
    // MPU6050_GetEulerComplementary(&data_f,&data_euler);
    // snprintf(str,sizeof(str),"%.3f,%.3f,%.3f\r\n",data_euler.roll,data_euler.pitch,data_euler.yaw);
    // HAL_UART_Transmit(&huart1,(uint8_t *)str,strlen(str),100);


}