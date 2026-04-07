#ifndef ELEVATOR_DOOR_CONTROLLER_H
#define ELEVATOR_DOOR_CONTROLLER_H

#include "door_types.h"

typedef struct {
    door_state_t state;
    door_fault_t latched_fault;
    unsigned obstruction_retries;
    unsigned motion_frames;
} elevator_door_controller_t;

void elevator_door_controller_init(elevator_door_controller_t *controller);
elevator_door_output_t elevator_door_controller_step(
    elevator_door_controller_t *controller, const elevator_door_input_t *input);

#endif
