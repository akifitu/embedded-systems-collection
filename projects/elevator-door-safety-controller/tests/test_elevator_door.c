#include <assert.h>

#include "elevator_door_controller.h"

static elevator_door_input_t make_input(bool open_request, bool close_request,
                                        bool obstruction_detected,
                                        bool door_open_fb,
                                        bool door_closed_fb,
                                        bool lock_engaged,
                                        bool reset_request) {
    elevator_door_input_t input;

    input.open_request = open_request;
    input.close_request = close_request;
    input.obstruction_detected = obstruction_detected;
    input.door_open_fb = door_open_fb;
    input.door_closed_fb = door_closed_fb;
    input.lock_engaged = lock_engaged;
    input.reset_request = reset_request;
    return input;
}

static void test_closed_holds_when_idle(void) {
    elevator_door_controller_t controller;
    elevator_door_output_t output;
    elevator_door_input_t input;

    elevator_door_controller_init(&controller);
    input = make_input(false, false, false, false, true, true, false);
    output = elevator_door_controller_step(&controller, &input);

    assert(output.state == DOOR_STATE_CLOSED);
    assert(output.command == DOOR_COMMAND_HOLD_CLOSED);
    assert(output.fault == DOOR_FAULT_NONE);
}

static void test_open_request_reaches_open_state(void) {
    elevator_door_controller_t controller;
    elevator_door_output_t output;
    elevator_door_input_t input;

    elevator_door_controller_init(&controller);
    input = make_input(true, false, false, false, false, false, false);
    output = elevator_door_controller_step(&controller, &input);
    assert(output.state == DOOR_STATE_OPENING);
    assert(output.command == DOOR_COMMAND_DRIVE_OPEN);

    input = make_input(false, false, false, true, false, false, false);
    output = elevator_door_controller_step(&controller, &input);
    assert(output.state == DOOR_STATE_OPEN);
    assert(output.command == DOOR_COMMAND_HOLD_OPEN);
}

static void test_close_and_lock_returns_closed(void) {
    elevator_door_controller_t controller;
    elevator_door_output_t output;
    elevator_door_input_t input;

    elevator_door_controller_init(&controller);
    input = make_input(true, false, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, false, true, true, false);
    output = elevator_door_controller_step(&controller, &input);

    assert(output.state == DOOR_STATE_CLOSED);
    assert(output.command == DOOR_COMMAND_HOLD_CLOSED);
    assert(output.fault == DOOR_FAULT_NONE);
}

static void test_obstruction_reopens_and_counts_retry(void) {
    elevator_door_controller_t controller;
    elevator_door_output_t output;
    elevator_door_input_t input;

    elevator_door_controller_init(&controller);
    input = make_input(true, false, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, true, false, false, false, false);
    output = elevator_door_controller_step(&controller, &input);

    assert(output.state == DOOR_STATE_OPENING);
    assert(output.command == DOOR_COMMAND_DRIVE_OPEN);
    assert(output.obstruction_retries == 1u);
}

static void test_second_obstruction_escalates_to_nudge(void) {
    elevator_door_controller_t controller;
    elevator_door_output_t output;
    elevator_door_input_t input;

    elevator_door_controller_init(&controller);
    input = make_input(true, false, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, true, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, true, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    output = elevator_door_controller_step(&controller, &input);

    assert(output.state == DOOR_STATE_NUDGE);
    assert(output.command == DOOR_COMMAND_NUDGE_CLOSE);
    assert(output.buzzer_active);
    assert(output.obstruction_retries == 2u);
}

static void test_lock_fail_latches_fault(void) {
    elevator_door_controller_t controller;
    elevator_door_output_t output;
    elevator_door_input_t input;

    elevator_door_controller_init(&controller);
    input = make_input(true, false, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, false, true, false, false);
    output = elevator_door_controller_step(&controller, &input);

    assert(output.state == DOOR_STATE_FAULT);
    assert(output.fault == DOOR_FAULT_LOCK_FAIL);
}

static void test_motion_timeout_faults_controller(void) {
    elevator_door_controller_t controller;
    elevator_door_output_t output;
    elevator_door_input_t input;

    elevator_door_controller_init(&controller);
    input = make_input(true, false, false, false, false, false, false);
    output = elevator_door_controller_step(&controller, &input);
    assert(output.state == DOOR_STATE_OPENING);
    output = elevator_door_controller_step(&controller, &input);
    assert(output.state == DOOR_STATE_OPENING);
    output = elevator_door_controller_step(&controller, &input);
    assert(output.state == DOOR_STATE_OPENING);
    output = elevator_door_controller_step(&controller, &input);

    assert(output.state == DOOR_STATE_FAULT);
    assert(output.fault == DOOR_FAULT_MOTION_TIMEOUT);
}

static void test_reset_recovers_from_fault(void) {
    elevator_door_controller_t controller;
    elevator_door_output_t output;
    elevator_door_input_t input;

    elevator_door_controller_init(&controller);
    input = make_input(true, false, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, true, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, true, false, false, false, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, false, true, false, false);
    (void)elevator_door_controller_step(&controller, &input);
    input = make_input(false, false, false, false, true, true, true);
    output = elevator_door_controller_step(&controller, &input);

    assert(output.state == DOOR_STATE_CLOSED);
    assert(output.command == DOOR_COMMAND_RESET_DOOR);
    assert(output.fault == DOOR_FAULT_NONE);
}

int main(void) {
    test_closed_holds_when_idle();
    test_open_request_reaches_open_state();
    test_close_and_lock_returns_closed();
    test_obstruction_reopens_and_counts_retry();
    test_second_obstruction_escalates_to_nudge();
    test_lock_fail_latches_fault();
    test_motion_timeout_faults_controller();
    test_reset_recovers_from_fault();
    return 0;
}
