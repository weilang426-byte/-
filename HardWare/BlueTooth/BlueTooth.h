//
// Created by LWadmin on 2026/9/1.
//

#ifndef PROJECT_BALANCECAR_BLUETOOTH_H
#define PROJECT_BALANCECAR_BLUETOOTH_H

#include "main.h"
#include "OLED.h"
#include "BluTooth_Head.h"
#include <string.h>
#include "OLED.h"
#include "OLED_Data.h"
#include <stdlib.h>
#include "PID.h"


/*摇杆的横向和纵向最大值均为100，当y为100时，前进，满占空比*/
typedef  struct joystick {
    //4个数据，左前进后退，右转向旋转
    int16_t LeftX;
    int16_t LeftY;
    int16_t RightX;
    int16_t RightY;
}joystick;



extern PID_Base_variables  PID_angle ;
extern joystick PID_joystick ;
extern volatile uint8_t PID_angle_flag;
extern PID_Base_variables  PID_left,PID_right;;

extern volatile uint8_t rx_data;
extern volatile uint8_t rx_flag;
extern char buffer[BLUETOOTH_PACKET_LENGTH];//蓝牙接收数据包的长度，目前位50

/*模块简单实现发送1字节数据和接收1字节数据*/

//收发数据帧，结合中断的方式
//注意以[开头 以]结尾就可以了


void BlueTooth_ReceiveStart(void);

void BlueTooth_Receive(void);

#endif //PROJECT_BALANCECAR_BLUETOOTH_H
