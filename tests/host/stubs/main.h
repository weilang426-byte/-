#ifndef HOST_MAIN_H
#define HOST_MAIN_H
#include <stdint.h>
#include <stddef.h>
typedef enum { HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT } HAL_StatusTypeDef;
typedef struct { int unused; } I2C_HandleTypeDef;
typedef struct { int unused; } UART_HandleTypeDef;
typedef struct { uint32_t CCR1, CCR2, CCER, PSC, ARR; } TIM_TypeDef;
typedef struct { TIM_TypeDef *Instance; } TIM_HandleTypeDef;
typedef struct { uint32_t CYCCNT, CTRL; } DWT_Type;
typedef struct { uint32_t DEMCR; } CoreDebug_Type;
extern DWT_Type host_dwt;
extern CoreDebug_Type host_debug;
extern TIM_TypeDef host_tim;
extern I2C_HandleTypeDef hi2c2;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;
extern uint32_t SystemCoreClock;
#define DWT (&host_dwt)
#define CoreDebug (&host_debug)
#define TIM2 (&host_tim)
#define CoreDebug_DEMCR_TRCENA_Msk 1U
#define DWT_CTRL_CYCCNTENA_Msk 1U
#define TIM_CCER_CC1E 1U
#define TIM_CCER_CC2E 16U
#define TIM_CHANNEL_1 1U
#define TIM_CHANNEL_2 2U
#define I2C_MEMADD_SIZE_8BIT 1U
#define GPIOB 0
#define GPIO_PIN_12 12
#define GPIO_PIN_13 13
#define GPIO_PIN_14 14
#define GPIO_PIN_15 15
#define GPIO_PIN_SET 1
#define GPIO_PIN_RESET 0
#define __HAL_TIM_SET_PRESCALER(h,v) ((h)->Instance->PSC=(v))
#define __HAL_TIM_SET_AUTORELOAD(h,v) ((h)->Instance->ARR=(v))
#define __HAL_TIM_SET_COMPARE(h,c,v) do { if ((c)==TIM_CHANNEL_1) (h)->Instance->CCR1=(v); else (h)->Instance->CCR2=(v); } while(0)
static inline uint32_t __get_PRIMASK(void) { return 0; }
static inline void __disable_irq(void) {}
static inline void __set_PRIMASK(uint32_t value) { (void)value; }
static inline void __DMB(void) {}
uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t ms);
void HAL_GPIO_WritePin(int port, int pin, int value);
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *, uint32_t);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *, uint8_t *, uint16_t);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *, uint16_t, uint16_t, uint16_t, uint8_t *, uint16_t, uint32_t);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *, uint16_t, uint16_t, uint16_t, uint8_t *, uint16_t, uint32_t);
#endif
