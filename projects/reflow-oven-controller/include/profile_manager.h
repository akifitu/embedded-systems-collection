#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "reflow_types.h"

typedef struct {
    reflow_stage_t stage;
    uint32_t total_elapsed_s;
    uint32_t stage_elapsed_s;
    uint32_t soak_hold_s;
    uint32_t reflow_hold_s;
} profile_manager_t;

typedef struct {
    reflow_stage_t stage;
    float target_c;
    uint8_t progress_percent;
} profile_state_t;

void profile_manager_init(profile_manager_t *manager);
profile_state_t profile_manager_step(profile_manager_t *manager, float measured_c,
                                     bool fault_active);
profile_state_t profile_manager_snapshot(const profile_manager_t *manager);

#endif
