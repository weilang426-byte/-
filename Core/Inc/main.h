/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;



extern volatile uint8_t key_flag ;
extern volatile uint32_t key_last_tick;


typedef struct {
  uint8_t flag_angle;//10
  uint8_t flag_speed;//10
  uint8_t flag_bluetooth;//20
  uint8_t flag_oled;//100
  uint8_t flag_key;//10ms检测一次
}Flags;





/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PC13_32_Pin GPIO_PIN_13
#define PC13_32_GPIO_Port GPIOC
#define TIM2_PWMA_Pin GPIO_PIN_0
#define TIM2_PWMA_GPIO_Port GPIOA
#define TIM2_PWMB_Pin GPIO_PIN_1
#define TIM2_PWMB_GPIO_Port GPIOA
#define Bluetooth_TX_Pin GPIO_PIN_2
#define Bluetooth_TX_GPIO_Port GPIOA
#define Bluetooth_RX_Pin GPIO_PIN_3
#define Bluetooth_RX_GPIO_Port GPIOA
#define Button_K4_Pin GPIO_PIN_4
#define Button_K4_GPIO_Port GPIOA
#define Button_K3_Pin GPIO_PIN_5
#define Button_K3_GPIO_Port GPIOA
#define TIM3_EA_Pin GPIO_PIN_6
#define TIM3_EA_GPIO_Port GPIOA
#define TIM3_EB_Pin GPIO_PIN_7
#define TIM3_EB_GPIO_Port GPIOA
#define Button_K2_Pin GPIO_PIN_0
#define Button_K2_GPIO_Port GPIOB
#define Button_K1_Pin GPIO_PIN_1
#define Button_K1_GPIO_Port GPIOB
#define MPU6050_SCL_Pin GPIO_PIN_10
#define MPU6050_SCL_GPIO_Port GPIOB
#define MPU6050_SDA_Pin GPIO_PIN_11
#define MPU6050_SDA_GPIO_Port GPIOB
#define AIN1_Pin GPIO_PIN_12
#define AIN1_GPIO_Port GPIOB
#define AIN2_Pin GPIO_PIN_13
#define AIN2_GPIO_Port GPIOB
#define BIN1_Pin GPIO_PIN_14
#define BIN1_GPIO_Port GPIOB
#define BIN2_Pin GPIO_PIN_15
#define BIN2_GPIO_Port GPIOB
#define Debug_TX_Pin GPIO_PIN_9
#define Debug_TX_GPIO_Port GPIOA
#define Debug_RX_Pin GPIO_PIN_10
#define Debug_RX_GPIO_Port GPIOA
#define TIM4_EA_Pin GPIO_PIN_6
#define TIM4_EA_GPIO_Port GPIOB
#define TIM4_EB_Pin GPIO_PIN_7
#define TIM4_EB_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_8
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_9
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
