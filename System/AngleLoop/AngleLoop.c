//
// Created by LWadmin on 2026/9/15.
//
#include "AngleLoop.h"
#include "MPU6050.h"
#include "Motor.h"
#include "PID.h"
#include <math.h>

extern PID_Base_variables PID_angle;
extern volatile uint8_t PID_angle_flag;
extern MPU6050_Data_t data_t;
extern MPU6050_Data_f data_f;
extern MPU6050_Euler data_euler;
extern volatile float FB_angle, PWM_AVG;
extern float PWM_Left, PWM_Right;
volatile BalanceDiagnostics balance_diag;
static uint8_t sensor_ready, filter_ready, sample_clock_ready, healthy_samples;
static uint32_t previous_cycles;
static volatile uint32_t last_output_ms;
static float integral, previous_error, filtered_rate, pitch;

static float clampf(float value, float low, float high)
{ return fminf(high, fmaxf(low, value)); }

void Balance_Stop(BalanceFault fault)
{
    uint32_t saved = __get_PRIMASK();
    __disable_irq();
    PID_angle_flag = 0;
    Motor_EmergencyStop();
    if (fault != BALANCE_OK) balance_diag.fault = fault;
    __set_PRIMASK(saved);
}

void Balance_Init(void)
{
    Balance_Stop(BALANCE_OK);
    balance_diag = (BalanceDiagnostics){0};
    sensor_ready = filter_ready = sample_clock_ready = healthy_samples = 0;
    integral = previous_error = filtered_rate = pitch = 0.0f;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    if (MPU6050_Init() != HAL_OK) {
        Balance_Stop(BALANCE_INIT_ERROR); return;
    }
    if (MPU6050_CalibrateGyro() != HAL_OK) {
        Balance_Stop(BALANCE_CALIBRATION_ERROR); return;
    }
    sensor_ready = 1;
}

void Balance_WatchdogTick(void)
{
    if (PID_angle_flag &&
        (uint32_t)(HAL_GetTick() - last_output_ms) > BALANCE_WATCHDOG_MS) {
        balance_diag.watchdog_trips++;
        Balance_Stop(BALANCE_CONTROL_TIMEOUT);
    }
}

void Balance_Toggle(void)
{
    if (PID_angle_flag) { Balance_Stop(BALANCE_OK); return; }
    if (!sensor_ready || healthy_samples < 5 || !isfinite(FB_angle) ||
        fabsf(FB_angle - BALANCE_TARGET_DEG) > BALANCE_ARM_DEG ||
        fabsf(filtered_rate) > BALANCE_ARM_RATE_DPS ||
        (uint32_t)(HAL_GetTick() - balance_diag.last_good_ms) > 15U) return;
    integral = 0.0f;
    previous_error = BALANCE_TARGET_DEG - FB_angle;
    PID_angle.SP = BALANCE_TARGET_DEG;
    uint32_t saved = __get_PRIMASK();
    __disable_irq();
    balance_diag.fault = BALANCE_OK;
    balance_diag.max_dt_ms = balance_diag.dt_ms;
    balance_diag.max_exec_ms = 0.0f;
    last_output_ms = HAL_GetTick();
    PID_angle_flag = 1;
    __set_PRIMASK(saved);
}

void Balance_Update(void)
{
    if (!sensor_ready) return;
    const uint32_t started = DWT->CYCCNT;
    if (MPU6050_Update(&data_t) != HAL_OK) {
        balance_diag.i2c_errors++;
        healthy_samples = filter_ready = sample_clock_ready = 0;
        Balance_Stop(BALANCE_I2C_ERROR); return;
    }
    /* Timestamp completed samples; use the same dt for fusion and integral. */
    const uint32_t sampled = DWT->CYCCNT;
    const float clock_hz = (float)SystemCoreClock;
    float dt = (uint32_t)(sampled - previous_cycles) / clock_hz;
    previous_cycles = sampled;
    if (!sample_clock_ready) { dt = 0.01f; sample_clock_ready = 1; }
    balance_diag.dt_ms = dt * 1000.0f;
    if (balance_diag.dt_ms > balance_diag.max_dt_ms)
        balance_diag.max_dt_ms = balance_diag.dt_ms;
    if (!isfinite(dt) || dt < BALANCE_MIN_DT_S || dt > BALANCE_MAX_DT_S) {
        /* Display stalls while disarmed are allowed; discard this sample interval. */
        if (PID_angle_flag) {
            balance_diag.timing_errors++;
            Balance_Stop(BALANCE_TIMING_ERROR);
        }
        healthy_samples = filter_ready = 0;
        return;
    }
    MPU6050_Calculate(&data_t, &data_f);
    const float norm2 = data_f.AccX * data_f.AccX + data_f.AccY * data_f.AccY
                      + data_f.AccZ * data_f.AccZ;
    if (!isfinite(norm2) || !isfinite(data_f.GyroY) || norm2 < 0.01f ||
        fabsf(data_f.GyroY) > 1900.0f ||
        (fabsf(data_f.AccX) + fabsf(data_f.AccZ)) < 0.05f) {
        healthy_samples = filter_ready = 0;
        Balance_Stop(BALANCE_INVALID_DATA); return;
    }
    const float acc_angle = -atan2f(data_f.AccX, data_f.AccZ) * 57.2957795f
                          - PITCH_ACC_ZERO_OFFSET;
    if (!filter_ready) {
        pitch = acc_angle;
        filtered_rate = data_f.GyroY;
        filter_ready = 1;
    } else {
        float alpha = BALANCE_FILTER_TAU_S / (BALANCE_FILTER_TAU_S + dt);
        /* Large acceleration is not a trustworthy gravity reference. */
        if (norm2 < 0.64f || norm2 > 1.44f) alpha = 1.0f;
        pitch = alpha * (pitch + data_f.GyroY * dt) + (1.0f - alpha) * acc_angle;
        filtered_rate += dt / (BALANCE_RATE_TAU_S + dt) * (data_f.GyroY - filtered_rate);
    }
    FB_angle = pitch + BALANCE_ANGLE_OFFSET;
    data_euler.pitch = FB_angle;
    balance_diag.angle_deg = FB_angle;
    balance_diag.rate_dps = filtered_rate;
    balance_diag.last_good_ms = HAL_GetTick();
    balance_diag.samples++;
    if (healthy_samples < 20) healthy_samples++;
    if (!isfinite(FB_angle) || fabsf(FB_angle) > BALANCE_TRIP_DEG) {
        healthy_samples = 0;
        Balance_Stop(BALANCE_FALLEN); return;
    }
    if (!PID_angle_flag) {
        integral = 0.0f;
        balance_diag.output = PWM_AVG = PWM_Left = PWM_Right = 0.0f;
        return;
    }
    if (!isfinite(PID_angle.Kp) || !isfinite(PID_angle.Ki) || !isfinite(PID_angle.Kd) ||
        PID_angle.Kp < 0.0f || PID_angle.Ki < 0.0f || PID_angle.Kd < 0.0f ||
        PID_angle.integral_max < 0.0f ||
        !isfinite(PID_angle.SP) || !isfinite(PID_angle.integral_max) ||
        !isfinite(PID_angle.outmax) || !isfinite(PID_angle.outmin) ||
        PID_angle.outmax <= 0.0f || PID_angle.outmax > 80.0f ||
        PID_angle.outmin >= 0.0f || PID_angle.outmin < -80.0f) {
        Balance_Stop(BALANCE_INVALID_DATA); return;
    }
    float error = PID_angle.SP - FB_angle;
    float p = PID_angle.Kp * error;
    /* D is negative feedback of calibrated, filtered degrees/second. */
    float d = -PID_angle.Kd * filtered_rate;
    float candidate = 0.0f;
    if (PID_angle.Ki != 0.0f) {
        float limit = fabsf(PID_angle.integral_max / PID_angle.Ki);
        candidate = clampf(integral + 0.5f * (error + previous_error) * dt, -limit, limit);
    }
    float proposed = p + PID_angle.Ki * candidate + d;
    float direction = PID_angle.Ki * error;
    if (!((proposed > PID_angle.outmax && direction > 0.0f) ||
          (proposed < PID_angle.outmin && direction < 0.0f))) integral = candidate;
    previous_error = error;
    float output = p + PID_angle.Ki * integral + d;
    if (!isfinite(output)) { Balance_Stop(BALANCE_INVALID_DATA); return; }
    if (output > PID_angle.outmax || output < PID_angle.outmin)
        balance_diag.saturation_samples++;
    output = clampf(output, PID_angle.outmin, PID_angle.outmax);
    balance_diag.p_term = p;
    balance_diag.i_term = PID_angle.Ki * integral;
    balance_diag.d_term = d;
    float elapsed = (uint32_t)(DWT->CYCCNT - started) / clock_hz * 1000.0f;
    balance_diag.exec_ms = elapsed;
    if (elapsed > balance_diag.max_exec_ms) balance_diag.max_exec_ms = elapsed;
    if (elapsed > 5.0f) {
        balance_diag.timing_errors++;
        Balance_Stop(BALANCE_TIMING_ERROR); return;
    }
    /* A watchdog trip during calculation must not be overwritten here. */
    uint32_t saved = __get_PRIMASK();
    __disable_irq();
    if (PID_angle_flag) {
        PWM_AVG = PWM_Left = PWM_Right = output;
        balance_diag.output = output;
        Motor_SetPWM(Motor_left, output);
        Motor_SetPWM(Motor_right, -output);
        last_output_ms = HAL_GetTick();
    }
    __set_PRIMASK(saved);
}
