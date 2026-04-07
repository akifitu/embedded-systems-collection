#include "burner_types.h"

const char *burner_state_name(burner_state_t state) {
    switch (state) {
        case BURNER_STATE_IDLE:
            return "IDLE";
        case BURNER_STATE_PREPURGE:
            return "PREPURGE";
        case BURNER_STATE_IGNITION:
            return "IGNITION";
        case BURNER_STATE_RUNNING:
            return "RUNNING";
        case BURNER_STATE_POSTPURGE:
            return "POSTPURGE";
        case BURNER_STATE_LOCKOUT:
            return "LOCKOUT";
        default:
            return "UNKNOWN";
    }
}

const char *burner_command_name(burner_command_t command) {
    switch (command) {
        case BURNER_COMMAND_HOLD_OFF:
            return "HOLD_OFF";
        case BURNER_COMMAND_START_PREPURGE:
            return "START_PREPURGE";
        case BURNER_COMMAND_TRIAL_IGNITE:
            return "TRIAL_IGNITE";
        case BURNER_COMMAND_HOLD_FLAME:
            return "HOLD_FLAME";
        case BURNER_COMMAND_POSTPURGE_FAN:
            return "POSTPURGE_FAN";
        case BURNER_COMMAND_LATCH_LOCKOUT:
            return "LATCH_LOCKOUT";
        case BURNER_COMMAND_RESET_BURNER:
            return "RESET_BURNER";
        default:
            return "UNKNOWN";
    }
}

const char *burner_fault_name(burner_fault_t fault) {
    switch (fault) {
        case BURNER_FAULT_NONE:
            return "NONE";
        case BURNER_FAULT_AIRFLOW_FAIL:
            return "AIRFLOW_FAIL";
        case BURNER_FAULT_IGNITION_FAIL:
            return "IGNITION_FAIL";
        case BURNER_FAULT_INTERLOCK_OPEN:
            return "INTERLOCK_OPEN";
        case BURNER_FAULT_FLAME_STUCK:
            return "FLAME_STUCK";
        default:
            return "UNKNOWN";
    }
}
