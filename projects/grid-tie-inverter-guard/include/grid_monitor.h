#ifndef GRID_MONITOR_H
#define GRID_MONITOR_H

#include "inverter_guard_types.h"

typedef struct {
    bool stable;
    inverter_reason_t fault_reason;
} grid_assessment_t;

grid_assessment_t grid_monitor_assess(const inverter_input_t *input);

#endif
