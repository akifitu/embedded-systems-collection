#include "reflow_types.h"

#include <stdio.h>
#include <string.h>

const char *reflow_stage_name(reflow_stage_t stage) {
    switch (stage) {
        case REFLOW_STAGE_IDLE:
            return "IDLE";
        case REFLOW_STAGE_PREHEAT:
            return "PREHEAT";
        case REFLOW_STAGE_SOAK:
            return "SOAK";
        case REFLOW_STAGE_REFLOW:
            return "REFLOW";
        case REFLOW_STAGE_COOL:
            return "COOL";
        case REFLOW_STAGE_COMPLETE:
            return "COMPLETE";
        case REFLOW_STAGE_FAULT:
            return "FAULT";
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

void reflow_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len) {
    if (buffer_len == 0u) {
        return;
    }

    buffer[0] = '\0';
    if (faults == REFLOW_FAULT_NONE) {
        (void)snprintf(buffer, buffer_len, "none");
        return;
    }

    if ((faults & REFLOW_FAULT_SENSOR_OPEN) != 0u) {
        append_fault(buffer, buffer_len, "sensor_open");
    }
    if ((faults & REFLOW_FAULT_OVERTEMP) != 0u) {
        append_fault(buffer, buffer_len, "overtemp");
    }
    if ((faults & REFLOW_FAULT_RUNAWAY) != 0u) {
        append_fault(buffer, buffer_len, "runaway");
    }
    if ((faults & REFLOW_FAULT_DOOR_OPEN) != 0u) {
        append_fault(buffer, buffer_len, "door_open");
    }
}
