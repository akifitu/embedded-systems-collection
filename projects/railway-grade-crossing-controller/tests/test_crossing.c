#include <assert.h>

#include "grade_crossing_controller.h"

static crossing_input_t make_input(bool approach_active, bool island_occupied,
                                   bool gate_down_fb, bool gate_up_fb,
                                   bool lamp_ok, bool reset_request) {
    crossing_input_t input;

    input.approach_active = approach_active;
    input.island_occupied = island_occupied;
    input.gate_down_fb = gate_down_fb;
    input.gate_up_fb = gate_up_fb;
    input.lamp_ok = lamp_ok;
    input.reset_request = reset_request;
    return input;
}

static void test_idle_with_clear_track(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;

    grade_crossing_controller_init(&controller);
    output = grade_crossing_controller_step(
        &controller, &(crossing_input_t){false, false, false, true, true, false});

    assert(output.state == CROSSING_STATE_IDLE);
    assert(output.command == CROSSING_COMMAND_GATE_UP);
    assert(!output.warning_active);
}

static void test_approach_enters_prewarn_then_lowering(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;
    crossing_input_t input;

    grade_crossing_controller_init(&controller);
    input = make_input(true, false, false, true, true, false);
    output = grade_crossing_controller_step(&controller, &input);
    assert(output.state == CROSSING_STATE_PREWARN);
    assert(output.command == CROSSING_COMMAND_START_WARNING);

    output = grade_crossing_controller_step(&controller, &input);
    assert(output.state == CROSSING_STATE_LOWERING);
    assert(output.command == CROSSING_COMMAND_LOWER_GATE);
}

static void test_gate_down_enters_protected(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;
    crossing_input_t input;

    grade_crossing_controller_init(&controller);
    input = make_input(true, false, false, true, true, false);
    (void)grade_crossing_controller_step(&controller, &input);
    (void)grade_crossing_controller_step(&controller, &input);
    input = make_input(false, true, true, false, true, false);
    output = grade_crossing_controller_step(&controller, &input);

    assert(output.state == CROSSING_STATE_PROTECTED);
    assert(output.command == CROSSING_COMMAND_HOLD_DOWN);
    assert(output.zone == TRAIN_ZONE_ISLAND);
}

static void test_clear_route_returns_to_idle(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;
    crossing_input_t input;

    grade_crossing_controller_init(&controller);
    input = make_input(true, false, false, true, true, false);
    (void)grade_crossing_controller_step(&controller, &input);
    (void)grade_crossing_controller_step(&controller, &input);
    input = make_input(false, true, true, false, true, false);
    (void)grade_crossing_controller_step(&controller, &input);
    input = make_input(false, false, false, false, true, false);
    output = grade_crossing_controller_step(&controller, &input);
    assert(output.state == CROSSING_STATE_RAISING);

    input = make_input(false, false, false, true, true, false);
    output = grade_crossing_controller_step(&controller, &input);
    assert(output.state == CROSSING_STATE_IDLE);
    assert(output.command == CROSSING_COMMAND_GATE_UP);
}

static void test_lamp_fault_latches_fault_state(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;
    crossing_input_t input;

    grade_crossing_controller_init(&controller);
    input = make_input(true, false, false, true, true, false);
    (void)grade_crossing_controller_step(&controller, &input);
    input = make_input(true, false, true, false, false, false);
    output = grade_crossing_controller_step(&controller, &input);

    assert(output.state == CROSSING_STATE_FAULT);
    assert(output.fault == CROSSING_FAULT_LAMP_FAIL);
}

static void test_gate_timeout_faults_controller(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;
    crossing_input_t input;

    grade_crossing_controller_init(&controller);
    input = make_input(true, false, false, true, true, false);
    (void)grade_crossing_controller_step(&controller, &input);
    output = grade_crossing_controller_step(&controller, &input);
    assert(output.state == CROSSING_STATE_LOWERING);
    output = grade_crossing_controller_step(&controller, &input);
    assert(output.state == CROSSING_STATE_LOWERING);
    output = grade_crossing_controller_step(&controller, &input);
    assert(output.state == CROSSING_STATE_LOWERING);
    output = grade_crossing_controller_step(&controller, &input);
    assert(output.state == CROSSING_STATE_FAULT);
    assert(output.fault == CROSSING_FAULT_GATE_TIMEOUT);
}

static void test_reset_recovers_fault_when_safe(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;
    crossing_input_t input;

    grade_crossing_controller_init(&controller);
    input = make_input(true, false, false, true, true, false);
    (void)grade_crossing_controller_step(&controller, &input);
    input = make_input(true, false, true, false, false, false);
    (void)grade_crossing_controller_step(&controller, &input);
    input = make_input(false, false, false, true, true, true);
    output = grade_crossing_controller_step(&controller, &input);

    assert(output.state == CROSSING_STATE_IDLE);
    assert(output.fault == CROSSING_FAULT_NONE);
}

int main(void) {
    test_idle_with_clear_track();
    test_approach_enters_prewarn_then_lowering();
    test_gate_down_enters_protected();
    test_clear_route_returns_to_idle();
    test_lamp_fault_latches_fault_state();
    test_gate_timeout_faults_controller();
    test_reset_recovers_fault_when_safe();
    return 0;
}
