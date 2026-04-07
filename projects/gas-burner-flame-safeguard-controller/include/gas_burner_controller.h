#ifndef GAS_BURNER_CONTROLLER_H
#define GAS_BURNER_CONTROLLER_H

#include "burner_types.h"

typedef struct {
    burner_state_t state;
    burner_fault_t latched_fault;
    unsigned phase_frames;
} gas_burner_controller_t;

void gas_burner_controller_init(gas_burner_controller_t *controller);
burner_output_t gas_burner_controller_step(gas_burner_controller_t *controller,
                                           const burner_input_t *input);

#endif
