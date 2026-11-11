#include <stdio.h>
#include <string.h>

#include "main.h"
//
// Created by LWadmin on 2026/8/23.
//
uint8_t Test_BlueTooth() {
    char *msg = "123123 this is a test\r\n";
    uint8_t data;
    HAL_UART_Transmit(&huart2,
                      (uint8_t *)msg,
                      strlen(msg),
                      1000);

    HAL_UART_Receive(&huart2,&data,1,1000);
    HAL_Delay(100);
    return data;
}