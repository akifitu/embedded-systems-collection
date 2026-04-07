#ifndef WIND_TURBINE_CONTROLLER_H
#define WIND_TURBINE_CONTROLLER_H

#include "turbine_types.h"

typedef struct {
    turbine_state_t state;
    turbine_fault_t latched_fault;
} wind_turbine_controller_t;

void wind_turbine_controller_init(wind_turbine_controller_t *controller);
turbine_output_t wind_turbine_controller_step(
    wind_turbine_controller_t *controller, const turbine_input_t *input);

#endif
