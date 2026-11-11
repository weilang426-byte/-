//
// Created by LWadmin on 2026/8/24.
//

#include "main.h"

/**
 * @retval 时间片
 */
void SetTick(void) {//手动实现时间片
    uint32_t now = HAL_GetTick(); //获取系统的执行时间 单位为毫秒
    static uint32_t tick1 = 0;//三个任务
    static uint32_t tick2 = 0;
    static uint32_t tick3 = 0;

    if (now - tick1 >= 10) {//10ms执行一次
        tick1 = now;
        //执行对应函数
    }
    if (now - tick2 >= 25)//25ms执行一次
    {
        tick2 = now;
        //执行对应函数 ...
    }
}
