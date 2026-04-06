#include <assert.h>
#include <stdio.h>

#include "pid_controller.h"
#include "profile_manager.h"
#include "reflow_controller.h"

static void test_profile_progression(void) {
    profile_manager_t manager;
    profile_state_t state;
    int i;

    profile_manager_init(&manager);
    state = profile_manager_step(&manager, 146.0f, false);
    assert(state.stage == REFLOW_STAGE_SOAK);

    for (i = 0; i < 30; ++i) {
        state = profile_manager_step(&manager, 178.0f, false);
    }
    assert(state.stage == REFLOW_STAGE_REFLOW);

    for (i = 0; i < 15; ++i) {
        state = profile_manager_step(&manager, 228.0f, false);
    }
    assert(state.stage == REFLOW_STAGE_COOL);

    for (i = 0; i < 25; ++i) {
        state = profile_manager_step(&manager, 75.0f, false);
    }
    assert(state.stage == REFLOW_STAGE_COMPLETE);
    assert(state.progress_percent == 100u);
}

static void test_pid_limits_and_cooldown_zero(void) {
    pid_controller_t pid;
    uint8_t demand;

    pid_controller_init(&pid);
    demand = pid_controller_step(&pid, REFLOW_STAGE_PREHEAT, 150.0f, 25.0f);
    assert(demand == 100u);

    demand = pid_controller_step(&pid, REFLOW_STAGE_COOL, 90.0f, 110.0f);
    assert(demand == 0u);
}

static void test_sensor_fault_latches_safe_outputs(void) {
    reflow_controller_t controller;
    oven_sample_t sample;
    oven_output_t output;

    reflow_controller_init(&controller);
    sample.measured_c = 180.0f;
    sample.ambient_c = 25.0f;
    sample.sensor_valid = false;
    sample.door_closed = true;

    output = reflow_controller_step(&controller, &sample);
    assert(output.stage == REFLOW_STAGE_FAULT);
    assert((output.faults & REFLOW_FAULT_SENSOR_OPEN) != 0u);
    assert(output.heater_percent == 0u);
    assert(output.fan_percent == 100u);
}

static void test_runaway_fault_detected(void) {
    reflow_controller_t controller;
    oven_sample_t sample;
    oven_output_t output = {0};
    int i;

    reflow_controller_init(&controller);
    sample.measured_c = 25.0f;
    sample.ambient_c = 25.0f;
    sample.sensor_valid = true;
    sample.door_closed = true;

    for (i = 0; i < 30; ++i) {
        output = reflow_controller_step(&controller, &sample);
    }

    assert(output.stage == REFLOW_STAGE_FAULT);
    assert((output.faults & REFLOW_FAULT_RUNAWAY) != 0u);
    assert(output.heater_percent == 0u);
}

int main(void) {
    test_profile_progression();
    test_pid_limits_and_cooldown_zero();
    test_sensor_fault_latches_safe_outputs();
    test_runaway_fault_detected();

    puts("reflow-oven-controller tests passed");
    return 0;
}
