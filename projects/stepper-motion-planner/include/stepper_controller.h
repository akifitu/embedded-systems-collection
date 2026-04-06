#ifndef STEPPER_CONTROLLER_H
#define STEPPER_CONTROLLER_H

#include <stdint.h>

#include "motion_types.h"
#include "trajectory_planner.h"

typedef struct {
    stepper_state_t state;
    uint32_t faults;
    int32_t position_steps;
    int32_t target_steps;
    int32_t homing_start_steps;
    float step_accumulator;
    float commanded_rate_sps;
    uint32_t move_ticks;
    uint32_t home_ticks;
    uint32_t stall_ticks;
    trajectory_plan_t plan;
} stepper_controller_t;

void stepper_controller_init(stepper_controller_t *controller,
                             int32_t initial_position_steps);
stepper_output_t stepper_controller_step(stepper_controller_t *controller,
                                         const stepper_command_t *command,
                                         const stepper_feedback_t *feedback);

#endif
