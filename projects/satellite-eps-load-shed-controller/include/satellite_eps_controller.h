#ifndef SATELLITE_EPS_CONTROLLER_H
#define SATELLITE_EPS_CONTROLLER_H

#include "eps_types.h"

typedef struct {
    eps_state_t state;
    eps_fault_t latched_fault;
} satellite_eps_controller_t;

void satellite_eps_controller_init(satellite_eps_controller_t *controller);
eps_output_t satellite_eps_controller_step(satellite_eps_controller_t *controller,
                                           const eps_input_t *input);

#endif
