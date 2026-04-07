#include <stdio.h>

#include "grade_crossing_controller.h"

static void print_phase(const char *label, const crossing_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s zone=%s gate=%s warn=%s bell=%s fault=%s\n",
        label, crossing_state_name(output->state),
        crossing_command_name(output->command), train_zone_name(output->zone),
        gate_position_name(output->gate_position),
        output->warning_active ? "ON" : "OFF",
        output->bell_active ? "ON" : "OFF",
        crossing_fault_name(output->fault));
}

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

static void demo_idle(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;

    grade_crossing_controller_init(&controller);
    output = grade_crossing_controller_step(
        &controller, &(crossing_input_t){false, false, false, true, true, false});
    print_phase("idle", &output);
}

static void demo_approach(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;

    grade_crossing_controller_init(&controller);
    output = grade_crossing_controller_step(
        &controller, &(crossing_input_t){true, false, false, true, true, false});
    print_phase("approach", &output);
}

static void demo_lowering(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;
    crossing_input_t input;
    crossing_input_t moving_input;

    grade_crossing_controller_init(&controller);
    input = make_input(true, false, false, true, true, false);
    (void)grade_crossing_controller_step(&controller, &input);
    moving_input = make_input(true, false, false, false, true, false);
    output = grade_crossing_controller_step(&controller, &moving_input);
    print_phase("lowering", &output);
}

static void demo_occupied(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;
    crossing_input_t input;

    grade_crossing_controller_init(&controller);
    input = make_input(true, false, false, true, true, false);
    (void)grade_crossing_controller_step(&controller, &input);
    (void)grade_crossing_controller_step(&controller, &input);
    input = make_input(false, true, true, false, true, false);
    output = grade_crossing_controller_step(&controller, &input);
    print_phase("occupied", &output);
}

static void demo_clearing(void) {
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
    print_phase("clearing", &output);
}

static void demo_lamp_fault(void) {
    grade_crossing_controller_t controller;
    crossing_output_t output;
    crossing_input_t input;

    grade_crossing_controller_init(&controller);
    input = make_input(true, false, false, true, true, false);
    (void)grade_crossing_controller_step(&controller, &input);
    input = make_input(true, false, true, false, false, false);
    output = grade_crossing_controller_step(&controller, &input);
    print_phase("lamp_fault", &output);
}

static void demo_recovered(void) {
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
    print_phase("recovered", &output);
}

int main(void) {
    demo_idle();
    demo_approach();
    demo_lowering();
    demo_occupied();
    demo_clearing();
    demo_lamp_fault();
    demo_recovered();
    return 0;
}
