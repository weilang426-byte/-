
#ifndef PROJECT_BALANCECAR_KEY_H
#define PROJECT_BALANCECAR_KEY_H
#include "main.h"

#define KEY_DEBOUNCE_MS 20

/**
 * 状态机实现按键非阻塞式消抖，包含4个状态
 */
typedef enum {//一个按键有4个状态，等待状态切换
    KEY_IDLE,          // 松开稳定
    KEY_DEBOUNCE_DOWN, // 检测到按下，等待确认
    KEY_PRESSED,       // 按下稳定
    KEY_DEBOUNCE_UP    // 检测到松开，等待确认
} KeyState;


//单个按键逻辑
typedef struct {
    KeyState state;//状态
    uint32_t tick;//记录刚切换时的时间
    uint8_t press_event;//按键按下事件
} KeyControl;


extern volatile KeyControl key1 ;//依次对应Button_K1/2/3/4
extern volatile KeyControl key2 ;
extern volatile KeyControl key3 ;
extern volatile KeyControl key4 ;

//extern volatile uint8_t key_flag1, key_flag2, key_flag3, key_flag4 ;//标记按键是否被按下
//extern volatile uint16_t key_last_tick1, key_last_tick2, key_last_tick3, key_last_tick4;//标记上个时间
//extern volatile int16_t CCR12,CCR34;

uint8_t Keys_GetStates(void);
void Key_FSMUpdate(KeyControl *key, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);



#endif //PROJECT_BALANCECAR_KEY_H
