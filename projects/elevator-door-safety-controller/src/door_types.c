#include "door_types.h"

const char *door_state_name(door_state_t state) {
    switch (state) {
        case DOOR_STATE_CLOSED:
            return "CLOSED";
        case DOOR_STATE_OPENING:
            return "OPENING";
        case DOOR_STATE_OPEN:
            return "OPEN";
        case DOOR_STATE_CLOSING:
            return "CLOSING";
        case DOOR_STATE_NUDGE:
            return "NUDGE";
        case DOOR_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *door_command_name(door_command_t command) {
    switch (command) {
        case DOOR_COMMAND_HOLD_CLOSED:
            return "HOLD_CLOSED";
        case DOOR_COMMAND_DRIVE_OPEN:
            return "DRIVE_OPEN";
        case DOOR_COMMAND_HOLD_OPEN:
            return "HOLD_OPEN";
        case DOOR_COMMAND_DRIVE_CLOSE:
            return "DRIVE_CLOSE";
        case DOOR_COMMAND_NUDGE_CLOSE:
            return "NUDGE_CLOSE";
        case DOOR_COMMAND_LATCH_FAULT:
            return "LATCH_FAULT";
        case DOOR_COMMAND_RESET_DOOR:
            return "RESET_DOOR";
        default:
            return "UNKNOWN";
    }
}

const char *door_fault_name(door_fault_t fault) {
    switch (fault) {
        case DOOR_FAULT_NONE:
            return "NONE";
        case DOOR_FAULT_LOCK_FAIL:
            return "LOCK_FAIL";
        case DOOR_FAULT_MOTION_TIMEOUT:
            return "MOTION_TIMEOUT";
        default:
            return "UNKNOWN";
    }
}

const char *door_position_name(door_position_t position) {
    switch (position) {
        case DOOR_POSITION_CLOSED:
            return "CLOSED";
        case DOOR_POSITION_OPEN:
            return "OPEN";
        case DOOR_POSITION_MOVING:
            return "MOVING";
        default:
            return "UNKNOWN";
    }
}

const char *door_motor_name(door_motor_t motor) {
    switch (motor) {
        case DOOR_MOTOR_STOP:
            return "STOP";
        case DOOR_MOTOR_OPEN:
            return "OPEN";
        case DOOR_MOTOR_CLOSE:
            return "CLOSE";
        default:
            return "UNKNOWN";
    }
}

const char *door_lock_name(door_lock_t lock) {
    switch (lock) {
        case DOOR_LOCK_LOCKED:
            return "LOCKED";
        case DOOR_LOCK_UNLOCKED:
            return "UNLOCKED";
        default:
            return "UNKNOWN";
    }
}
