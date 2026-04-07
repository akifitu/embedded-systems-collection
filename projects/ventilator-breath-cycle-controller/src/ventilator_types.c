#include "ventilator_types.h"

const char *vent_state_name(vent_state_t state) {
    switch (state) {
        case VENT_STATE_STANDBY:
            return "STANDBY";
        case VENT_STATE_INHALE:
            return "INHALE";
        case VENT_STATE_HOLD:
            return "HOLD";
        case VENT_STATE_EXHALE:
            return "EXHALE";
        case VENT_STATE_BACKUP:
            return "BACKUP";
        case VENT_STATE_ALARM:
            return "ALARM";
        default:
            return "UNKNOWN";
    }
}

const char *vent_command_name(vent_command_t command) {
    switch (command) {
        case VENT_COMMAND_HOLD_STANDBY:
            return "HOLD_STANDBY";
        case VENT_COMMAND_DELIVER_INHALE:
            return "DELIVER_INHALE";
        case VENT_COMMAND_HOLD_PLATEAU:
            return "HOLD_PLATEAU";
        case VENT_COMMAND_VENT_EXHALE:
            return "VENT_EXHALE";
        case VENT_COMMAND_START_BACKUP:
            return "START_BACKUP";
        case VENT_COMMAND_LATCH_ALARM:
            return "LATCH_ALARM";
        case VENT_COMMAND_RESET_VENT:
            return "RESET_VENT";
        default:
            return "UNKNOWN";
    }
}

const char *vent_fault_name(vent_fault_t fault) {
    switch (fault) {
        case VENT_FAULT_NONE:
            return "NONE";
        case VENT_FAULT_HIGH_PRESSURE:
            return "HIGH_PRESSURE";
        case VENT_FAULT_DISCONNECT:
            return "DISCONNECT";
        case VENT_FAULT_GAS_SUPPLY_LOSS:
            return "GAS_SUPPLY_LOSS";
        case VENT_FAULT_SENSOR_FAULT:
            return "SENSOR_FAULT";
        default:
            return "UNKNOWN";
    }
}
