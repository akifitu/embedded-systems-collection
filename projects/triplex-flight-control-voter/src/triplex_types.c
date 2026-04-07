#include "triplex_types.h"

const char *triplex_state_name(triplex_state_t state) {
    switch (state) {
        case TRIPLEX_STATE_IDLE:
            return "IDLE";
        case TRIPLEX_STATE_SYNC:
            return "SYNC";
        case TRIPLEX_STATE_DEGRADED:
            return "DEGRADED";
        case TRIPLEX_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *triplex_command_name(triplex_command_t command) {
    switch (command) {
        case TRIPLEX_COMMAND_HOLD_LAST_GOOD:
            return "HOLD_LAST_GOOD";
        case TRIPLEX_COMMAND_VOTE_OUTPUT:
            return "VOTE_OUTPUT";
        case TRIPLEX_COMMAND_ISOLATE_LANE:
            return "ISOLATE_LANE";
        case TRIPLEX_COMMAND_LATCH_FAULT:
            return "LATCH_FAULT";
        case TRIPLEX_COMMAND_RESET_VOTER:
            return "RESET_VOTER";
        default:
            return "UNKNOWN";
    }
}

const char *triplex_fault_name(triplex_fault_t fault) {
    switch (fault) {
        case TRIPLEX_FAULT_NONE:
            return "NONE";
        case TRIPLEX_FAULT_LANE_A_DRIFT:
            return "LANE_A_DRIFT";
        case TRIPLEX_FAULT_LANE_B_DRIFT:
            return "LANE_B_DRIFT";
        case TRIPLEX_FAULT_LANE_C_DRIFT:
            return "LANE_C_DRIFT";
        case TRIPLEX_FAULT_LANE_TIMEOUT:
            return "LANE_TIMEOUT";
        case TRIPLEX_FAULT_SPLIT_VOTE:
            return "SPLIT_VOTE";
        case TRIPLEX_FAULT_MULTI_LANE_FAIL:
            return "MULTI_LANE_FAIL";
        default:
            return "UNKNOWN";
    }
}

const char *triplex_lane_name(triplex_lane_t lane) {
    switch (lane) {
        case TRIPLEX_LANE_NONE:
            return "NONE";
        case TRIPLEX_LANE_A:
            return "A";
        case TRIPLEX_LANE_B:
            return "B";
        case TRIPLEX_LANE_C:
            return "C";
        default:
            return "UNKNOWN";
    }
}

const char *triplex_active_mask_name(unsigned mask) {
    switch (mask) {
        case 0u:
            return "NONE";
        case TRIPLEX_MASK_A:
            return "A";
        case TRIPLEX_MASK_B:
            return "B";
        case TRIPLEX_MASK_C:
            return "C";
        case TRIPLEX_MASK_A | TRIPLEX_MASK_B:
            return "AB";
        case TRIPLEX_MASK_A | TRIPLEX_MASK_C:
            return "AC";
        case TRIPLEX_MASK_B | TRIPLEX_MASK_C:
            return "BC";
        case TRIPLEX_MASK_ALL:
            return "ABC";
        default:
            return "MASK?";
    }
}
