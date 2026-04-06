#ifndef REFLOW_CONTROLLER_H
#define REFLOW_CONTROLLER_H

#include <stdint.h>

#include "pid_controller.h"
#include "profile_manager.h"
#include "reflow_types.h"

typedef struct {
    profile_manager_t profile;
    pid_controller_t pid;
    uint32_t faults;
    uint32_t runaway_window_s;
    float runaway_start_temp_c;
    uint8_t previous_heater_percent;
} reflow_controller_t;

void reflow_controller_init(reflow_controller_t *controller);
oven_output_t reflow_controller_step(reflow_controller_t *controller,
                                     const oven_sample_t *sample);

#endif
