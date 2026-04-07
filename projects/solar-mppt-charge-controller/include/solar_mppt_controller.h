#ifndef SOLAR_MPPT_CONTROLLER_H
#define SOLAR_MPPT_CONTROLLER_H

#include "power_tracker.h"
#include "solar_types.h"

typedef struct {
    solar_state_t state;
    solar_fault_t latched_fault;
    unsigned absorb_frames;
    power_tracker_state_t tracker;
} solar_mppt_controller_t;

void solar_mppt_controller_init(solar_mppt_controller_t *controller);
solar_output_t solar_mppt_controller_step(solar_mppt_controller_t *controller,
                                          const solar_input_t *input);

#endif
