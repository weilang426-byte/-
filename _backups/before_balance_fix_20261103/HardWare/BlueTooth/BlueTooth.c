//
// Created by LWadmin on 2026/9/1.
//

#include "BlueTooth.h"



/**
 * @retval 只负责开机
 */
void BlueTooth_ReceiveStart(void) {

    HAL_UART_Receive_IT(&huart2,&rx_data,1);//启动接收，中断通知

}





//时间片，蓝牙接收
/**
 * @retval 接收蓝牙发来的数据，包括按键 滑杆 摇杆数据
 */
void BlueTooth_Receive(void) {
    /*蓝牙收发测试*/
    if (rx_flag == 1) {//数据包经过处理后无[]且,分隔
        //下面使用数据帧
        //按键数据格式
            //自锁:按下[key,项名,down],松开[key,项名,up]
            //非自锁：按一下[key,项名,down]，其余都是[key,项名,up]
        //滑杆数据格式 [slider,项名称,值]
        //摇杆数据格式 [joystick,0,0,0,0]

        char * p;
        uint8_t i = 0;
        p = strtok(buffer,",");
        if (!strcmp(p,"key")) {//3项 用按键控制角度环开启 RST复位等功能
            p=strtok(NULL,",");
            if (!strcmp(p,"angle")) {
                p=strtok(NULL,",");
                if (!strcmp(p,"down")) {//角度环开启
                    //
                   // PID_angle_flag = 1;
                }else if (!strcmp(p,"up")) {//关闭角度环
                    //
                   // PID_angle_flag = 0;
                }else;
            }else if (!strcmp(p,"RST")) {
                //复位信号
            }


        }else if (!strcmp(p,"slider")) {//3项 主要用来调试KpKiKd
            p=strtok(NULL,",");
            if (!strcmp(p,"Kp")) {//调整Kp 第一个摇杆Kp
                PID_angle.Kp = atof(strtok(NULL,","));//左轮测试
            }else if (!strcmp(p,"Ki")) {
                PID_angle.Ki = atof(strtok(NULL,","));
            }else if (!strcmp(p,"Kd")) {
                PID_angle.Kd = atof(strtok(NULL,","));
            }else;
        }else if (!strcmp(p,"joystick")) {//4项  主要用来控制移动
            //存放4个数据
            while ((p = strtok(NULL, ",")) != NULL) {
                if (i == 0) PID_joystick.LeftX = atoi(p);
                else if (i == 1) PID_joystick.LeftY = atoi(p);
                else if (i == 2) PID_joystick.RightX = atoi(p);
                else if (i == 3) PID_joystick.RightY = atoi(p);
                else;
                i++;
            }
        }else;
        rx_flag = 0;
    }else;

}
