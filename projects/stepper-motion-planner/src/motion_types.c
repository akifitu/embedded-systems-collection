#include "motion_types.h"

#include <stdio.h>
#include <string.h>

const char *stepper_state_name(stepper_state_t state) {
    switch (state) {
        case STEPPER_STATE_IDLE:
            return "IDLE";
        case STEPPER_STATE_HOMING:
            return "HOMING";
        case STEPPER_STATE_READY:
            return "READY";
        case STEPPER_STATE_MOVING:
            return "MOVING";
        case STEPPER_STATE_FAULT:
            return "FAULT";
    }

    return "UNKNOWN";
}

const char *stepper_direction_name(stepper_direction_t direction) {
    switch (direction) {
        case STEPPER_DIR_STOP:
            return "STOP";
        case STEPPER_DIR_FWD:
            return "FWD";
        case STEPPER_DIR_REV:
            return "REV";
    }

    return "UNKNOWN";
}

static void append_fault(char *buffer, unsigned buffer_len, const char *label) {
    unsigned used = (unsigned)strlen(buffer);

    if (used >= buffer_len) {
        return;
    }

    if (used != 0u) {
        (void)snprintf(buffer + used, buffer_len - used, "|%s", label);
    } else {
        (void)snprintf(buffer + used, buffer_len - used, "%s", label);
    }
}

void stepper_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len) {
    if (buffer_len == 0u) {
        return;
    }

    buffer[0] = '\0';
    if (faults == STEPPER_FAULT_NONE) {
        (void)snprintf(buffer, buffer_len, "none");
        return;
    }

    if ((faults & STEPPER_FAULT_HOME_TIMEOUT) != 0u) {
        append_fault(buffer, buffer_len, "home_timeout");
    }
    if ((faults & STEPPER_FAULT_LIMIT_HIT) != 0u) {
        append_fault(buffer, buffer_len, "limit_hit");
    }
    if ((faults & STEPPER_FAULT_STALL) != 0u) {
        append_fault(buffer, buffer_len, "stall");
    }
    if ((faults & STEPPER_FAULT_TARGET_TIMEOUT) != 0u) {
        append_fault(buffer, buffer_len, "target_timeout");
    }
}
