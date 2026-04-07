#include <stdio.h>

#include "satellite_eps_controller.h"

static const char *yes_no(bool value) {
    return value ? "YES" : "NO";
}

static const char *on_off(bool value) {
    return value ? "ON" : "OFF";
}

static const char *radio_name(bool high_rate) {
    return high_rate ? "HIGH" : "BEACON";
}

static const char *adcs_name(bool full_rate) {
    return full_rate ? "FULL" : "SAFE";
}

static eps_input_t make_input(bool in_sunlight, unsigned battery_soc_pct,
                              unsigned bus_mv, unsigned panel_power_w,
                              unsigned payload_request_w, bool heater_request,
                              bool reset_request) {
    eps_input_t input;

    input.in_sunlight = in_sunlight;
    input.battery_soc_pct = battery_soc_pct;
    input.bus_mv = bus_mv;
    input.panel_power_w = panel_power_w;
    input.payload_request_w = payload_request_w;
    input.heater_request = heater_request;
    input.reset_request = reset_request;
    return input;
}

static void print_phase(const char *phase, const eps_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s fault=%s sun=%s soc=%u bus=%umV "
        "payload=%s heater=%s radio=%s adcs=%s budget=%uW\n",
        phase, eps_state_name(output->state),
        eps_command_name(output->command), eps_fault_name(output->fault),
        yes_no(output->in_sunlight), output->battery_soc_pct, output->bus_mv,
        on_off(output->payload_enabled), on_off(output->heater_enabled),
        radio_name(output->radio_high_rate), adcs_name(output->adcs_full_rate),
        output->budget_w);
}

int main(void) {
    satellite_eps_controller_t controller;
    satellite_eps_controller_t fault_controller;
    eps_output_t output;
    eps_input_t input;

    satellite_eps_controller_init(&controller);
    input = make_input(true, 78u, 7600u, 60u, 28u, true, false);
    output = satellite_eps_controller_step(&controller, &input);
    print_phase("nominal_daylight", &output);

    input = make_input(false, 42u, 7280u, 4u, 22u, false, false);
    output = satellite_eps_controller_step(&controller, &input);
    print_phase("eclipse_shed", &output);

    input = make_input(false, 16u, 6940u, 0u, 18u, false, false);
    output = satellite_eps_controller_step(&controller, &input);
    print_phase("deep_eclipse", &output);

    input = make_input(true, 34u, 7420u, 48u, 18u, true, false);
    output = satellite_eps_controller_step(&controller, &input);
    print_phase("recharge_recovery", &output);

    input = make_input(true, 62u, 7580u, 58u, 26u, false, false);
    output = satellite_eps_controller_step(&controller, &input);
    print_phase("nominal_restored", &output);

    satellite_eps_controller_init(&fault_controller);
    input = make_input(false, 18u, 6480u, 0u, 14u, false, false);
    output = satellite_eps_controller_step(&fault_controller, &input);
    print_phase("undervoltage_fault", &output);

    input = make_input(true, 66u, 7620u, 56u, 24u, false, true);
    output = satellite_eps_controller_step(&fault_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
