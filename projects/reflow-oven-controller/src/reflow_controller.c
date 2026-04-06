#include "reflow_controller.h"

static uint8_t fan_for_stage(reflow_stage_t stage, float measured_c,
                             float ambient_c, uint32_t faults) {
    if (faults != REFLOW_FAULT_NONE) {
        return measured_c > ambient_c + 10.0f ? 100u : 30u;
    }

    if (stage == REFLOW_STAGE_COOL) {
        return measured_c > 120.0f ? 100u : 70u;
    }

    if (stage == REFLOW_STAGE_COMPLETE && measured_c > ambient_c + 8.0f) {
        return 25u;
    }

    return 0u;
}

static void reset_runaway_window(reflow_controller_t *controller) {
    controller->runaway_window_s = 0u;
    controller->runaway_start_temp_c = 0.0f;
}

static void update_runaway_fault(reflow_controller_t *controller,
                                 const profile_state_t *profile,
                                 const oven_sample_t *sample,
                                 uint8_t heater_percent) {
    if (profile->stage == REFLOW_STAGE_COOL ||
        profile->stage == REFLOW_STAGE_COMPLETE ||
        profile->stage == REFLOW_STAGE_FAULT) {
        reset_runaway_window(controller);
        return;
    }

    if (heater_percent >= 85u &&
        (profile->target_c - sample->measured_c) >= 20.0f) {
        if (controller->runaway_window_s == 0u) {
            controller->runaway_start_temp_c = sample->measured_c;
        }
        controller->runaway_window_s++;

        if (controller->runaway_window_s >= 25u &&
            (sample->measured_c - controller->runaway_start_temp_c) < 6.0f) {
            controller->faults |= REFLOW_FAULT_RUNAWAY;
        }
    } else {
        reset_runaway_window(controller);
    }
}

void reflow_controller_init(reflow_controller_t *controller) {
    profile_manager_init(&controller->profile);
    pid_controller_init(&controller->pid);
    controller->faults = REFLOW_FAULT_NONE;
    controller->runaway_window_s = 0u;
    controller->runaway_start_temp_c = 0.0f;
    controller->previous_heater_percent = 0u;
}

oven_output_t reflow_controller_step(reflow_controller_t *controller,
                                     const oven_sample_t *sample) {
    oven_output_t output;
    profile_state_t profile;
    uint8_t fan_percent;
    uint8_t heater_percent;
    uint32_t new_faults = REFLOW_FAULT_NONE;

    if (!sample->sensor_valid) {
        new_faults |= REFLOW_FAULT_SENSOR_OPEN;
    }
    if (sample->measured_c >= 250.0f) {
        new_faults |= REFLOW_FAULT_OVERTEMP;
    }
    if (!sample->door_closed) {
        new_faults |= REFLOW_FAULT_DOOR_OPEN;
    }

    controller->faults |= new_faults;
    profile = profile_manager_step(&controller->profile, sample->measured_c,
                                   controller->faults != REFLOW_FAULT_NONE);

    heater_percent = pid_controller_step(&controller->pid, profile.stage,
                                         profile.target_c, sample->measured_c);
    update_runaway_fault(controller, &profile, sample, heater_percent);

    if (controller->faults != REFLOW_FAULT_NONE) {
        profile = profile_manager_step(&controller->profile, sample->measured_c,
                                       true);
        heater_percent = 0u;
    }

    fan_percent = fan_for_stage(profile.stage, sample->measured_c,
                                sample->ambient_c, controller->faults);
    controller->previous_heater_percent = heater_percent;

    output.stage = profile.stage;
    output.target_c = profile.target_c;
    output.heater_percent = heater_percent;
    output.fan_percent = fan_percent;
    output.progress_percent = profile.progress_percent;
    output.elapsed_s = controller->profile.total_elapsed_s;
    output.faults = controller->faults;
    return output;
}
