#include "generator_types.h"

const char *gen_state_name(gen_state_t state) {
    switch (state) {
        case GEN_STATE_IDLE:
            return "IDLE";
        case GEN_STATE_START_DELAY:
            return "START_DELAY";
        case GEN_STATE_CRANKING:
            return "CRANKING";
        case GEN_STATE_WARMUP:
            return "WARMUP";
        case GEN_STATE_RUNNING:
            return "RUNNING";
        case GEN_STATE_COOLDOWN:
            return "COOLDOWN";
        case GEN_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *gen_command_name(gen_command_t command) {
    switch (command) {
        case GEN_COMMAND_OPEN_CONTACTOR:
            return "OPEN_CONTACTOR";
        case GEN_COMMAND_PREHEAT:
            return "PREHEAT";
        case GEN_COMMAND_CRANK_START:
            return "CRANK_START";
        case GEN_COMMAND_CLOSE_CONTACTOR:
            return "CLOSE_CONTACTOR";
        case GEN_COMMAND_COOL_ENGINE:
            return "COOL_ENGINE";
        case GEN_COMMAND_STOP_ENGINE:
            return "STOP_ENGINE";
        default:
            return "UNKNOWN";
    }
}

const char *gen_fault_name(gen_fault_t fault) {
    switch (fault) {
        case GEN_FAULT_NONE:
            return "NONE";
        case GEN_FAULT_LOW_BATTERY:
            return "LOW_BATTERY";
        case GEN_FAULT_START_FAIL:
            return "START_FAIL";
        case GEN_FAULT_LOW_OIL:
            return "LOW_OIL";
        case GEN_FAULT_E_STOP:
            return "E_STOP";
        default:
            return "UNKNOWN";
    }
}
