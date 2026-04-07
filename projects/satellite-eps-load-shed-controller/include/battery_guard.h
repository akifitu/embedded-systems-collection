#ifndef BATTERY_GUARD_H
#define BATTERY_GUARD_H

#include <stdbool.h>

#include "eps_types.h"

eps_fault_t battery_guard_fault(const eps_input_t *input);
bool battery_guard_should_shed(const eps_input_t *input);
bool battery_guard_should_survive(const eps_input_t *input);
bool battery_guard_can_recover(const eps_input_t *input);
bool battery_guard_can_nominal(const eps_input_t *input);

#endif
