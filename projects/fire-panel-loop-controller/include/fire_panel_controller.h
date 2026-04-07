#ifndef FIRE_PANEL_CONTROLLER_H
#define FIRE_PANEL_CONTROLLER_H

#include "fire_panel_types.h"

typedef struct {
    panel_state_t state;
    panel_alarm_t latched_alarm;
    panel_trouble_t latched_trouble;
    unsigned smoke_verify_ticks;
} fire_panel_controller_t;

void fire_panel_controller_init(fire_panel_controller_t *controller);
fire_panel_output_t fire_panel_controller_step(fire_panel_controller_t *controller,
                                               const fire_panel_input_t *input);

#endif
