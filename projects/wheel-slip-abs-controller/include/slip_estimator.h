#ifndef SLIP_ESTIMATOR_H
#define SLIP_ESTIMATOR_H

#include "abs_types.h"

abs_fault_t slip_estimator_calculate(const abs_input_t *input,
                                     uint8_t slip_pct[ABS_WHEEL_COUNT]);

#endif
