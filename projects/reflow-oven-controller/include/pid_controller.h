#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdint.h>

#include "reflow_types.h"

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral;
    float prev_measured_c;
    int initialized;
} pid_controller_t;

void pid_controller_init(pid_controller_t *pid);
void pid_controller_reset(pid_controller_t *pid);
uint8_t pid_controller_step(pid_controller_t *pid, reflow_stage_t stage,
                            float target_c, float measured_c);

#endif
