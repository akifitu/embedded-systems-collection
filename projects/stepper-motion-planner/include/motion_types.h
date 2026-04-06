#ifndef MOTION_TYPES_H
#define MOTION_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    STEPPER_STATE_IDLE = 0,
    STEPPER_STATE_HOMING,
    STEPPER_STATE_READY,
    STEPPER_STATE_MOVING,
    STEPPER_STATE_FAULT
} stepper_state_t;

typedef enum {
    STEPPER_DIR_STOP = 0,
    STEPPER_DIR_FWD,
    STEPPER_DIR_REV
} stepper_direction_t;

enum {
    STEPPER_FAULT_NONE = 0u,
    STEPPER_FAULT_HOME_TIMEOUT = 1u << 0,
    STEPPER_FAULT_LIMIT_HIT = 1u << 1,
    STEPPER_FAULT_STALL = 1u << 2,
    STEPPER_FAULT_TARGET_TIMEOUT = 1u << 3
};

typedef struct {
    bool home_switch;
    bool min_limit_switch;
    bool max_limit_switch;
    bool stalled;
} stepper_feedback_t;

typedef struct {
    bool start_homing;
    bool clear_fault;
    bool move_request;
    int32_t target_steps;
} stepper_command_t;

typedef struct {
    stepper_state_t state;
    stepper_direction_t direction;
    int32_t position_steps;
    int32_t target_steps;
    uint32_t step_rate_sps;
    uint8_t progress_percent;
    bool driver_enable;
    uint32_t faults;
} stepper_output_t;

const char *stepper_state_name(stepper_state_t state);
const char *stepper_direction_name(stepper_direction_t direction);
void stepper_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len);

#endif
