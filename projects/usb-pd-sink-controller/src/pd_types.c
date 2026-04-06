#include "pd_types.h"

#include <stdio.h>
#include <string.h>

const char *pd_state_name(pd_state_t state) {
    switch (state) {
        case PD_STATE_DETACHED:
            return "DETACHED";
        case PD_STATE_REQUESTING:
            return "REQUESTING";
        case PD_STATE_READY:
            return "READY";
        case PD_STATE_DERATED:
            return "DERATED";
        case PD_STATE_FAULT:
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

void pd_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len) {
    if (buffer_len == 0u) {
        return;
    }

    buffer[0] = '\0';
    if (faults == PD_FAULT_NONE) {
        (void)snprintf(buffer, buffer_len, "none");
        return;
    }

    if ((faults & PD_FAULT_SOURCE_MISMATCH) != 0u) {
        append_fault(buffer, buffer_len, "source_mismatch");
    }
    if ((faults & PD_FAULT_OVERTEMP) != 0u) {
        append_fault(buffer, buffer_len, "overtemp");
    }
    if ((faults & PD_FAULT_BROWNOUT) != 0u) {
        append_fault(buffer, buffer_len, "brownout");
    }
    if ((faults & PD_FAULT_RETRY_EXHAUSTED) != 0u) {
        append_fault(buffer, buffer_len, "retry_exhausted");
    }
}

void pd_contract_to_string(const pd_contract_t *contract, char *buffer,
                           unsigned buffer_len) {
    if (buffer_len == 0u) {
        return;
    }

    if (!contract->valid) {
        (void)snprintf(buffer, buffer_len, "none");
        return;
    }

    (void)snprintf(buffer, buffer_len, "%.1fV/%.2fA",
                   (double)contract->voltage_mv / 1000.0,
                   (double)contract->current_ma / 1000.0);
}

bool pd_contract_equal(const pd_contract_t *lhs, const pd_contract_t *rhs) {
    if (lhs->valid != rhs->valid) {
        return false;
    }

    if (!lhs->valid) {
        return true;
    }

    return lhs->pdo_index == rhs->pdo_index &&
           lhs->voltage_mv == rhs->voltage_mv &&
           lhs->current_ma == rhs->current_ma && lhs->power_mw == rhs->power_mw;
}
