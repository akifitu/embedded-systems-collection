#ifndef ENGINE_GUARD_H
#define ENGINE_GUARD_H

#include <stdbool.h>

#include "generator_types.h"

bool engine_guard_battery_ok(const gen_input_t *input);
bool engine_guard_started(const gen_input_t *input);
bool engine_guard_oil_ok(const gen_input_t *input);

#endif
