#include <stdio.h>

#include "wind_turbine_controller.h"

static const char *on_off(bool value) {
    return value ? "ON" : "OFF";
}

static double dmps_to_mps(unsigned value) {
    return (double)value / 10.0;
}

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

static void print_phase(const char *phase, const turbine_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s fault=%s wind=%.1fmps rpm=%u pitch=%udeg "
        "gen=%s brake=%s\n",
        phase, turbine_state_name(output->state),
        turbine_command_name(output->command),
        turbine_fault_name(output->fault), dmps_to_mps(output->wind_dmps),
        output->rotor_rpm, output->pitch_deg, on_off(output->generator_enabled),
        on_off(output->brake_applied));
}

int main(void) {
    wind_turbine_controller_t controller;
    wind_turbine_controller_t fault_controller;
    turbine_output_t output;
    turbine_input_t input;

    wind_turbine_controller_init(&controller);
    input = make_input(false, false, true, true, true, 40u, 0u);
    output = wind_turbine_controller_step(&controller, &input);
    print_phase("parked", &output);

    input = make_input(true, false, true, true, true, 90u, 220u);
    output = wind_turbine_controller_step(&controller, &input);
    print_phase("startup", &output);

    input = make_input(true, false, true, true, true, 120u, 1480u);
    output = wind_turbine_controller_step(&controller, &input);
    print_phase("generating", &output);

    input = make_input(true, false, false, true, true, 130u, 1320u);
    output = wind_turbine_controller_step(&controller, &input);
    print_phase("grid_trip_feather", &output);

    input = make_input(false, false, false, true, true, 280u, 140u);
    output = wind_turbine_controller_step(&controller, &input);
    print_phase("storm_hold", &output);

    wind_turbine_controller_init(&fault_controller);
    input = make_input(true, false, true, true, true, 110u, 250u);
    (void)wind_turbine_controller_step(&fault_controller, &input);
    input = make_input(true, false, true, true, true, 110u, 1300u);
    (void)wind_turbine_controller_step(&fault_controller, &input);
    input = make_input(true, false, true, false, true, 110u, 1300u);
    output = wind_turbine_controller_step(&fault_controller, &input);
    print_phase("hydraulic_fault", &output);

    input = make_input(false, true, true, true, true, 70u, 0u);
    output = wind_turbine_controller_step(&fault_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
