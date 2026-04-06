#ifndef STARTUP_CONTROLLER_H
#define STARTUP_CONTROLLER_H

#include <stdint.h>

#include "bldc_types.h"

typedef struct {
    bldc_state_t state;
    bldc_sector_t sector;
    bldc_lock_t lock;
    uint8_t duty_percent;
    uint16_t commutation_period_us;
    uint16_t estimated_rpm;
    uint32_t faults;
    uint8_t align_ticks;
    uint8_t open_loop_ticks;
    uint8_t zero_cross_events;
    uint8_t missed_zero_cross_ticks;
} startup_controller_t;

void startup_controller_init(startup_controller_t *controller);
bldc_output_t startup_controller_step(startup_controller_t *controller,
                                      const bldc_command_t *command,
                                      const bldc_observation_t *observation);

#endif
