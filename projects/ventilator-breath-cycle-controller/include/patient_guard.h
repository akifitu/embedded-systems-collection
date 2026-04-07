#ifndef PATIENT_GUARD_H
#define PATIENT_GUARD_H

#include <stdbool.h>

#include "ventilator_types.h"

vent_fault_t patient_guard_fault(vent_state_t state, const vent_input_t *input);
bool patient_guard_apnea_due(unsigned exhale_frames, bool therapy_enabled);
bool patient_guard_can_reset(const vent_input_t *input);

#endif
