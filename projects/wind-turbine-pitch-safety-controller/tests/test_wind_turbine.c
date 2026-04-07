#include <assert.h>

#include "wind_turbine_controller.h"

static turbine_input_t make_input(bool start_request, bool reset_request,
                                  bool grid_available, bool hydraulic_ok,
                                  bool pitch_sensor_ok, unsigned wind_dmps,
                                  unsigned rotor_rpm) {
    turbine_input_t input;

    input.start_request = start_request;
    input.reset_request = reset_request;
    input.grid_available = grid_available;
    input.hydraulic_ok = hydraulic_ok;
    input.pitch_sensor_ok = pitch_sensor_ok;
    input.wind_dmps = wind_dmps;
    input.rotor_rpm = rotor_rpm;
    return input;
}

static void test_parked_holds_when_idle(void) {
    wind_turbine_controller_t controller;
    turbine_output_t output;
    turbine_input_t input;

    wind_turbine_controller_init(&controller);
    input = make_input(false, false, true, true, true, 40u, 0u);
    output = wind_turbine_controller_step(&controller, &input);

    assert(output.state == TURBINE_STATE_PARKED);
    assert(output.command == TURBINE_COMMAND_HOLD_PARK);
    assert(output.brake_applied);
}

static void test_start_request_enters_startup(void) {
    wind_turbine_controller_t controller;
    turbine_output_t output;
    turbine_input_t input;

    wind_turbine_controller_init(&controller);
    input = make_input(true, false, true, true, true, 90u, 220u);
    output = wind_turbine_controller_step(&controller, &input);

    assert(output.state == TURBINE_STATE_STARTUP);
    assert(output.command == TURBINE_COMMAND_STARTUP_RELEASE);
    assert(output.pitch_deg == 18u);
}

static void test_startup_reaches_generating(void) {
    wind_turbine_controller_t controller;
    turbine_output_t output;
    turbine_input_t input;

    wind_turbine_controller_init(&controller);
    input = make_input(true, false, true, true, true, 90u, 220u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(true, false, true, true, true, 120u, 1480u);
    output = wind_turbine_controller_step(&controller, &input);

    assert(output.state == TURBINE_STATE_GENERATING);
    assert(output.command == TURBINE_COMMAND_TRACK_POWER);
    assert(output.generator_enabled);
}

static void test_grid_loss_enters_feathering(void) {
    wind_turbine_controller_t controller;
    turbine_output_t output;
    turbine_input_t input;

    wind_turbine_controller_init(&controller);
    input = make_input(true, false, true, true, true, 90u, 220u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(true, false, true, true, true, 120u, 1480u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(true, false, false, true, true, 130u, 1320u);
    output = wind_turbine_controller_step(&controller, &input);

    assert(output.state == TURBINE_STATE_FEATHERING);
    assert(output.command == TURBINE_COMMAND_FEATHER_BLADES);
    assert(!output.generator_enabled);
}

static void test_feathering_can_enter_storm_hold(void) {
    wind_turbine_controller_t controller;
    turbine_output_t output;
    turbine_input_t input;

    wind_turbine_controller_init(&controller);
    input = make_input(true, false, true, true, true, 90u, 220u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(true, false, true, true, true, 120u, 1480u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(true, false, false, true, true, 130u, 1320u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(false, false, false, true, true, 280u, 140u);
    output = wind_turbine_controller_step(&controller, &input);

    assert(output.state == TURBINE_STATE_STORM_HOLD);
    assert(output.command == TURBINE_COMMAND_HOLD_STORM);
    assert(output.brake_applied);
}

static void test_hydraulic_fault_latches_fault(void) {
    wind_turbine_controller_t controller;
    turbine_output_t output;
    turbine_input_t input;

    wind_turbine_controller_init(&controller);
    input = make_input(true, false, true, true, true, 90u, 220u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(true, false, true, true, true, 120u, 1300u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(true, false, true, false, true, 110u, 1300u);
    output = wind_turbine_controller_step(&controller, &input);

    assert(output.state == TURBINE_STATE_FAULT);
    assert(output.fault == TURBINE_FAULT_HYDRAULIC_LOW);
}

static void test_pitch_sensor_fault_latches_fault(void) {
    wind_turbine_controller_t controller;
    turbine_output_t output;
    turbine_input_t input;

    wind_turbine_controller_init(&controller);
    input = make_input(false, false, true, true, false, 70u, 0u);
    output = wind_turbine_controller_step(&controller, &input);

    assert(output.state == TURBINE_STATE_FAULT);
    assert(output.fault == TURBINE_FAULT_PITCH_SENSOR_MISMATCH);
}

static void test_reset_recovers_fault_when_safe(void) {
    wind_turbine_controller_t controller;
    turbine_output_t output;
    turbine_input_t input;

    wind_turbine_controller_init(&controller);
    input = make_input(true, false, true, true, true, 90u, 220u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(true, false, true, true, true, 120u, 1300u);
    (void)wind_turbine_controller_step(&controller, &input);
    input = make_input(true, false, true, false, true, 110u, 1300u);
    (void)wind_turbine_controller_step(&controller, &input);

    input = make_input(false, true, true, true, true, 70u, 0u);
    output = wind_turbine_controller_step(&controller, &input);

    assert(output.state == TURBINE_STATE_PARKED);
    assert(output.command == TURBINE_COMMAND_RESET_TURBINE);
    assert(output.fault == TURBINE_FAULT_NONE);
}

int main(void) {
    test_parked_holds_when_idle();
    test_start_request_enters_startup();
    test_startup_reaches_generating();
    test_grid_loss_enters_feathering();
    test_feathering_can_enter_storm_hold();
    test_hydraulic_fault_latches_fault();
    test_pitch_sensor_fault_latches_fault();
    test_reset_recovers_fault_when_safe();
    return 0;
}
