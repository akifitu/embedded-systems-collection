#ifndef INVERTER_GUARD_H
#define INVERTER_GUARD_H

#include <stdint.h>

#include "inverter_guard_types.h"

typedef struct {
    inverter_state_t state;
    uint8_t sync_count;
    uint8_t cooldown_remaining;
    inverter_reason_t latched_reason;
} inverter_guard_t;

void inverter_guard_init(inverter_guard_t *guard);
inverter_output_t inverter_guard_step(inverter_guard_t *guard,
                                      const inverter_input_t *input);

#endif
