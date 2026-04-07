#include "eps_types.h"

const char *eps_state_name(eps_state_t state) {
    switch (state) {
        case EPS_STATE_NOMINAL:
            return "NOMINAL";
        case EPS_STATE_SHED:
            return "SHED";
        case EPS_STATE_SURVIVAL:
            return "SURVIVAL";
        case EPS_STATE_RECOVERY:
            return "RECOVERY";
        case EPS_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *eps_command_name(eps_command_t command) {
    switch (command) {
        case EPS_COMMAND_HOLD_FULL_POWER:
            return "HOLD_FULL_POWER";
        case EPS_COMMAND_SHED_NONCRITICAL:
            return "SHED_NONCRITICAL";
        case EPS_COMMAND_ENTER_SURVIVAL:
            return "ENTER_SURVIVAL";
        case EPS_COMMAND_RESTORE_LOADS:
            return "RESTORE_LOADS";
        case EPS_COMMAND_LATCH_FAULT:
            return "LATCH_FAULT";
        case EPS_COMMAND_RESET_EPS:
            return "RESET_EPS";
        default:
            return "UNKNOWN";
    }
}

const char *eps_fault_name(eps_fault_t fault) {
    switch (fault) {
        case EPS_FAULT_NONE:
            return "NONE";
        case EPS_FAULT_BUS_UNDERVOLTAGE:
            return "BUS_UNDERVOLTAGE";
        case EPS_FAULT_BATTERY_DEEP_DISCHARGE:
            return "BATTERY_DEEP_DISCHARGE";
        default:
            return "UNKNOWN";
    }
}
