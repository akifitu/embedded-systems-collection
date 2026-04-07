#include "crossing_types.h"

const char *crossing_state_name(crossing_state_t state) {
    switch (state) {
        case CROSSING_STATE_IDLE:
            return "IDLE";
        case CROSSING_STATE_PREWARN:
            return "PREWARN";
        case CROSSING_STATE_LOWERING:
            return "LOWERING";
        case CROSSING_STATE_PROTECTED:
            return "PROTECTED";
        case CROSSING_STATE_RAISING:
            return "RAISING";
        case CROSSING_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *crossing_command_name(crossing_command_t command) {
    switch (command) {
        case CROSSING_COMMAND_GATE_UP:
            return "GATE_UP";
        case CROSSING_COMMAND_START_WARNING:
            return "START_WARNING";
        case CROSSING_COMMAND_LOWER_GATE:
            return "LOWER_GATE";
        case CROSSING_COMMAND_HOLD_DOWN:
            return "HOLD_DOWN";
        case CROSSING_COMMAND_RAISE_GATE:
            return "RAISE_GATE";
        case CROSSING_COMMAND_LATCH_FAULT:
            return "LATCH_FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *crossing_fault_name(crossing_fault_t fault) {
    switch (fault) {
        case CROSSING_FAULT_NONE:
            return "NONE";
        case CROSSING_FAULT_LAMP_FAIL:
            return "LAMP_FAIL";
        case CROSSING_FAULT_GATE_TIMEOUT:
            return "GATE_TIMEOUT";
        default:
            return "UNKNOWN";
    }
}

const char *train_zone_name(train_zone_t zone) {
    switch (zone) {
        case TRAIN_ZONE_CLEAR:
            return "CLEAR";
        case TRAIN_ZONE_APPROACH:
            return "APPROACH";
        case TRAIN_ZONE_ISLAND:
            return "ISLAND";
        default:
            return "UNKNOWN";
    }
}

const char *gate_position_name(gate_position_t position) {
    switch (position) {
        case GATE_POSITION_UP:
            return "UP";
        case GATE_POSITION_DOWN:
            return "DOWN";
        case GATE_POSITION_MOVING:
            return "MOVING";
        default:
            return "UNKNOWN";
    }
}
