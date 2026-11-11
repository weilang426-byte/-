//
// Created by LWadmin on 2026/9/15.
//

#ifndef PROJECT_BALANCECAR_ANGLELOOP_H
#define PROJECT_BALANCECAR_ANGLELOOP_H

#include "main.h"
#define BALANCE_TARGET_DEG 0.25f
#define BALANCE_ARM_DEG 10.0f
#define BALANCE_ARM_RATE_DPS 20.0f
#define BALANCE_TRIP_DEG 50.0f
#define BALANCE_MIN_DT_S 0.005f
#define BALANCE_MAX_DT_S 0.015f
#define BALANCE_WATCHDOG_MS 25U
#define BALANCE_FILTER_TAU_S 0.99f
#define BALANCE_RATE_TAU_S 0.01f
typedef enum {
    BALANCE_OK = 0, BALANCE_INIT_ERROR, BALANCE_CALIBRATION_ERROR,
    BALANCE_I2C_ERROR, BALANCE_TIMING_ERROR, BALANCE_FALLEN,
    BALANCE_INVALID_DATA, BALANCE_CONTROL_TIMEOUT
} BalanceFault;
typedef struct {
    uint32_t samples, i2c_errors, timing_errors, watchdog_trips, last_good_ms;
    float dt_ms, max_dt_ms, exec_ms, max_exec_ms;
    float angle_deg, rate_dps, p_term, i_term, d_term, output;
    uint32_t saturation_samples;
    BalanceFault fault;
} BalanceDiagnostics;
extern volatile BalanceDiagnostics balance_diag;
void Balance_Init(void);
void Balance_Update(void);  /* Main context only: blocking I2C must not run in ISR. */
void Balance_Toggle(void);
void Balance_Stop(BalanceFault fault);
void Balance_WatchdogTick(void);

#endif //PROJECT_BALANCECAR_ANGLELOOP_H
