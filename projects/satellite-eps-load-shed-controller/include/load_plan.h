#ifndef LOAD_PLAN_H
#define LOAD_PLAN_H

#include <stdbool.h>

#include "eps_types.h"

typedef struct {
    bool payload_enabled;
    bool heater_enabled;
    bool radio_high_rate;
    bool adcs_full_rate;
    unsigned budget_w;
} load_plan_output_t;

load_plan_output_t load_plan_apply(eps_state_t state, const eps_input_t *input);

#endif
