#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "AngleLoop.h"
#include "MPU6050.h"
#include "Motor.h"
#include "BlueTooth.h"

DWT_Type host_dwt;
CoreDebug_Type host_debug;
TIM_TypeDef host_tim;
I2C_HandleTypeDef hi2c2;
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim2 = {&host_tim};
uint32_t SystemCoreClock = 72000000;
PID_Base_variables PID_angle, PID_left, PID_right;
volatile uint8_t PID_angle_flag, rx_flag, rx_data;
char buffer[BLUETOOTH_PACKET_LENGTH];
joystick PID_joystick;
MPU6050_Data_t data_t;
MPU6050_Data_f data_f;
MPU6050_Euler data_euler;
volatile float FB_angle, PWM_AVG;
float PWM_Left, PWM_Right;
extern volatile uint32_t bluetooth_rejected_packets;
static uint32_t tick, remainder_us;
static uint8_t registers[256];
static int fail_read, bad_id, moving, sample_count, trip_during_read;
static uint32_t read_us = 400;
static int pin_states[16];
static int16_t raw[7] = {21,0,2048,0,0,25,0};

static void advance(uint32_t us) {
    host_dwt.CYCCNT += us * 72U;
    remainder_us += us;
    tick += remainder_us / 1000U;
    remainder_us %= 1000U;
}
uint32_t HAL_GetTick(void) { return tick; }
void HAL_Delay(uint32_t ms) { advance(ms * 1000); }
void HAL_GPIO_WritePin(int port, int pin, int value) { (void)port; pin_states[pin] = value; }
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *h, uint32_t c)
{ (void)h; (void)c; return HAL_OK; }
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *h, uint8_t *p, uint16_t n)
{ (void)h; (void)p; (void)n; return HAL_OK; }
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *h, uint16_t a, uint16_t r,
 uint16_t size, uint8_t *p, uint16_t n, uint32_t timeout) {
    (void)h; (void)a; (void)size; (void)n; (void)timeout;
    registers[r] = *p; return HAL_OK;
}
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *h, uint16_t a, uint16_t r,
 uint16_t size, uint8_t *p, uint16_t n, uint32_t timeout) {
    (void)h; (void)a; (void)size; (void)timeout;
    advance(read_us);
    if (trip_during_read) { advance(30000); Balance_WatchdogTick(); }
    if (fail_read) { memset(p, 0xaa, n / 2); return HAL_TIMEOUT; }
    if (r == 0x75) { *p = bad_id ? 0 : 0x68; return HAL_OK; }
    if (n == 1) { *p = registers[r]; return HAL_OK; }
    sample_count++;
    for (unsigned i = 0; i < 7; ++i) {
        int16_t v = raw[i];
        if (moving && i == 5) v += (sample_count & 1) ? 200 : -200;
        p[i*2] = (uint16_t)v >> 8; p[i*2+1] = (uint8_t)v;
    }
    return HAL_OK;
}
static void cycle(void) { advance(9600); Balance_Update(); }
static void reset_ready(void) {
    fail_read = bad_id = moving = trip_during_read = 0; read_us = 400;
    raw[0]=21; raw[1]=0; raw[2]=2048; raw[5]=25;
    PID_angle = (PID_Base_variables){1.9f,0.0f,0.03f,0.25f,80,-80,15};
    Balance_Init(); assert(balance_diag.fault == BALANCE_OK);
    for (int i=0;i<6;i++) cycle();
    Balance_Toggle(); assert(PID_angle_flag);
}
static void packet(const char *text) {
    memset(buffer,0,sizeof(buffer));
    strncpy(buffer,text,sizeof(buffer)-1); rx_flag=1; BlueTooth_Receive();
    assert(!rx_flag);
}
int main(void) {
    reset_ready(); cycle();
    assert(fabsf(balance_diag.dt_ms-10.0f)<0.01f);
    assert(fabsf(mpu6050_gyro_bias_raw-25.0f)<0.01f);
    assert(PWM_Left == PWM_Right && host_tim.CCR1 == host_tim.CCR2);
    puts("PASS: startup calibration and 10 ms clock");
    PID_angle.SP=5; cycle();
    assert(pin_states[12]==1 && pin_states[13]==0 && pin_states[14]==0 && pin_states[15]==1);
    PID_angle.SP=-5; cycle();
    assert(pin_states[12]==0 && pin_states[13]==1 && pin_states[14]==1 && pin_states[15]==0);
    PID_angle.SP=0.25f;
    puts("PASS: both signed outputs preserve the original GPIO polarity");
    raw[5]=189; cycle(); assert(balance_diag.d_term<0);
    puts("PASS: pitch-rate damping sign");

    MPU6050_Data_t before = data_t;
    fail_read=1; cycle();
    assert(!PID_angle_flag && balance_diag.fault==BALANCE_I2C_ERROR);
    assert(memcmp(&before,&data_t,sizeof(data_t))==0);
    assert(host_tim.CCER==0 && host_tim.CCR1==0 && host_tim.CCR2==0);
    fail_read=0; for(int i=0;i<10;i++) cycle(); assert(!PID_angle_flag);
    puts("PASS: partial read cannot publish stale sample; PWM stops; no auto-arm");

    reset_ready(); advance(21000); Balance_Update();
    assert(!PID_angle_flag && balance_diag.fault==BALANCE_TIMING_ERROR);
    reset_ready(); advance(100); Balance_Update();
    assert(!PID_angle_flag && balance_diag.fault==BALANCE_TIMING_ERROR);
    puts("PASS: long/short intervals fail closed");

    reset_ready(); advance(30000); Balance_WatchdogTick();
    assert(!PID_angle_flag && balance_diag.fault==BALANCE_CONTROL_TIMEOUT && host_tim.CCER==0);
    reset_ready(); trip_during_read=1; cycle();
    assert(!PID_angle_flag && host_tim.CCER==0);
    trip_during_read=0;
    puts("PASS: watchdog stops stalled main; in-flight calculation cannot re-enable motor");

    reset_ready(); raw[0]=raw[1]=raw[2]=0; cycle();
    assert(!PID_angle_flag && balance_diag.fault==BALANCE_INVALID_DATA);
    reset_ready(); PID_angle.Kp=NAN; cycle();
    assert(!PID_angle_flag && balance_diag.fault==BALANCE_INVALID_DATA);
    reset_ready(); PID_angle.Kp=50; PID_angle.Ki=50; PID_angle.SP=10;
    for(int i=0;i<20;i++) cycle();
    assert(fabsf(balance_diag.output)<=80 && balance_diag.i_term==0);
    puts("PASS: invalid data/gains, output limit and anti-windup");

    reset_ready(); raw[5]=16409;
    for(int i=0;i<20 && PID_angle_flag;i++) cycle();
    assert(!PID_angle_flag && balance_diag.fault==BALANCE_FALLEN);
    reset_ready(); Balance_Stop(BALANCE_OK);
    raw[0]=2048; raw[2]=0;
    advance(30000); Balance_Update();
    for(int i=0;i<6;i++) cycle();
    Balance_Toggle(); assert(!PID_angle_flag);
    puts("PASS: fall protection and upright-only re-arm");

    reset_ready(); read_us=6000; advance(4000); Balance_Update();
    assert(!PID_angle_flag && balance_diag.fault==BALANCE_TIMING_ERROR);
    read_us=400;
    reset_ready(); host_dwt.CYCCNT=0xffff0000U; Balance_Update();
    for(int i=0;i<6;i++) cycle();
    Balance_Toggle(); assert(PID_angle_flag); cycle(); assert(PID_angle_flag);
    puts("PASS: execution deadline and unsigned cycle-counter wrap");

    Balance_Stop(BALANCE_OK); moving=1; Balance_Init();
    assert(balance_diag.fault==BALANCE_CALIBRATION_ERROR);
    moving=0; bad_id=1; Balance_Init();
    assert(balance_diag.fault==BALANCE_INIT_ERROR);
    puts("PASS: calibration movement and wrong sensor ID reject startup");

    reset_ready(); float old=PID_angle.Kp;
    packet("slider,Kp,4"); assert(PID_angle.Kp==old);
    Balance_Stop(BALANCE_OK);
    const char *bad[]={"","slider,Kp","slider,,2","slider,Kd,nan","slider,Kp,inf",
      "slider,Kp,2x","slider,Kd,3","slider,Kp,-1","joystick,1,2,3","joystick,101,0,0,0"};
    uint32_t rejected=bluetooth_rejected_packets;
    for(unsigned i=0;i<sizeof(bad)/sizeof(bad[0]);i++) packet(bad[i]);
    assert(bluetooth_rejected_packets==rejected+sizeof(bad)/sizeof(bad[0]));
    packet("slider,Kd,0.04"); assert(fabsf(PID_angle.Kd-0.04f)<1e-6f);
    packet("joystick,1,-2,3,-4"); assert(PID_joystick.LeftY==-2 && PID_joystick.RightY==-4);
    Motor_SetPWM(Motor_left,NAN); assert(host_tim.CCER==0);
    puts("PASS: invalid/live tuning rejected; valid packets accepted; NaN motor output stops");
    puts("All host fault-injection tests passed (not a hardware stability test).");
    return 0;
}
