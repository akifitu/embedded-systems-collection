#ifndef DOOR_TYPES_H
#define DOOR_TYPES_H

#include <stdbool.h>

typedef enum {
    DOOR_STATE_CLOSED = 0,
    DOOR_STATE_OPENING,
    DOOR_STATE_OPEN,
    DOOR_STATE_CLOSING,
    DOOR_STATE_NUDGE,
    DOOR_STATE_FAULT
} door_state_t;

typedef enum {
    DOOR_COMMAND_HOLD_CLOSED = 0,
    DOOR_COMMAND_DRIVE_OPEN,
    DOOR_COMMAND_HOLD_OPEN,
    DOOR_COMMAND_DRIVE_CLOSE,
    DOOR_COMMAND_NUDGE_CLOSE,
    DOOR_COMMAND_LATCH_FAULT,
    DOOR_COMMAND_RESET_DOOR
} door_command_t;

typedef enum {
    DOOR_FAULT_NONE = 0,
    DOOR_FAULT_LOCK_FAIL,
    DOOR_FAULT_MOTION_TIMEOUT
} door_fault_t;

typedef enum {
    DOOR_POSITION_CLOSED = 0,
    DOOR_POSITION_OPEN,
    DOOR_POSITION_MOVING
} door_position_t;

typedef enum {
    DOOR_MOTOR_STOP = 0,
    DOOR_MOTOR_OPEN,
    DOOR_MOTOR_CLOSE
} door_motor_t;

typedef enum {
    DOOR_LOCK_LOCKED = 0,
    DOOR_LOCK_UNLOCKED
} door_lock_t;

typedef struct {
    bool open_request;
    bool close_request;
    bool obstruction_detected;
    bool door_open_fb;
    bool door_closed_fb;
    bool lock_engaged;
    bool reset_request;
} elevator_door_input_t;

typedef struct {
    door_state_t state;
    door_command_t command;
    door_fault_t fault;
    door_position_t position;
    door_motor_t motor;
    door_lock_t lock;
    bool buzzer_active;
    unsigned obstruction_retries;
} elevator_door_output_t;

const char *door_state_name(door_state_t state);
const char *door_command_name(door_command_t command);
const char *door_fault_name(door_fault_t fault);
const char *door_position_name(door_position_t position);
const char *door_motor_name(door_motor_t motor);
const char *door_lock_name(door_lock_t lock);

#endif
