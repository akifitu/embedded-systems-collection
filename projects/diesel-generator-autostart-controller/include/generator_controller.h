#ifndef GENERATOR_CONTROLLER_H
#define GENERATOR_CONTROLLER_H

#include <stdint.h>

#include "generator_types.h"

typedef struct {
    gen_state_t state;
    gen_fault_t latched_fault;
    uint8_t start_attempts;
    uint8_t crank_frames;
    uint8_t warmup_frames;
    uint8_t cooldown_frames;
} generator_controller_t;

void generator_controller_init(generator_controller_t *controller);
gen_output_t generator_controller_step(generator_controller_t *controller,
                                       const gen_input_t *input);

#endif
