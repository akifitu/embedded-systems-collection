#ifndef GRADE_CROSSING_CONTROLLER_H
#define GRADE_CROSSING_CONTROLLER_H

#include "crossing_types.h"

typedef struct {
    crossing_state_t state;
    crossing_fault_t latched_fault;
    unsigned prewarn_frames;
    unsigned motion_frames;
} grade_crossing_controller_t;

void grade_crossing_controller_init(grade_crossing_controller_t *controller);
crossing_output_t grade_crossing_controller_step(
    grade_crossing_controller_t *controller, const crossing_input_t *input);

#endif
