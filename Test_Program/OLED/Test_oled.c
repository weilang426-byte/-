//
// Created by LWadmin on 2026/8/23.
//
//
// Created by LWadmin on 2026/8/23.
//
#include "OLED.h"

/**
 * @retval OLED的显示测试，所有的驱动函数已经写好
 */
void OLED_Show() {

    OLED_ShowString(0,0,"this is a test",OLED_8X16);
    OLED_ShowNum(0,16,1050,4,OLED_8X16);
    OLED_Update();
}