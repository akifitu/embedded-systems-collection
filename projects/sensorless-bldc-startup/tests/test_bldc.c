#include <assert.h>
#include <stdio.h>

#include "startup_controller.h"

static bldc_output_t step_once(startup_controller_t *controller, bool start,
                               bool clear_fault, uint8_t throttle_percent,
                               bool zero_cross, uint16_t phase_current_ma,
                               uint16_t measured_rpm) {
    bldc_command_t command;
    bldc_observation_t observation;

    command.start = start;
    command.clear_fault = clear_fault;
    command.throttle_percent = throttle_percent;
    observation.zero_cross = zero_cross;
    observation.phase_current_ma = phase_current_ma;
    observation.measured_rpm = measured_rpm;

    return startup_controller_step(controller, &command, &observation);
}

static void advance_to_open_loop(startup_controller_t *controller) {
    int tick;

    for (tick = 0; tick < 20; ++tick) {
        (void)step_once(controller, tick == 0, false, 34u, false, 2200u, 0u);
        if (controller->state == BLDC_STATE_OPEN_LOOP) {
            return;
        }
    }
}

static void advance_to_closed_loop(startup_controller_t *controller) {
    int tick;

    advance_to_open_loop(controller);
    for (tick = 0; tick < 4; ++tick) {
        (void)step_once(controller, false, false, 34u, true, 2600u, 1860u);
        if (controller->state == BLDC_STATE_CLOSED_LOOP) {
            return;
        }
    }
}

static void test_alignment_enters_open_loop(void) {
    startup_controller_t controller;

    startup_controller_init(&controller);
    advance_to_open_loop(&controller);

    assert(controller.state == BLDC_STATE_OPEN_LOOP);
    assert(controller.commutation_period_us <= 3200u);
    assert(controller.lock == BLDC_LOCK_SEEKING);
}

static void test_zero_cross_events_lock_closed_loop(void) {
    startup_controller_t controller;

    startup_controller_init(&controller);
    advance_to_closed_loop(&controller);

    assert(controller.state == BLDC_STATE_CLOSED_LOOP);
    assert(controller.lock == BLDC_LOCK_LOCKED);
    assert(controller.estimated_rpm == 1860u);
}

static void test_no_lock_fault_latches(void) {
    startup_controller_t controller;
    bldc_output_t output = {0};
    int tick;

    startup_controller_init(&controller);
    for (tick = 0; tick < 50; ++tick) {
        output = step_once(&controller, tick == 0, false, 34u, false, 2100u, 0u);
        if (output.state == BLDC_STATE_FAULT) {
            break;
        }
    }

    assert(output.state == BLDC_STATE_FAULT);
    assert((output.faults & BLDC_FAULT_NO_LOCK) != 0u);
}

static void test_overcurrent_fault_disables_drive(void) {
    startup_controller_t controller;
    bldc_output_t output;

    startup_controller_init(&controller);
    advance_to_closed_loop(&controller);
    output = step_once(&controller, false, false, 50u, true, 8400u, 1500u);

    assert(output.state == BLDC_STATE_FAULT);
    assert((output.faults & BLDC_FAULT_OVERCURRENT) != 0u);
    assert(output.duty_percent == 0u);
}

static void test_stall_fault_after_lock_loss(void) {
    startup_controller_t controller;
    bldc_output_t output = {0};
    int tick;

    startup_controller_init(&controller);
    advance_to_closed_loop(&controller);
    for (tick = 0; tick < 12; ++tick) {
        output = step_once(&controller, false, false, 34u, false, 2600u, 0u);
        if (output.state == BLDC_STATE_FAULT) {
            break;
        }
    }

    assert(output.state == BLDC_STATE_FAULT);
    assert((output.faults & BLDC_FAULT_STALL) != 0u);
}

int main(void) {
    test_alignment_enters_open_loop();
    test_zero_cross_events_lock_closed_loop();
    test_no_lock_fault_latches();
    test_overcurrent_fault_disables_drive();
    test_stall_fault_after_lock_loss();

    puts("sensorless-bldc-startup tests passed");
    return 0;
}
