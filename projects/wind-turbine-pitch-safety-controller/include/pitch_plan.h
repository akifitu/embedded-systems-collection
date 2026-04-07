#ifndef PITCH_PLAN_H
#define PITCH_PLAN_H

#include <stdbool.h>

#include "turbine_types.h"

typedef struct {
    unsigned pitch_deg;
    bool generator_enabled;
    bool brake_applied;
} pitch_plan_output_t;

pitch_plan_output_t pitch_plan_apply(turbine_state_t state,
                                     const turbine_input_t *input);

#endif
