#include <stdio.h>

#include "abs_controller.h"

static void print_phase(const char *label, const abs_input_t *input,
                        const abs_output_t *output) {
    printf(
        "phase=%s state=%s pump=%s brake=%u veh=%u.%u slip=%u/%u/%u/%u "
        "valves=%s/%s/%s/%s fault=%s\n",
        label, abs_state_name(output->state),
        output->pump_enabled ? "ON" : "OFF", input->brake_pct,
        input->vehicle_speed_kph_x10 / 10u, input->vehicle_speed_kph_x10 % 10u,
        output->slip_pct[0], output->slip_pct[1], output->slip_pct[2],
        output->slip_pct[3], abs_valve_name(output->valves[0]),
        abs_valve_name(output->valves[1]), abs_valve_name(output->valves[2]),
        abs_valve_name(output->valves[3]), abs_fault_name(output->fault));
}

static abs_input_t make_input(uint8_t brake_pct, uint16_t vehicle_speed_kph_x10,
                              uint16_t fl, uint16_t fr, uint16_t rl,
                              uint16_t rr) {
    abs_input_t input;

    input.brake_pct = brake_pct;
    input.vehicle_speed_kph_x10 = vehicle_speed_kph_x10;
    input.wheel_speed_kph_x10[0] = fl;
    input.wheel_speed_kph_x10[1] = fr;
    input.wheel_speed_kph_x10[2] = rl;
    input.wheel_speed_kph_x10[3] = rr;
    return input;
}

static void demo_cruise(void) {
    abs_controller_t controller;
    abs_input_t input;
    abs_output_t output;

    abs_controller_init(&controller);
    input = make_input(0u, 880u, 880u, 879u, 878u, 880u);
    output = abs_controller_step(&controller, &input);
    print_phase("cruise", &input, &output);
}

static void demo_threshold(void) {
    abs_controller_t controller;
    abs_input_t input;
    abs_output_t output;

    abs_controller_init(&controller);
    input = make_input(34u, 720u, 675u, 672u, 684u, 676u);
    output = abs_controller_step(&controller, &input);
    print_phase("threshold", &input, &output);
}

static void demo_split_mu(void) {
    abs_controller_t controller;
    abs_input_t input;
    abs_output_t output;

    abs_controller_init(&controller);
    input = make_input(76u, 580u, 440u, 493u, 522u, 522u);
    output = abs_controller_step(&controller, &input);
    print_phase("split_mu", &input, &output);
}

static void demo_peak_control(void) {
    abs_controller_t controller;
    abs_input_t input;
    abs_output_t output;

    abs_controller_init(&controller);
    input = make_input(82u, 420u, 344u, 351u, 357u, 357u);
    output = abs_controller_step(&controller, &input);
    print_phase("peak_control", &input, &output);
}

static void demo_sensor_fault(void) {
    abs_controller_t controller;
    abs_input_t input;
    abs_output_t output;

    abs_controller_init(&controller);
    input = make_input(68u, 520u, 900u, 480u, 479u, 478u);
    output = abs_controller_step(&controller, &input);
    print_phase("sensor_fault", &input, &output);
}

static void demo_recovery(void) {
    abs_controller_t controller;
    abs_input_t fault_input;
    abs_input_t recovery_input;
    abs_output_t output;

    abs_controller_init(&controller);
    fault_input = make_input(68u, 520u, 900u, 480u, 479u, 478u);
    (void)abs_controller_step(&controller, &fault_input);
    recovery_input = make_input(0u, 0u, 0u, 0u, 0u, 0u);
    output = abs_controller_step(&controller, &recovery_input);
    print_phase("recovery", &recovery_input, &output);
}

int main(void) {
    demo_cruise();
    demo_threshold();
    demo_split_mu();
    demo_peak_control();
    demo_sensor_fault();
    demo_recovery();
    return 0;
}
