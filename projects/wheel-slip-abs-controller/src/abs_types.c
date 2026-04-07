#include "abs_types.h"

const char *abs_state_name(abs_state_t state) {
    switch (state) {
        case ABS_STATE_STANDBY:
            return "STANDBY";
        case ABS_STATE_BRAKING:
            return "BRAKING";
        case ABS_STATE_ABS_ACTIVE:
            return "ABS_ACTIVE";
        case ABS_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *abs_valve_name(abs_valve_t valve) {
    switch (valve) {
        case ABS_VALVE_HOLD:
            return "HOLD";
        case ABS_VALVE_APPLY:
            return "APPLY";
        case ABS_VALVE_RELEASE:
            return "RELEASE";
        default:
            return "UNKNOWN";
    }
}

const char *abs_fault_name(abs_fault_t fault) {
    switch (fault) {
        case ABS_FAULT_NONE:
            return "NONE";
        case ABS_FAULT_WHEEL_SENSOR:
            return "WHEEL_SENSOR";
        default:
            return "UNKNOWN";
    }
}
