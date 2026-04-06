#include "trajectory_planner.h"

#include <math.h>
#include <stdlib.h>

enum {
    CONTROL_TICK_MS = 10
};

static float minf_local(float lhs, float rhs) {
    return lhs < rhs ? lhs : rhs;
}

static uint32_t abs_distance(int32_t lhs, int32_t rhs) {
    long long delta = (long long)rhs - (long long)lhs;
    if (delta < 0ll) {
        delta = -delta;
    }
    return (uint32_t)delta;
}

void trajectory_plan_build(trajectory_plan_t *plan, int32_t start_steps,
                           int32_t target_steps, float max_velocity_sps,
                           float accel_sps2) {
    float accel_distance_steps;
    float estimated_seconds;

    plan->start_steps = start_steps;
    plan->target_steps = target_steps;
    plan->distance_steps = abs_distance(start_steps, target_steps);
    plan->max_velocity_sps = max_velocity_sps;
    plan->accel_sps2 = accel_sps2;

    if (plan->distance_steps == 0u) {
        plan->peak_velocity_sps = 0.0f;
        plan->estimated_ticks = 0u;
        return;
    }

    accel_distance_steps =
        (max_velocity_sps * max_velocity_sps) / (2.0f * accel_sps2);
    if ((float)plan->distance_steps < (2.0f * accel_distance_steps)) {
        plan->peak_velocity_sps = sqrtf((float)plan->distance_steps * accel_sps2);
        estimated_seconds = 2.0f * plan->peak_velocity_sps / accel_sps2;
    } else {
        float cruise_distance_steps =
            (float)plan->distance_steps - (2.0f * accel_distance_steps);

        plan->peak_velocity_sps = max_velocity_sps;
        estimated_seconds =
            2.0f * max_velocity_sps / accel_sps2 +
            cruise_distance_steps / max_velocity_sps;
    }

    plan->estimated_ticks =
        (uint32_t)(estimated_seconds * (1000.0f / (float)CONTROL_TICK_MS)) + 20u;
}

bool trajectory_plan_is_triangular(const trajectory_plan_t *plan) {
    return plan->peak_velocity_sps < plan->max_velocity_sps;
}

float trajectory_plan_rate(const trajectory_plan_t *plan, int32_t current_steps) {
    float traveled_steps;
    float remaining_steps;
    float accel_rate;
    float decel_rate;
    float rate;

    if (plan->distance_steps == 0u) {
        return 0.0f;
    }

    traveled_steps = (float)abs_distance(plan->start_steps, current_steps);
    remaining_steps = (float)abs_distance(current_steps, plan->target_steps);

    accel_rate = sqrtf(2.0f * plan->accel_sps2 * (traveled_steps + 1.0f));
    decel_rate = sqrtf(2.0f * plan->accel_sps2 * (remaining_steps + 1.0f));
    rate = minf_local(plan->peak_velocity_sps, minf_local(accel_rate, decel_rate));

    if (remaining_steps > 0.0f && rate < 400.0f) {
        rate = minf_local(plan->peak_velocity_sps, 400.0f);
    }

    return rate;
}
