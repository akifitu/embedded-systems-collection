#include "pid_controller.h"

static float clamp_float(float value, float min_value, float max_value) {
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

void pid_controller_init(pid_controller_t *pid) {
    pid->kp = 2.8f;
    pid->ki = 0.06f;
    pid->kd = 7.0f;
    pid->integral = 0.0f;
    pid->prev_measured_c = 0.0f;
    pid->initialized = 0;
}

void pid_controller_reset(pid_controller_t *pid) {
    pid->integral = 0.0f;
    pid->prev_measured_c = 0.0f;
    pid->initialized = 0;
}

uint8_t pid_controller_step(pid_controller_t *pid, reflow_stage_t stage,
                            float target_c, float measured_c) {
    float base_output;
    float derivative;
    float error;
    float output;

    if (stage == REFLOW_STAGE_COOL || stage == REFLOW_STAGE_COMPLETE ||
        stage == REFLOW_STAGE_FAULT || stage == REFLOW_STAGE_IDLE) {
        pid_controller_reset(pid);
        return 0u;
    }

    if (!pid->initialized) {
        pid->prev_measured_c = measured_c;
        pid->initialized = 1;
    }

    error = target_c - measured_c;
    derivative = measured_c - pid->prev_measured_c;
    pid->prev_measured_c = measured_c;
    pid->integral = clamp_float(pid->integral + error, -700.0f, 700.0f);

    base_output = 0.0f;
    if (stage == REFLOW_STAGE_PREHEAT) {
        base_output = 30.0f;
    } else if (stage == REFLOW_STAGE_SOAK) {
        base_output = 32.0f;
    } else if (stage == REFLOW_STAGE_REFLOW) {
        base_output = 40.0f;
    }

    output = base_output + (pid->kp * error) + (pid->ki * pid->integral) -
             (pid->kd * derivative);

    if (stage == REFLOW_STAGE_SOAK) {
        output = clamp_float(output, 0.0f, 85.0f);
    } else if (stage == REFLOW_STAGE_REFLOW && measured_c >= 220.0f) {
        output = clamp_float(output, 0.0f, 75.0f);
    } else {
        output = clamp_float(output, 0.0f, 100.0f);
    }

    return (uint8_t)(output + 0.5f);
}
