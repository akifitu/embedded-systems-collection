#ifndef TRAJECTORY_PLANNER_H
#define TRAJECTORY_PLANNER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int32_t start_steps;
    int32_t target_steps;
    uint32_t distance_steps;
    float max_velocity_sps;
    float peak_velocity_sps;
    float accel_sps2;
    uint32_t estimated_ticks;
} trajectory_plan_t;

void trajectory_plan_build(trajectory_plan_t *plan, int32_t start_steps,
                           int32_t target_steps, float max_velocity_sps,
                           float accel_sps2);
bool trajectory_plan_is_triangular(const trajectory_plan_t *plan);
float trajectory_plan_rate(const trajectory_plan_t *plan, int32_t current_steps);

#endif
