#include <assert.h>

#include "gas_burner_controller.h"

static burner_input_t make_input(bool call_for_heat, bool airflow_proven,
                                 bool flame_present, bool limit_closed,
                                 bool reset_request) {
    burner_input_t input;

    input.call_for_heat = call_for_heat;
    input.airflow_proven = airflow_proven;
    input.flame_present = flame_present;
    input.limit_closed = limit_closed;
    input.reset_request = reset_request;
    return input;
}

static void test_idle_holds_off(void) {
    gas_burner_controller_t controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(false, false, false, true, false);
    output = gas_burner_controller_step(&controller, &input);

    assert(output.state == BURNER_STATE_IDLE);
    assert(output.command == BURNER_COMMAND_HOLD_OFF);
}

static void test_heat_request_enters_prepurge(void) {
    gas_burner_controller_t controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(true, true, false, true, false);
    output = gas_burner_controller_step(&controller, &input);

    assert(output.state == BURNER_STATE_PREPURGE);
    assert(output.command == BURNER_COMMAND_START_PREPURGE);
    assert(output.progress_pct == 50u);
}

static void test_prepurge_reaches_ignition_and_running(void) {
    gas_burner_controller_t controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(true, true, false, true, false);
    (void)gas_burner_controller_step(&controller, &input);
    output = gas_burner_controller_step(&controller, &input);
    assert(output.state == BURNER_STATE_IGNITION);
    assert(output.command == BURNER_COMMAND_TRIAL_IGNITE);

    input = make_input(true, true, true, true, false);
    output = gas_burner_controller_step(&controller, &input);
    assert(output.state == BURNER_STATE_RUNNING);
    assert(output.flame_proven);
}

static void test_heat_end_enters_postpurge(void) {
    gas_burner_controller_t controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(true, true, false, true, false);
    (void)gas_burner_controller_step(&controller, &input);
    (void)gas_burner_controller_step(&controller, &input);
    input = make_input(true, true, true, true, false);
    (void)gas_burner_controller_step(&controller, &input);

    input = make_input(false, true, false, true, false);
    output = gas_burner_controller_step(&controller, &input);
    assert(output.state == BURNER_STATE_POSTPURGE);
    assert(output.command == BURNER_COMMAND_POSTPURGE_FAN);
}

static void test_ignition_fail_locks_out(void) {
    gas_burner_controller_t controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(true, true, false, true, false);
    (void)gas_burner_controller_step(&controller, &input);
    (void)gas_burner_controller_step(&controller, &input);
    output = gas_burner_controller_step(&controller, &input);

    assert(output.state == BURNER_STATE_LOCKOUT);
    assert(output.fault == BURNER_FAULT_IGNITION_FAIL);
}

static void test_stuck_flame_locks_out(void) {
    gas_burner_controller_t controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(false, false, true, true, false);
    output = gas_burner_controller_step(&controller, &input);

    assert(output.state == BURNER_STATE_LOCKOUT);
    assert(output.fault == BURNER_FAULT_FLAME_STUCK);
}

static void test_limit_open_locks_out(void) {
    gas_burner_controller_t controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(true, true, false, false, false);
    output = gas_burner_controller_step(&controller, &input);

    assert(output.state == BURNER_STATE_LOCKOUT);
    assert(output.fault == BURNER_FAULT_INTERLOCK_OPEN);
}

static void test_airflow_loss_during_run_locks_out(void) {
    gas_burner_controller_t controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(true, true, false, true, false);
    (void)gas_burner_controller_step(&controller, &input);
    (void)gas_burner_controller_step(&controller, &input);
    input = make_input(true, true, true, true, false);
    (void)gas_burner_controller_step(&controller, &input);

    input = make_input(true, false, true, true, false);
    output = gas_burner_controller_step(&controller, &input);
    assert(output.state == BURNER_STATE_LOCKOUT);
    assert(output.fault == BURNER_FAULT_AIRFLOW_FAIL);
}

static void test_reset_recovers_safely(void) {
    gas_burner_controller_t controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(true, true, false, true, false);
    (void)gas_burner_controller_step(&controller, &input);
    (void)gas_burner_controller_step(&controller, &input);
    (void)gas_burner_controller_step(&controller, &input);

    input = make_input(false, true, false, true, true);
    output = gas_burner_controller_step(&controller, &input);
    assert(output.state == BURNER_STATE_IDLE);
    assert(output.command == BURNER_COMMAND_RESET_BURNER);
}

int main(void) {
    test_idle_holds_off();
    test_heat_request_enters_prepurge();
    test_prepurge_reaches_ignition_and_running();
    test_heat_end_enters_postpurge();
    test_ignition_fail_locks_out();
    test_stuck_flame_locks_out();
    test_limit_open_locks_out();
    test_airflow_loss_during_run_locks_out();
    test_reset_recovers_safely();
    return 0;
}
