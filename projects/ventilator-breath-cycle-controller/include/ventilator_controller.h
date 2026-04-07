#ifndef VENTILATOR_CONTROLLER_H
#define VENTILATOR_CONTROLLER_H

#include "ventilator_types.h"

typedef struct {
    vent_state_t state;
    vent_fault_t latched_fault;
    unsigned phase_frames;
    unsigned exhale_frames;
} ventilator_controller_t;

void ventilator_controller_init(ventilator_controller_t *controller);
vent_output_t ventilator_controller_step(ventilator_controller_t *controller,
                                         const vent_input_t *input);

#endif
