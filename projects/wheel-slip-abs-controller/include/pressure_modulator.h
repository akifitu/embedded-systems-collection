#ifndef PRESSURE_MODULATOR_H
#define PRESSURE_MODULATOR_H

#include <stdint.h>

#include "abs_types.h"

abs_valve_t pressure_modulator_command(uint8_t slip_pct);

#endif
