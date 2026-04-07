#include "turbine_types.h"

const char *turbine_state_name(turbine_state_t state) {
    switch (state) {
        case TURBINE_STATE_PARKED:
            return "PARKED";
        case TURBINE_STATE_STARTUP:
            return "STARTUP";
        case TURBINE_STATE_GENERATING:
            return "GENERATING";
        case TURBINE_STATE_FEATHERING:
            return "FEATHERING";
        case TURBINE_STATE_STORM_HOLD:
            return "STORM_HOLD";
        case TURBINE_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *turbine_command_name(turbine_command_t command) {
    switch (command) {
        case TURBINE_COMMAND_HOLD_PARK:
            return "HOLD_PARK";
        case TURBINE_COMMAND_STARTUP_RELEASE:
            return "STARTUP_RELEASE";
        case TURBINE_COMMAND_TRACK_POWER:
            return "TRACK_POWER";
        case TURBINE_COMMAND_FEATHER_BLADES:
            return "FEATHER_BLADES";
        case TURBINE_COMMAND_HOLD_STORM:
            return "HOLD_STORM";
        case TURBINE_COMMAND_LATCH_FAULT:
            return "LATCH_FAULT";
        case TURBINE_COMMAND_RESET_TURBINE:
            return "RESET_TURBINE";
        default:
            return "UNKNOWN";
    }
}

const char *turbine_fault_name(turbine_fault_t fault) {
    switch (fault) {
        case TURBINE_FAULT_NONE:
            return "NONE";
        case TURBINE_FAULT_HYDRAULIC_LOW:
            return "HYDRAULIC_LOW";
        case TURBINE_FAULT_PITCH_SENSOR_MISMATCH:
            return "PITCH_SENSOR_MISMATCH";
        default:
            return "UNKNOWN";
    }
}
