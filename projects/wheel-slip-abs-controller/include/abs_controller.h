#ifndef ABS_CONTROLLER_H
#define ABS_CONTROLLER_H

#include "abs_types.h"

typedef struct {
    abs_state_t state;
    abs_fault_t latched_fault;
} abs_controller_t;

void abs_controller_init(abs_controller_t *controller);
abs_output_t abs_controller_step(abs_controller_t *controller,
                                 const abs_input_t *input);

#endif
