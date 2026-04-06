#include "startup_controller.h"

#include "commutation_table.h"

enum {
    ALIGN_TICKS = 12,
    OPEN_LOOP_START_PERIOD_US = 3200,
    OPEN_LOOP_MIN_PERIOD_US = 1100,
    OPEN_LOOP_PERIOD_STEP_US = 100,
    OPEN_LOOP_TIMEOUT_TICKS = 28,
    LOCK_DETECTION_EVENTS = 3,
    STALL_TIMEOUT_TICKS = 8,
    OVERCURRENT_LIMIT_MA = 7800
};

static uint8_t clamp_u8(uint8_t value, uint8_t min_value, uint8_t max_value) {
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static void reset_dynamic_fields(startup_controller_t *controller) {
    controller->align_ticks = 0u;
    controller->open_loop_ticks = 0u;
    controller->zero_cross_events = 0u;
    controller->missed_zero_cross_ticks = 0u;
}

static void start_sequence(startup_controller_t *controller,
                           const bldc_command_t *command) {
    controller->state = BLDC_STATE_ALIGN;
    controller->sector = BLDC_SECTOR_S1;
    controller->lock = BLDC_LOCK_SEEKING;
    controller->duty_percent = clamp_u8((uint8_t)(command->throttle_percent / 2u),
                                        18u, 28u);
    controller->commutation_period_us = 0u;
    controller->estimated_rpm = 0u;
    controller->faults = BLDC_FAULT_NONE;
    reset_dynamic_fields(controller);
}

static void enter_fault(startup_controller_t *controller, uint32_t fault) {
    controller->state = BLDC_STATE_FAULT;
    controller->lock = BLDC_LOCK_LOST;
    controller->faults |= fault;
    controller->duty_percent = 0u;
    controller->commutation_period_us = 0u;
    controller->estimated_rpm = 0u;
}

static bldc_output_t compose_output(const startup_controller_t *controller) {
    bldc_output_t output;

    output.state = controller->state;
    output.sector = controller->sector;
    output.lock = controller->lock;
    output.duty_percent = controller->duty_percent;
    output.commutation_period_us = controller->commutation_period_us;
    output.estimated_rpm = controller->estimated_rpm;
    output.faults = controller->faults;
    return output;
}

void startup_controller_init(startup_controller_t *controller) {
    controller->state = BLDC_STATE_IDLE;
    controller->sector = BLDC_SECTOR_S1;
    controller->lock = BLDC_LOCK_NONE;
    controller->duty_percent = 0u;
    controller->commutation_period_us = 0u;
    controller->estimated_rpm = 0u;
    controller->faults = BLDC_FAULT_NONE;
    reset_dynamic_fields(controller);
}

bldc_output_t startup_controller_step(startup_controller_t *controller,
                                      const bldc_command_t *command,
                                      const bldc_observation_t *observation) {
    if (command->clear_fault) {
        startup_controller_init(controller);
    }

    if (command->start && controller->state == BLDC_STATE_IDLE) {
        start_sequence(controller, command);
    }

    if (controller->state != BLDC_STATE_IDLE &&
        controller->state != BLDC_STATE_FAULT &&
        observation->phase_current_ma >= OVERCURRENT_LIMIT_MA) {
        enter_fault(controller, BLDC_FAULT_OVERCURRENT);
        return compose_output(controller);
    }

    switch (controller->state) {
        case BLDC_STATE_IDLE:
            break;
        case BLDC_STATE_ALIGN:
            controller->align_ticks++;
            controller->duty_percent =
                clamp_u8((uint8_t)(command->throttle_percent / 2u), 18u, 28u);
            controller->estimated_rpm = 0u;
            if (controller->align_ticks >= ALIGN_TICKS) {
                controller->state = BLDC_STATE_OPEN_LOOP;
                controller->commutation_period_us = OPEN_LOOP_START_PERIOD_US;
                controller->duty_percent = clamp_u8(
                    (uint8_t)(command->throttle_percent - 8u), 20u, 55u);
                controller->open_loop_ticks = 0u;
                controller->zero_cross_events = 0u;
            }
            break;
        case BLDC_STATE_OPEN_LOOP:
            controller->open_loop_ticks++;
            controller->sector = commutation_next_sector(controller->sector);
            if (controller->commutation_period_us > OPEN_LOOP_MIN_PERIOD_US) {
                controller->commutation_period_us -= OPEN_LOOP_PERIOD_STEP_US;
                if (controller->commutation_period_us < OPEN_LOOP_MIN_PERIOD_US) {
                    controller->commutation_period_us = OPEN_LOOP_MIN_PERIOD_US;
                }
            }
            controller->duty_percent = clamp_u8(
                (uint8_t)(command->throttle_percent - 8u), 20u, 55u);
            controller->estimated_rpm =
                observation->measured_rpm != 0u
                    ? observation->measured_rpm
                    : commutation_estimated_rpm(controller->commutation_period_us);

            if (observation->zero_cross) {
                controller->zero_cross_events++;
            }

            if (controller->zero_cross_events >= LOCK_DETECTION_EVENTS &&
                controller->estimated_rpm >= 900u) {
                controller->state = BLDC_STATE_CLOSED_LOOP;
                controller->lock = BLDC_LOCK_LOCKED;
                controller->duty_percent =
                    clamp_u8(command->throttle_percent, 24u, 70u);
                controller->missed_zero_cross_ticks = 0u;
                if (observation->measured_rpm != 0u) {
                    controller->commutation_period_us =
                        commutation_period_from_rpm(observation->measured_rpm);
                    controller->estimated_rpm = observation->measured_rpm;
                }
            } else if (controller->open_loop_ticks > OPEN_LOOP_TIMEOUT_TICKS) {
                enter_fault(controller, BLDC_FAULT_NO_LOCK);
            }
            break;
        case BLDC_STATE_CLOSED_LOOP:
            controller->duty_percent =
                clamp_u8(command->throttle_percent, 24u, 80u);

            if (observation->zero_cross) {
                controller->sector = commutation_next_sector(controller->sector);
                controller->missed_zero_cross_ticks = 0u;
                if (observation->measured_rpm != 0u) {
                    controller->commutation_period_us =
                        commutation_period_from_rpm(observation->measured_rpm);
                    controller->estimated_rpm = observation->measured_rpm;
                } else {
                    controller->estimated_rpm =
                        commutation_estimated_rpm(controller->commutation_period_us);
                }
            } else {
                controller->missed_zero_cross_ticks++;
                controller->estimated_rpm =
                    commutation_estimated_rpm(controller->commutation_period_us);
                if (controller->missed_zero_cross_ticks > STALL_TIMEOUT_TICKS) {
                    enter_fault(controller, BLDC_FAULT_STALL);
                }
            }
            break;
        case BLDC_STATE_FAULT:
            break;
    }

    return compose_output(controller);
}
