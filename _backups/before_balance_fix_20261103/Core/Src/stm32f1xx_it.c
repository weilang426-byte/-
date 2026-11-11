/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "MPU6050.h"
#include "Motor.h"
#include "Encoder.h"
#include "Key.h"
#include "BluTooth_Head.h"
#include "PID.h"
#include "Motor_Head.h"
#include "BlueTooth.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/*TIM*/
extern uint32_t counter;

/*Motor*/
extern int16_t CNTL,CNTR;

extern float PWM_Left,PWM_Right;

/*bluetooth*/
extern volatile uint8_t rx_data;
extern char buffer[BLUETOOTH_PACKET_LENGTH];//蓝牙接收数据包的长度，目前位50
extern volatile uint8_t rx_flag;//指示是否已经接收完成，0未完成，1完成
extern joystick PID_joystick ;

/*Button*/
extern volatile KeyControl key1 ;//依次对应Button_K1/2/3/4
extern volatile KeyControl key2 ;
extern volatile KeyControl key3 ;
extern volatile KeyControl key4 ;



/*PID*/
extern volatile float FB_left ,err_left,CCR_left ;
extern volatile float FB_right ,err_right ,CCR_right;
extern PID_Base_variables  PID_right;
extern PID_Base_variables  PID_left;
extern PID_Base_variables  PID_angle;

extern PID_Compute_variables PID_Compute_left ;
extern PID_Compute_variables PID_Compute_right;
extern PID_Compute_variables PID_Compute_angle ;
extern volatile joystick Car_control ;


//angle
extern volatile float FB_angle,err_angle ,CCR_angle;

extern volatile float PWM_AVG,PWM_DIFF;//平均CCR和差分CCR

extern volatile uint8_t PID_angle_flag ;


/*SpeedPID*/
extern PID_Base_variables  PID_speed;//角度环PID
extern PID_Compute_variables PID_Compute_speed;
extern volatile float FB_speed,err_speed ,CCR_speed ;//FB ERR CO的值
extern volatile float FB_AVG,FB_DIFF;//平均CCR和差分CCR

/*DirectPID*/
extern PID_Base_variables  PID_direct;//角度环PID
extern PID_Compute_variables PID_Compute_direct;
extern volatile float FB_direct ,err_direct ,CCR_direct ;//FB ERR CO的值


/*MPU6050*/
extern MPU6050_Data_t data_t;//存放7个原始数据

extern MPU6050_Data_f data_f;//存放加速度/角速度等
extern MPU6050_Euler data_euler;//存放欧拉角



extern volatile Flags flags ;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim1;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM1 update interrupt.
  */
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */

  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */

  /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */


/**
 *
 * @param htim 检测触发源 定时中断
 * @retval 更新中断秒计数器，TIM11秒定时一次
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  static uint8_t count_key,count_speed,count_PID_Speed,count_angle,count_direct,count_Bluetooth,count_oled;
  if (htim->Instance == TIM1)
  {
    // TIM1更新中断发生后执行这里
    count_key++;//20ms读一次按键状态
    count_speed++;
    count_PID_Speed++;
    count_angle++;
    count_direct++;
    count_Bluetooth++;
    count_oled++;
    if (count_key >= 10) {//10ms一次按键状态
      count_key = 0;
      //读取按键状态
      flags.flag_key = 1;

    }
    if (count_speed >= 20) {//20ms一次 读取当前的CNT  一会记得改回来
      count_speed=0;

      flags.flag_speed = 1;


    }

    if (count_Bluetooth >= 20) {//20ms读取一次蓝牙摇杆得到的速度和转向
      count_Bluetooth=0;

      flags.flag_bluetooth = 1;

    }
    if (count_angle >= 10)
    {//角度环 重要
      count_angle = 0;
      flags.flag_angle = 1;

    }

    if (count_oled >= 200) {
      //角度环 重要
      count_oled = 0;
      flags.flag_oled = 1;
    }


    // if (count_direct >= 20) {//PID级联控制
    //   count_direct = 0;
    //
    //   FB_angle = GetPitchAngle(&data_euler);//得到的是角度反馈 [-90,90]
    //   FB_left = Encoder_GetSpeed(Motor_left);
    //   FB_right = Encoder_GetSpeed(Motor_right);
    //   FB_AVG = 0.5f * (FB_left+FB_right);//平均速度
    //   FB_DIFF = FB_left -FB_right;//差分速度
    //
    //   /*关键在平均PWM的计算*/
    //   PID_angle.SP = PID_Compute(&PID_speed,&PID_Compute_speed,FB_AVG);//得到的是角度，进行一个角度限幅
    //   /**/
    //   PWM_AVG = PID_Compute(&PID_angle,&PID_Compute_angle,FB_angle);
    //   //差分PWM通过差分速度得到
    //   PWM_DIFF = PID_Compute(&PID_direct,&PID_Compute_direct,FB_DIFF);//得到的是差分PWM
    //
    //
    //   //计算左右轮PWM
    //   PWM_Left = PWM_AVG +PWM_DIFF * 0.5f;
    //   PWM_Right = PWM_AVG -PWM_DIFF * 0.5f;
    //
    //   //限幅
    //   if (PWM_Left > 100.0f) PWM_Left = 100.0f;
    //   else if (PWM_Left < -100.0f) PWM_Left = -100.0f;
    //   if (PWM_Right > 100.0f) PWM_Right = 100.0f;
    //   else if (PWM_Right < -100.0f) PWM_Right = -100.0f;
    //
    //   //输出到电机
    //   Motor_SetPWM(Motor_left ,PWM_Left);
    //   Motor_SetPWM(Motor_right ,PWM_Right);
    // }

  //     if (count_PID_Speed >=20) {//速度PID调试
  //       count_PID_Speed = 0;
  // //sp小于0
  //       FB_left = -Encoder_GetSpeed(Motor_left);
  //       FB_right = Encoder_GetSpeed(Motor_right);
  //
  //       PWM_Left = PID_Compute(&PID_left,&PID_Compute_left,FB_left);
  //       PWM_Right = PID_Compute(&PID_right,&PID_Compute_right,FB_right);
  //
  //
  //       //限幅
  //       if (PWM_Left > 100.0f) PWM_Left = 100.0f;
  //       else if (PWM_Left < -100.0f) PWM_Left = -100.0f;
  //       if (PWM_Right > 100.0f) PWM_Right = 100.0f;
  //       else if (PWM_Right < -100.0f) PWM_Right = -100.0f;
  //
  //       //输出到电机
  //       Motor_SetPWM(Motor_left ,PWM_Left);
  //       Motor_SetPWM(Motor_right ,PWM_Right);
  //     }

  }
}



/**
 *
 * @param huart 标记是否是huart2
 * @retval 回调函数用来接收蓝牙串口发送来的信息，以'['为帧头，']'为帧尾,接收到的数据存放到msg中，且一个完整的数据帧接收完成后flag置1可以读走
 * @retval msg的最后1位保存该数据帧的长度。采用状态机的思想编程
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  static uint8_t rx_state = 0;//标记蓝牙是否处于空闲状态，区分与flag的区别
  static uint8_t i = 0;
  if (huart->Instance == USART2)
  {

    if (rx_state == 0) {//空闲中 可以接收[
      if (rx_data == '[' && rx_flag == 0) {//flag == 1表示接收完成，可以取走，为0表示还没有开始接收呢
        rx_state = 1;//开始接收，处于忙态  
        i = 0;
      }
    } else {
      if (rx_data == ']') {
        buffer[i] = '\0';//最后一位是\0
        buffer[sizeof(buffer)-1] = i;//msg的最后一位存放该数据帧的长度
        rx_flag = 1;//表示接收完成
        rx_state = 0;//退出忙状态

      } else if (i < sizeof(buffer) - 2) {//48个有效字节，帧结束存\0
        buffer[i++] = (char)rx_data;
      } else {
        //该数据包过长，丢弃
        rx_state = 0;
        i = 0;
      }
    }

    HAL_UART_Receive_IT(&huart2,&rx_data,1);//继续接受一个字节放到rxdata中
  }
}

/* USER CODE END 1 */
