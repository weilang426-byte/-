
#include "Key.h"

/**
 *
 * @return 返回按键状态，如果Key1被按下则返回1，以此类推；否则返回0
 */
uint8_t Keys_GetStates(void) {

    if (key1.press_event == 1) {
        //k1被按下
        key1.press_event =0;
        return 1;
    }else if (key2.press_event == 1) {
        key2.press_event =0;
        return 2;
    }else if (key3.press_event == 1) {
        key3.press_event =0;
        return 3;
    }else if (key4.press_event == 1) {
        key4.press_event =0;
        return 4;
    }else return 0 ;
}


/**
 *
 * @param key 结构体
 * @param GPIOx 按键
 * @param GPIO_Pin 该外设按键实际绑定结构体key
 * @retval 状态机实现每个按键切换状态，非阻塞式
 */
void Key_FSMUpdate(KeyControl *key, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    uint8_t pressed =
        (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET);//若按下，pressed为1，否则为0

    uint32_t now = HAL_GetTick();//记录当前执行时间

    switch (key->state)//
    {
        case KEY_IDLE://稳定松开状态时，pressed为1（按下)后进入if语句
            if (pressed) {
                key->tick = now;//保存切换状态的时间，然后进入检测到按下状态
                key->state = KEY_DEBOUNCE_DOWN;
            }
            break;

        case KEY_DEBOUNCE_DOWN:
            if (!pressed) {//如果这个时候pressed已经为0（松开了),则认为这是一次抖动，不是实际按下
                // 按下时间不足，认为是抖动
                key->state = KEY_IDLE;
            }
            else if (now - key->tick >= KEY_DEBOUNCE_MS) {//否则，超过20ms，认为是一次按下，进入稳定按下状态，且记录一次按下事件
                // 按下已稳定
                key->state = KEY_PRESSED;
                key->press_event = 1;
            }
            break;

        case KEY_PRESSED://在稳定按下状态下，如果检测到松开，则进入检测到松开状态，
            if (!pressed) {
                key->tick = now;
                key->state = KEY_DEBOUNCE_UP;
            }
            break;

        case KEY_DEBOUNCE_UP:
            if (pressed) {
                // 松开时间不足，认为是抖动
                key->state = KEY_PRESSED;
            }
            else if (now - key->tick >= KEY_DEBOUNCE_MS) {//否则认为是一次有效的松开，进入稳定松开状态
                // 松开已稳定
                key->state = KEY_IDLE;
            }
            break;
    }
}
