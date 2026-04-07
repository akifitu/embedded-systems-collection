#include "interlock_guard.h"

burner_fault_t interlock_guard_fault(burner_state_t state,
                                     const burner_input_t *input) {
    if (state == BURNER_STATE_IDLE) {
        if (input->flame_present) {
            return BURNER_FAULT_FLAME_STUCK;
        }
        if (input->call_for_heat && !input->limit_closed) {
            return BURNER_FAULT_INTERLOCK_OPEN;
        }
        return BURNER_FAULT_NONE;
    }

    if (!input->limit_closed) {
        return BURNER_FAULT_INTERLOCK_OPEN;
    }

    if ((state == BURNER_STATE_PREPURGE) || (state == BURNER_STATE_IGNITION) ||
        (state == BURNER_STATE_RUNNING) || (state == BURNER_STATE_POSTPURGE)) {
        if (!input->airflow_proven) {
            return BURNER_FAULT_AIRFLOW_FAIL;
        }
    }

    return BURNER_FAULT_NONE;
}
