#include "bldc_types.h"

#include <stdio.h>
#include <string.h>

const char *bldc_state_name(bldc_state_t state) {
    switch (state) {
        case BLDC_STATE_IDLE:
            return "IDLE";
        case BLDC_STATE_ALIGN:
            return "ALIGN";
        case BLDC_STATE_OPEN_LOOP:
            return "OPEN_LOOP";
        case BLDC_STATE_CLOSED_LOOP:
            return "CLOSED_LOOP";
        case BLDC_STATE_FAULT:
            return "FAULT";
    }

    return "UNKNOWN";
}

const char *bldc_sector_name(bldc_sector_t sector) {
    switch (sector) {
        case BLDC_SECTOR_S1:
            return "S1";
        case BLDC_SECTOR_S2:
            return "S2";
        case BLDC_SECTOR_S3:
            return "S3";
        case BLDC_SECTOR_S4:
            return "S4";
        case BLDC_SECTOR_S5:
            return "S5";
        case BLDC_SECTOR_S6:
            return "S6";
    }

    return "S?";
}

const char *bldc_lock_name(bldc_lock_t lock) {
    switch (lock) {
        case BLDC_LOCK_NONE:
            return "NONE";
        case BLDC_LOCK_SEEKING:
            return "SEEKING";
        case BLDC_LOCK_LOCKED:
            return "LOCKED";
        case BLDC_LOCK_LOST:
            return "LOST";
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

void bldc_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len) {
    if (buffer_len == 0u) {
        return;
    }

    buffer[0] = '\0';
    if (faults == BLDC_FAULT_NONE) {
        (void)snprintf(buffer, buffer_len, "none");
        return;
    }

    if ((faults & BLDC_FAULT_NO_LOCK) != 0u) {
        append_fault(buffer, buffer_len, "no_lock");
    }
    if ((faults & BLDC_FAULT_OVERCURRENT) != 0u) {
        append_fault(buffer, buffer_len, "overcurrent");
    }
    if ((faults & BLDC_FAULT_STALL) != 0u) {
        append_fault(buffer, buffer_len, "stall");
    }
}
