#ifndef CHARGE_POLICY_H
#define CHARGE_POLICY_H

#include <stdbool.h>

#include "solar_types.h"

typedef struct {
    bool relay_closed;
    bool fan_active;
    unsigned stage_target_mv;
} charge_policy_output_t;

charge_policy_output_t charge_policy_apply(solar_state_t state,
                                           const solar_input_t *input);

#endif
