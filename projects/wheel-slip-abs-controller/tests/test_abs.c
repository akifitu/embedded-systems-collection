#include <assert.h>

#include "abs_controller.h"

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

static void test_no_brake_stays_standby(void) {
    abs_controller_t controller;
    abs_output_t output;
    abs_input_t input;

    abs_controller_init(&controller);
    input = make_input(0u, 880u, 880u, 879u, 878u, 880u);
    output = abs_controller_step(&controller, &input);

    assert(output.state == ABS_STATE_STANDBY);
    assert(!output.pump_enabled);
    assert(output.fault == ABS_FAULT_NONE);
}

static void test_threshold_braking_stays_base_brake(void) {
    abs_controller_t controller;
    abs_output_t output;
    abs_input_t input;

    abs_controller_init(&controller);
    input = make_input(34u, 720u, 675u, 672u, 684u, 676u);
    output = abs_controller_step(&controller, &input);

    assert(output.state == ABS_STATE_BRAKING);
    assert(!output.pump_enabled);
    assert(output.valves[0] == ABS_VALVE_APPLY);
    assert(output.valves[1] == ABS_VALVE_APPLY);
}

static void test_split_mu_enters_abs_active(void) {
    abs_controller_t controller;
    abs_output_t output;
    abs_input_t input;

    abs_controller_init(&controller);
    input = make_input(76u, 580u, 440u, 493u, 522u, 522u);
    output = abs_controller_step(&controller, &input);

    assert(output.state == ABS_STATE_ABS_ACTIVE);
    assert(output.pump_enabled);
    assert(output.valves[0] == ABS_VALVE_RELEASE);
    assert(output.valves[1] == ABS_VALVE_HOLD);
    assert(output.valves[2] == ABS_VALVE_APPLY);
    assert(output.valves[3] == ABS_VALVE_APPLY);
}

static void test_implausible_sensor_latches_fault(void) {
    abs_controller_t controller;
    abs_output_t output;
    abs_input_t input;

    abs_controller_init(&controller);
    input = make_input(68u, 520u, 900u, 480u, 479u, 478u);
    output = abs_controller_step(&controller, &input);

    assert(output.state == ABS_STATE_FAULT);
    assert(output.fault == ABS_FAULT_WHEEL_SENSOR);
    assert(!output.pump_enabled);
}

static void test_brake_release_clears_fault(void) {
    abs_controller_t controller;
    abs_output_t output;
    abs_input_t input;

    abs_controller_init(&controller);
    input = make_input(68u, 520u, 900u, 480u, 479u, 478u);
    output = abs_controller_step(&controller, &input);
    assert(output.state == ABS_STATE_FAULT);

    input = make_input(0u, 0u, 0u, 0u, 0u, 0u);
    output = abs_controller_step(&controller, &input);

    assert(output.state == ABS_STATE_STANDBY);
    assert(output.fault == ABS_FAULT_NONE);
}

int main(void) {
    test_no_brake_stays_standby();
    test_threshold_braking_stays_base_brake();
    test_split_mu_enters_abs_active();
    test_implausible_sensor_latches_fault();
    test_brake_release_clears_fault();
    return 0;
}
