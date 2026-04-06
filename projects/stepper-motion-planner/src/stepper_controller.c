#include "stepper_controller.h"

#include <stdbool.h>

enum {
    CONTROL_TICK_MS = 10,
    HOMING_RATE_SPS = 1200,
    MAX_MOVE_RATE_SPS = 4800,
    MOVE_ACCEL_SPS2 = 24000,
    HOME_TIMEOUT_MARGIN_TICKS = 80,
    STALL_THRESHOLD_TICKS = 12
};

static int32_t abs_i32(int32_t value) {
    return value < 0 ? -value : value;
}

static uint8_t clamp_progress(uint32_t value) {
    if (value > 100u) {
        return 100u;
    }
    return (uint8_t)value;
}

static uint32_t allowed_home_ticks(const stepper_controller_t *controller) {
    uint32_t step_per_tick = (uint32_t)((HOMING_RATE_SPS * CONTROL_TICK_MS) / 1000);
    uint32_t distance_steps = (uint32_t)abs_i32(controller->homing_start_steps) + 24u;

    if (step_per_tick == 0u) {
        return HOME_TIMEOUT_MARGIN_TICKS;
    }

    return (distance_steps + step_per_tick - 1u) / step_per_tick +
           HOME_TIMEOUT_MARGIN_TICKS;
}

static stepper_output_t compose_output(const stepper_controller_t *controller) {
    stepper_output_t output;
    uint8_t progress = 0u;
    stepper_direction_t direction = STEPPER_DIR_STOP;

    output.state = controller->state;
    output.position_steps = controller->position_steps;
    output.target_steps = controller->target_steps;
    output.step_rate_sps = (uint32_t)(controller->commanded_rate_sps + 0.5f);
    output.driver_enable =
        controller->state == STEPPER_STATE_HOMING ||
        controller->state == STEPPER_STATE_MOVING;
    output.faults = controller->faults;

    if (controller->state == STEPPER_STATE_HOMING) {
        int32_t start = abs_i32(controller->homing_start_steps);
        int32_t moved = abs_i32(controller->homing_start_steps -
                                controller->position_steps);

        if (controller->position_steps < controller->target_steps) {
            direction = STEPPER_DIR_FWD;
        } else if (controller->position_steps > controller->target_steps) {
            direction = STEPPER_DIR_REV;
        }

        if (start > 0) {
            progress = clamp_progress((uint32_t)((moved * 100) / start));
        }
    } else if (controller->state == STEPPER_STATE_MOVING) {
        int32_t total = abs_i32(controller->plan.target_steps -
                                controller->plan.start_steps);
        int32_t traveled = abs_i32(controller->position_steps -
                                   controller->plan.start_steps);

        if (controller->position_steps < controller->target_steps) {
            direction = STEPPER_DIR_FWD;
        } else if (controller->position_steps > controller->target_steps) {
            direction = STEPPER_DIR_REV;
        }

        if (total > 0) {
            progress = clamp_progress((uint32_t)((traveled * 100) / total));
        }
    } else if (controller->state == STEPPER_STATE_READY) {
        progress = 100u;
    } else if (controller->state == STEPPER_STATE_FAULT) {
        int32_t total = abs_i32(controller->plan.target_steps -
                                controller->plan.start_steps);
        int32_t traveled = abs_i32(controller->position_steps -
                                   controller->plan.start_steps);

        if (total > 0) {
            progress = clamp_progress((uint32_t)((traveled * 100) / total));
        }
    }

    output.direction = direction;
    output.progress_percent = progress;
    return output;
}

static void enter_fault(stepper_controller_t *controller, uint32_t fault) {
    controller->faults |= fault;
    controller->state = STEPPER_STATE_FAULT;
    controller->commanded_rate_sps = 0.0f;
    controller->step_accumulator = 0.0f;
}

static void apply_position_step(stepper_controller_t *controller, int direction_sign,
                                bool stalled) {
    uint32_t whole_steps;
    int32_t remaining;

    controller->step_accumulator +=
        controller->commanded_rate_sps * ((float)CONTROL_TICK_MS / 1000.0f);
    whole_steps = (uint32_t)controller->step_accumulator;
    controller->step_accumulator -= (float)whole_steps;

    if (whole_steps == 0u || stalled) {
        return;
    }

    remaining = abs_i32(controller->target_steps - controller->position_steps);
    if ((uint32_t)remaining <= whole_steps) {
        controller->position_steps = controller->target_steps;
        controller->step_accumulator = 0.0f;
    } else {
        controller->position_steps += direction_sign * (int32_t)whole_steps;
    }
}

void stepper_controller_init(stepper_controller_t *controller,
                             int32_t initial_position_steps) {
    controller->state = STEPPER_STATE_IDLE;
    controller->faults = STEPPER_FAULT_NONE;
    controller->position_steps = initial_position_steps;
    controller->target_steps = initial_position_steps;
    controller->homing_start_steps = initial_position_steps;
    controller->step_accumulator = 0.0f;
    controller->commanded_rate_sps = 0.0f;
    controller->move_ticks = 0u;
    controller->home_ticks = 0u;
    controller->stall_ticks = 0u;
    trajectory_plan_build(&controller->plan, initial_position_steps,
                          initial_position_steps, (float)MAX_MOVE_RATE_SPS,
                          (float)MOVE_ACCEL_SPS2);
}

stepper_output_t stepper_controller_step(stepper_controller_t *controller,
                                         const stepper_command_t *command,
                                         const stepper_feedback_t *feedback) {
    if (command->clear_fault) {
        controller->faults = STEPPER_FAULT_NONE;
        controller->state = STEPPER_STATE_IDLE;
        controller->commanded_rate_sps = 0.0f;
        controller->step_accumulator = 0.0f;
        controller->move_ticks = 0u;
        controller->stall_ticks = 0u;
    }

    if (command->start_homing && controller->faults == STEPPER_FAULT_NONE &&
        controller->state != STEPPER_STATE_HOMING) {
        controller->state = STEPPER_STATE_HOMING;
        controller->homing_start_steps = controller->position_steps;
        controller->target_steps = 0;
        controller->commanded_rate_sps = (float)HOMING_RATE_SPS;
        controller->home_ticks = 0u;
        controller->step_accumulator = 0.0f;
    }

    if (command->move_request && controller->faults == STEPPER_FAULT_NONE &&
        controller->state == STEPPER_STATE_READY &&
        command->target_steps != controller->position_steps) {
        controller->target_steps = command->target_steps;
        trajectory_plan_build(&controller->plan, controller->position_steps,
                              controller->target_steps,
                              (float)MAX_MOVE_RATE_SPS,
                              (float)MOVE_ACCEL_SPS2);
        controller->state = STEPPER_STATE_MOVING;
        controller->commanded_rate_sps = 0.0f;
        controller->move_ticks = 0u;
        controller->stall_ticks = 0u;
        controller->step_accumulator = 0.0f;
    }

    switch (controller->state) {
        case STEPPER_STATE_HOMING:
            controller->home_ticks++;
            if (feedback->home_switch) {
                controller->position_steps = 0;
                controller->target_steps = 0;
                controller->commanded_rate_sps = 0.0f;
                controller->step_accumulator = 0.0f;
                controller->state = STEPPER_STATE_READY;
            } else if (controller->home_ticks > allowed_home_ticks(controller)) {
                enter_fault(controller, STEPPER_FAULT_HOME_TIMEOUT);
            } else {
                controller->commanded_rate_sps = (float)HOMING_RATE_SPS;
                controller->position_steps -=
                    (int32_t)((controller->commanded_rate_sps *
                               (float)CONTROL_TICK_MS) /
                              1000.0f);
            }
            break;
        case STEPPER_STATE_MOVING: {
            int direction_sign =
                controller->target_steps > controller->position_steps ? 1 : -1;

            controller->move_ticks++;
            if ((direction_sign > 0 && feedback->max_limit_switch) ||
                (direction_sign < 0 && feedback->min_limit_switch)) {
                enter_fault(controller, STEPPER_FAULT_LIMIT_HIT);
                break;
            }

            controller->commanded_rate_sps =
                trajectory_plan_rate(&controller->plan,
                                     controller->position_steps);
            apply_position_step(controller, direction_sign, feedback->stalled);

            if (feedback->stalled) {
                controller->stall_ticks++;
            } else {
                controller->stall_ticks = 0u;
            }

            if (controller->stall_ticks >= STALL_THRESHOLD_TICKS) {
                enter_fault(controller, STEPPER_FAULT_STALL);
                break;
            }

            if (controller->position_steps == controller->target_steps) {
                controller->commanded_rate_sps = 0.0f;
                controller->step_accumulator = 0.0f;
                controller->state = STEPPER_STATE_READY;
                break;
            }

            if (controller->move_ticks > controller->plan.estimated_ticks + 60u) {
                enter_fault(controller, STEPPER_FAULT_TARGET_TIMEOUT);
            }
            break;
        }
        case STEPPER_STATE_IDLE:
        case STEPPER_STATE_READY:
        case STEPPER_STATE_FAULT:
            controller->commanded_rate_sps = 0.0f;
            break;
    }

    return compose_output(controller);
}
