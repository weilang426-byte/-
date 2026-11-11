/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <sys/stat.h>

/*外设和系统文件*/
#include "OLED.h"
#include "mpu6050.h"
#include "Encoder.h"
#include "Motor.h"
#include "string.h"
#include "stdio.h"
#include "LED.h"
#include "TIM_Counter.h"
#include "Key.h"
#include "BlueTooth.h"
#include "Motor_Head.h"
#include "BluTooth_Head.h"
#include "PID.h"
#include <stdlib.h>

/*测试程序*/
#include "Test_oled.h"
#include "Test_MPU6050.h"
#include "Test_Bluetooth.h"
#include "Test_Motor.h"
#include "Test_Encoder.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/*TIM*/
volatile uint32_t counter;//指示TIM1的CNT的值



/*Key*/

volatile KeyControl key1 = {KEY_IDLE,0,0};//依次对应Button_K1/2/3/4
volatile KeyControl key2 = {KEY_IDLE,0,0};//检测是否按下
volatile KeyControl key3 = {KEY_IDLE,0,0};
volatile KeyControl key4 = {KEY_IDLE,0,0};

/*Motor*/
int16_t CNTL=0,CNTR=0;//电机左右两侧CNT
float SpeedL=0,SpeedR=0;//电机左右两侧速度
float PWM_Left=0,PWM_Right=0;

/*蓝牙*/
volatile uint8_t rx_data;//蓝牙开始接收的第一给而数据
char buffer[BLUETOOTH_PACKET_LENGTH];//接收的数据缓冲区
volatile uint8_t rx_flag = 0;//为1时表示收到了数据包，转入接收程序
joystick PID_joystick = {0,0,0,0};//摇杆数据

/*MPU6050*/
MPU6050_Data_t data_t;//存放7个原始数据
MPU6050_Data_f data_f;//存放加速度/角速度等
MPU6050_Euler data_euler;//存放欧拉角


/*PID*/
PID_Base_variables  PID_right= {0,0,0,0,0,0,0};//左轮PID
PID_Base_variables  PID_left= {0,0,0,0,0,0,0};//左轮PID

PID_Compute_variables PID_Compute_left = {0,0,0,0};
PID_Compute_variables PID_Compute_right = {0,0,0,0};

volatile float FB_left = 0,err_left = 0,CCR_left = 0;
volatile float FB_right = 0,err_right = 0,CCR_right = 0;
volatile joystick Car_control = {0,0,0,0};

/*anglePID*/
PID_Base_variables  PID_angle= {0,0,0,0,0,0,0};//角度环PID
PID_Compute_variables PID_Compute_angle = {0,0,0,0};
volatile float FB_angle = 0,err_angle = 0,CCR_angle = 0;//FB ERR CO的值

volatile float PWM_AVG=0,PWM_DIFF=0;//平均CCR和差分CCR
volatile uint8_t PID_angle_flag = 0;//为1时表示角度环开启


/*SpeedPID*/
PID_Base_variables  PID_speed= {0,0,0,0,0,0,0};//角度环PID
PID_Compute_variables PID_Compute_speed = {0,0,0,0};
volatile float FB_speed = 0,err_speed = 0,CCR_speed = 0;//FB ERR CO的值
volatile float FB_AVG=0,FB_DIFF=0;

/*DirectPID*/
PID_Base_variables  PID_direct= {0,0,0,0,0,0,0};//角度环PID
PID_Compute_variables PID_Compute_direct = {0,0,0,0};
volatile float FB_direct = 0,err_direct = 0,CCR_direct = 0;//FB ERR CO的值


volatile Flags flags = {0,0,0,0,0};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */



  OLED_Init();//OLED底层硬件驱动初始化 像素128*64
  MPU6050_Init();
  Motor_Init();
  Encoder_Init();
  BlueTooth_ReceiveStart();//启动


  //初始化PID的三个变量
  PID_Init(&PID_left, 0.01,0.65,0,100,-100,100);
  PID_Init(&PID_right,0.01,0.65,0,100,-100,100);
  PID_Init(&PID_speed, 0.01,0.65,0,100,-100,100);//速度环
  PID_Init(&PID_angle,1.9,0,0,80,-80,15);//后续等待角度环测试后得到PID值
  PID_Init(&PID_direct, 0, 0, 0,60,-60,40);//转向环

  TIM1_Init();

  //设置SP
  PID_SetSP(&PID_left,00);//SP这里为转速，上限为14.58*60 = 850
  PID_SetSP(&PID_right,00);
  PID_SetSP(&PID_speed,00);

  PID_SetSP(&PID_angle,00);
  PID_SetSP(&PID_direct,00);

  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {//TIM1  1ms中断依次








    /*时间片*/

  //   uint32_t now = HAL_GetTick(); //获取系统的执行时间 单位为毫秒
  //   static uint32_t tick1 = 0;//4个任务
  //   static uint32_t tick2 = 0;
  //   static uint32_t tick3 = 0;
  //   static uint32_t tick4 = 0;
  //
  //    // if (now - tick1 >= 10) {//蓝牙接收数据
  //    //   tick1 = now;
  //    //
  //    // }
  //    if (now - tick3 >= 10) {
  //      //10ms执行一次
  //      tick3 = now;
  //
  //      /*状态机开启角度环*/
  //      if (key1.press_event == 1) {//按键1按下，开启角度环
  //        key1.press_event = 0;
  //        static uint8_t i = 0;
  //        i++;
  //
  //        if (i % 2 == 1) {//奇数次按下
  //          PID_angle_flag = 1;
  //        }else {
  //          PID_angle_flag = 0;
  //        }
  //      }
  //      if (GetPitchAngle(&data_euler) >50 || GetPitchAngle(&data_euler) <-50) {//范围是-90到90度
  //        PID_angle_flag = 0;
  //      }
  //      switch (PID_angle_flag) {
  //        case 1://按下之后，角度环开启 留
  //        {
  //          PID_angle.SP = 1.2;
  //          LED_ON();
  //          break;
  //        }
  //        case 0:
  //        {
  //          LED_OFF();
  //
  //          PID_Compute_angle.err_dev_k_1 = 0;
  //          PID_Compute_angle.err_int_k_1 = 0;
  //          PID_Compute_angle.err_k_1=0;
  //          PID_Compute_angle.t_k_1=0;
  //          PID_speed.SP = 0;
  //          break;
  //        }
  //      }
  // //后续三个按键先不操作
  //
  //    }
  //
  //   if (now - tick4 >= 10) {
  //     //10ms执行一次设定的速度和当前速度
  //     tick4 = now;
  //     BlueTooth_Receive();//对接受到的数据包进行处理
  //     //OLED_Clear();
  //     //显示PID三个参数
  //     OLED_Printf(0, 0, OLED_6X8, "Kp:%.3f",  PID_angle.Kp); //左轮的rpm
  //     OLED_Printf(0, 16, OLED_6X8, "Ki:%.3f", PID_angle.Ki); //右轮的rpm
  //     OLED_Printf(64, 16, OLED_6X8, "Kd:%.3f", PID_angle.Kd); //右轮的rpm
  //
  //     // OLED_Printf(64, 0, OLED_6X8, "Kp:%.1f",  PID_right.Kp); //左轮的rpm
  //     // OLED_Printf(64, 16, OLED_6X8, "Ki:%.1f", PID_right.Ki); //右轮的rpm
  //     // OLED_Printf(64, 32, OLED_6X8, "Kd:%.1f", PID_right.Kd); //右轮的rpm
  //     //俯仰角
  //
  //     OLED_Printf(0, 32, OLED_6X8, "angle:%.2f", FB_angle);
  //     OLED_Update();
  //
  //   }





    /*MPU6050测试*/
    // MPU6050_Update(&data_t);
    // MPU6050_Calculate(&data_t,&data_f);
    // //MPU6050_GetEuler_a(&data_f, &data_euler);
    // MPU6050_GetEulerComplementary(&data_f,&data_euler);
    // //snprintf(str,sizeof(str),"%.4f\r\n",data_f.GyroY);//零漂校准
    // snprintf(str,sizeof(str),"%.4f\r\n",GetPitchAngle(&data_euler));//零漂校准
    // HAL_UART_Transmit(&huart1,str,strlen(str),100);//pitch平放是为0度





/*速度环测试PID,目前计算得到的是占空比PWM*/
//速度环设定SP的范围为[-800,800],对应到摇杆上-100,100
    //该测试在it中开启速度环
    // static int16_t SP_left,SP_right;//上限设置为800
    // if (key1.press_event == 1) {//表示key1被按下，正转100
    //   key1.press_event =0;
    //   SP_left+=100;
    //   if (SP_left >= 800) SP_left = 800;
    //   PID_SetSP(&PID_left,-SP_left);
    //   //__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,3599);
    // }else if (key2.press_event == 1) {
    //   key2.press_event =0;
    //   SP_left-=100;
    //   if (SP_left < -800) SP_left = -800;
    //   PID_SetSP(&PID_left,-SP_left);
    // }else if (key3.press_event == 1) {
    //   key3.press_event =0;
    //   SP_right+=100;
    //   if (SP_right >= 800) SP_right = 800;
    //   PID_SetSP(&PID_right,SP_right);
    //
    // }else if (key4.press_event == 1) {
    //   key4.press_event =0;
    //   SP_right-=100;
    //   if (SP_right <= -800) SP_right = -800;
    //   PID_SetSP(&PID_right,SP_right);
    // }
    //
    //
    //
    // //显示PID是如何设置CCR的
    // OLED_Printf(0, 32,OLED_6X8, "SPL:%4d", SP_left); //左轮的SP
    // OLED_Printf(0, 48,OLED_6X8, "SPR:%4d", SP_right); //右轮的SP
    //
    // /*电机速度显示*/
    // SpeedL =Encoder_GetSpeed(Motor_left);
    // // if (SpeedL ==0) SpeedL =0;
    // SpeedR =Encoder_GetSpeed(Motor_right);
    // char str1[48];
    // snprintf(str1,30,"SL:%7.2f\r\n",SpeedL);
    //
    // HAL_UART_Transmit(&huart1,(uint8_t*)str1,strlen(str1),100);
    // OLED_Printf(0, 48,OLED_6X8, "L:%6.1f", SpeedL); //左轮的rpm
    // OLED_Printf(64, 48,OLED_6X8, "R:%6.1f", SpeedR); //右轮的rpm
    // OLED_Update();




 //电机死区判断
     // static int16_t SP_left,SP_right;//上限设置为800
     // if (key1.press_event == 1) {//表示key1被按下，正转100
     //   key1.press_event =0;
     //   SP_left+=1;//1-100
     //   if (SP_left >= 100) SP_left = 100;
     //   Motor_SetPWM(Motor_left,-SP_left);
     //
     // }else if (key2.press_event == 1) {
     //   key2.press_event =0;
     //   SP_left-=1;//1-100
     //   if (SP_left <= -100) SP_left = -100;
     //   Motor_SetPWM(Motor_left,-SP_left);
     // }else if (key3.press_event == 1) {
     //   key3.press_event =0;
     //   SP_right+=1;//1-100
     //   if (SP_right >= 100) SP_right = 100;
     //   Motor_SetPWM(Motor_right,SP_right);
     //
     // }else if (key4.press_event == 1) {
     //   key4.press_event =0;
     //   SP_right-=1;//1-100
     //   if (SP_right <= -100) SP_right = -100;
     //   Motor_SetPWM(Motor_right,SP_right);
     // }
     // SpeedL =Encoder_GetSpeed(Motor_left);
     // // if (SpeedL ==0) SpeedL =0;
     // SpeedR =Encoder_GetSpeed(Motor_right);
     // OLED_Printf(0, 0,OLED_6X8, "SPL:%4d", SP_left); //左轮的SP
     // OLED_Printf(0, 16,OLED_6X8, "SPR:%4d", SP_right); //右轮的SP
     // OLED_Printf(0, 32,OLED_6X8, "L:%6.1f", SpeedL); //左轮的rpm
     // OLED_Printf(0, 48,OLED_6X8, "R:%6.1f", SpeedR); //右轮的rpm
     // OLED_Update();


/*按键测试,直接电平测试*/
    //
    // OLED_Printf(0, 0,OLED_8X16, "K1:%d",  key1.press_event); //左轮的SP
    // OLED_Printf(0, 16,OLED_8X16, "K2:%d", key2.press_event); //右轮的SP
    // OLED_Printf(0, 32,OLED_8X16, "K3:%d", key3.press_event ); //左轮的rpm
    // OLED_Printf(0, 48,OLED_8X16, "K4:%d", key4.press_event); //右轮的rpm
    // OLED_Update();


/*摇杆测试速度和转向控制*/
    //显示摇杆的移动参数OLED_6X8
    // OLED_Printf(64, 0, OLED_6X8,  "LX:%4d",  PID_joystick.LeftX); //左轮的rpm
    // OLED_Printf(64, 16,OLED_6X8, "LY:%4d", PID_joystick.LeftY); //右轮的rpm
    // OLED_Printf(64, 32,OLED_6X8, "RX:%4d", PID_joystick.RightX); //右轮的rpm
    // OLED_Printf(64, 48,OLED_6X8, "RY:%4d", PID_joystick.RightY); //右轮的rpm





/*不需要时间片，直接调度*/
//角度环，读取速度，蓝牙数据包，OLED显示


    if (flags.flag_angle == 1) {//角度环
      flags.flag_angle =0;

      MPU6050_Update(&data_t);
      MPU6050_Calculate(&data_t, &data_f);
      MPU6050_GetEulerComplementary(&data_f, &data_euler);

      FB_angle = GetPitchAngle(&data_euler);//得到反馈角

      if (PID_angle_flag) {//按键状态为1，表示开启角度环
        PWM_AVG = PID_Compute(
            &PID_angle,
            &PID_Compute_angle,
            FB_angle
        );

        PWM_Left = PWM_AVG;
        PWM_Right = PWM_AVG;

        Motor_SetPWM(Motor_left, PWM_Left);
        Motor_SetPWM(Motor_right, -PWM_Right);
      }
      else {//关闭角度环
        Motor_SetPWM(Motor_left , 0);
        Motor_SetPWM(Motor_right, 0);
      }



    }
    if (flags.flag_speed == 1) {
      flags.flag_speed =0;
      //读取速度
      CNTL = -Encoder_GetCounter(Motor_left);
      CNTR = Encoder_GetCounter(Motor_right);


    }
    if (flags.flag_bluetooth == 1) {

      flags.flag_bluetooth =0;

      BlueTooth_Receive();//对接受到的数据包进行处理
      // PID_speed.SP =(float)((PID_joystick.LeftY) * 8);//范围-800 800
      // PID_direct.SP =(float)(PID_joystick.RightY);//转向环参数多少关系目前还未找到

    }
    if (flags.flag_key == 1) {
      flags.flag_key =0;
      Key_FSMUpdate(&key1, GPIOB, Button_K1_Pin);
      Key_FSMUpdate(&key2, GPIOB, Button_K2_Pin);
      Key_FSMUpdate(&key3, GPIOA, Button_K3_Pin);
      Key_FSMUpdate(&key4, GPIOA, Button_K4_Pin);

      static uint8_t i = 0;
      if (key1.press_event == 1) {//按键1按下，开启角度环
        key1.press_event = 0;

        i++;

        if (i % 2 == 1 ) {//奇数次按下
          PID_angle_flag = 1;
        }else {
          PID_angle_flag = 0;
        }
      }
      if (GetPitchAngle(&data_euler) >50 || GetPitchAngle(&data_euler) <-50) {//范围是-90到90度
        PID_angle_flag = 0;
        i=0;
      }

      switch (PID_angle_flag) {
        case 1://按下之后，角度环开启 留
        {
          PID_angle.SP = 0.25;
          LED_ON();
          break;
        }
        case 0:
        {
          LED_OFF();

          PID_Compute_angle.err_dev_k_1 = 0;
          PID_Compute_angle.err_int_k_1 = 0;
          PID_Compute_angle.err_k_1=0;
          PID_Compute_angle.t_k_1=0;
          PID_speed.SP = 0;
          break;
        }
      }


    }
    if (flags.flag_oled == 1) {

      flags.flag_oled =0;


      //显示PID三个参数
      OLED_Printf(0, 0, OLED_6X8, "Kp:%.3f",  PID_angle.Kp);
      OLED_Printf(0, 16, OLED_6X8, "Ki:%.3f", PID_angle.Ki);
      OLED_Printf(64, 16, OLED_6X8, "Kd:%.3f", PID_angle.Kd);

      // OLED_Printf(64, 0, OLED_6X8, "Kp:%.1f",  PID_right.Kp); //左轮的rpm
      // OLED_Printf(64, 16, OLED_6X8, "Ki:%.1f", PID_right.Ki); //右轮的rpm
      // OLED_Printf(64, 32, OLED_6X8, "Kd:%.1f", PID_right.Kd); //右轮的rpm
      //俯仰角

      OLED_Printf(0, 32, OLED_6X8, "angle:%.2f", FB_angle);
      OLED_Update();

    }


















    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
