#ifndef BREATH_PROFILE_H
#define BREATH_PROFILE_H

#include <stdbool.h>

#include "ventilator_types.h"

typedef struct {
    bool backup_active;
    unsigned blower_pct;
    bool inspiratory_valve_open;
    bool expiratory_valve_open;
    unsigned target_pressure_cmh2o;
} breath_profile_output_t;

breath_profile_output_t breath_profile_apply(vent_state_t state);

#endif
