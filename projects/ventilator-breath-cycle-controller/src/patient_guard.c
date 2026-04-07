#include "patient_guard.h"

#define VENT_HIGH_PRESSURE_LIMIT 35u
#define VENT_DISCONNECT_PRESSURE_MAX 3u
#define VENT_DISCONNECT_FLOW_MAX 1u
#define VENT_APNEA_TIMEOUT_FRAMES 1u

vent_fault_t patient_guard_fault(vent_state_t state, const vent_input_t *input) {
    if (!input->gas_supply_ok) {
        return VENT_FAULT_GAS_SUPPLY_LOSS;
    }
    if (!input->pressure_sensor_ok) {
        return VENT_FAULT_SENSOR_FAULT;
    }
    if (((state == VENT_STATE_INHALE) || (state == VENT_STATE_HOLD) ||
         (state == VENT_STATE_BACKUP)) &&
        (input->airway_pressure_cmh2o >= VENT_HIGH_PRESSURE_LIMIT)) {
        return VENT_FAULT_HIGH_PRESSURE;
    }
    if ((state == VENT_STATE_EXHALE) && input->therapy_enabled &&
        (input->airway_pressure_cmh2o <= VENT_DISCONNECT_PRESSURE_MAX) &&
        (input->flow_lpm <= VENT_DISCONNECT_FLOW_MAX)) {
        return VENT_FAULT_DISCONNECT;
    }
    return VENT_FAULT_NONE;
}

bool patient_guard_apnea_due(unsigned exhale_frames, bool therapy_enabled) {
    return therapy_enabled && (exhale_frames >= VENT_APNEA_TIMEOUT_FRAMES);
}

bool patient_guard_can_reset(const vent_input_t *input) {
    return input->reset_request && input->gas_supply_ok && input->pressure_sensor_ok &&
           (input->airway_pressure_cmh2o <= 6u);
}
