#include "profile_manager.h"

enum {
    PREHEAT_TIMEOUT_S = 120,
    SOAK_TIMEOUT_S = 90,
    SOAK_HOLD_TARGET_S = 30,
    REFLOW_TIMEOUT_S = 70,
    REFLOW_HOLD_TARGET_S = 15,
    COOL_MIN_TIME_S = 20,
    COOL_TIMEOUT_S = 120
};

static float target_for_stage(reflow_stage_t stage) {
    switch (stage) {
        case REFLOW_STAGE_PREHEAT:
            return 150.0f;
        case REFLOW_STAGE_SOAK:
            return 180.0f;
        case REFLOW_STAGE_REFLOW:
            return 235.0f;
        case REFLOW_STAGE_COOL:
            return 90.0f;
        case REFLOW_STAGE_COMPLETE:
            return 40.0f;
        case REFLOW_STAGE_FAULT:
        case REFLOW_STAGE_IDLE:
            return 0.0f;
    }

    return 0.0f;
}

static uint32_t min_u32(uint32_t a, uint32_t b) {
    return a < b ? a : b;
}

static uint8_t progress_for_manager(const profile_manager_t *manager) {
    switch (manager->stage) {
        case REFLOW_STAGE_PREHEAT:
            return (uint8_t)(min_u32(manager->stage_elapsed_s, PREHEAT_TIMEOUT_S) *
                             35u / PREHEAT_TIMEOUT_S);
        case REFLOW_STAGE_SOAK:
            return (uint8_t)(35u +
                             min_u32(manager->soak_hold_s, SOAK_HOLD_TARGET_S) *
                                 30u / SOAK_HOLD_TARGET_S);
        case REFLOW_STAGE_REFLOW:
            return (uint8_t)(65u +
                             min_u32(manager->reflow_hold_s,
                                     REFLOW_HOLD_TARGET_S) *
                                 20u / REFLOW_HOLD_TARGET_S);
        case REFLOW_STAGE_COOL:
            return (uint8_t)(85u +
                             min_u32(manager->stage_elapsed_s, 40u) * 15u / 40u);
        case REFLOW_STAGE_COMPLETE:
            return 100u;
        case REFLOW_STAGE_FAULT:
        case REFLOW_STAGE_IDLE:
            return 0u;
    }

    return 0u;
}

static void advance_stage(profile_manager_t *manager, reflow_stage_t next_stage) {
    manager->stage = next_stage;
    manager->stage_elapsed_s = 0u;
}

void profile_manager_init(profile_manager_t *manager) {
    manager->stage = REFLOW_STAGE_PREHEAT;
    manager->total_elapsed_s = 0u;
    manager->stage_elapsed_s = 0u;
    manager->soak_hold_s = 0u;
    manager->reflow_hold_s = 0u;
}

profile_state_t profile_manager_snapshot(const profile_manager_t *manager) {
    profile_state_t state;

    state.stage = manager->stage;
    state.target_c = target_for_stage(manager->stage);
    state.progress_percent = progress_for_manager(manager);
    return state;
}

profile_state_t profile_manager_step(profile_manager_t *manager, float measured_c,
                                     bool fault_active) {
    if (fault_active) {
        advance_stage(manager, REFLOW_STAGE_FAULT);
        return profile_manager_snapshot(manager);
    }

    if (manager->stage == REFLOW_STAGE_COMPLETE ||
        manager->stage == REFLOW_STAGE_FAULT) {
        return profile_manager_snapshot(manager);
    }

    manager->total_elapsed_s++;
    manager->stage_elapsed_s++;

    switch (manager->stage) {
        case REFLOW_STAGE_PREHEAT:
            if (measured_c >= 145.0f ||
                manager->stage_elapsed_s >= PREHEAT_TIMEOUT_S) {
                advance_stage(manager, REFLOW_STAGE_SOAK);
            }
            break;
        case REFLOW_STAGE_SOAK:
            if (measured_c >= 170.0f && measured_c <= 190.0f) {
                manager->soak_hold_s++;
            } else if (measured_c < 165.0f && manager->soak_hold_s > 0u) {
                manager->soak_hold_s--;
            }

            if (manager->soak_hold_s >= SOAK_HOLD_TARGET_S ||
                manager->stage_elapsed_s >= SOAK_TIMEOUT_S) {
                advance_stage(manager, REFLOW_STAGE_REFLOW);
            }
            break;
        case REFLOW_STAGE_REFLOW:
            if (measured_c >= 225.0f) {
                manager->reflow_hold_s++;
            } else if (measured_c < 220.0f && manager->reflow_hold_s > 0u) {
                manager->reflow_hold_s--;
            }

            if (manager->reflow_hold_s >= REFLOW_HOLD_TARGET_S ||
                manager->stage_elapsed_s >= REFLOW_TIMEOUT_S) {
                advance_stage(manager, REFLOW_STAGE_COOL);
            }
            break;
        case REFLOW_STAGE_COOL:
            if ((measured_c <= 80.0f &&
                 manager->stage_elapsed_s >= COOL_MIN_TIME_S) ||
                manager->stage_elapsed_s >= COOL_TIMEOUT_S) {
                advance_stage(manager, REFLOW_STAGE_COMPLETE);
            }
            break;
        case REFLOW_STAGE_COMPLETE:
        case REFLOW_STAGE_FAULT:
        case REFLOW_STAGE_IDLE:
            break;
    }

    return profile_manager_snapshot(manager);
}
