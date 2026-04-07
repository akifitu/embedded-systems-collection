#ifndef WIND_GUARD_H
#define WIND_GUARD_H

#include <stdbool.h>

#include "turbine_types.h"

turbine_fault_t wind_guard_fault(const turbine_input_t *input);
bool wind_guard_can_start(const turbine_input_t *input);
bool wind_guard_should_storm_hold(const turbine_input_t *input);
bool wind_guard_should_feather(const turbine_input_t *input);
bool wind_guard_rotor_slow(const turbine_input_t *input);
bool wind_guard_can_park(const turbine_input_t *input);

#endif
