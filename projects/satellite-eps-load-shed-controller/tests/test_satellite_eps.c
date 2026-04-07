#include <assert.h>

#include "satellite_eps_controller.h"

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

static void test_nominal_daylight_keeps_full_power(void) {
    satellite_eps_controller_t controller;
    eps_output_t output;
    eps_input_t input;

    satellite_eps_controller_init(&controller);
    input = make_input(true, 78u, 7600u, 60u, 28u, true, false);
    output = satellite_eps_controller_step(&controller, &input);

    assert(output.state == EPS_STATE_NOMINAL);
    assert(output.command == EPS_COMMAND_HOLD_FULL_POWER);
    assert(output.payload_enabled);
    assert(output.radio_high_rate);
}

static void test_eclipse_enters_shed(void) {
    satellite_eps_controller_t controller;
    eps_output_t output;
    eps_input_t input;

    satellite_eps_controller_init(&controller);
    input = make_input(false, 42u, 7280u, 4u, 22u, false, false);
    output = satellite_eps_controller_step(&controller, &input);

    assert(output.state == EPS_STATE_SHED);
    assert(output.command == EPS_COMMAND_SHED_NONCRITICAL);
    assert(!output.payload_enabled);
}

static void test_deep_eclipse_enters_survival(void) {
    satellite_eps_controller_t controller;
    eps_output_t output;
    eps_input_t input;

    satellite_eps_controller_init(&controller);
    input = make_input(false, 42u, 7280u, 4u, 22u, false, false);
    (void)satellite_eps_controller_step(&controller, &input);
    input = make_input(false, 16u, 6940u, 0u, 18u, false, false);
    output = satellite_eps_controller_step(&controller, &input);

    assert(output.state == EPS_STATE_SURVIVAL);
    assert(output.command == EPS_COMMAND_ENTER_SURVIVAL);
    assert(!output.adcs_full_rate);
}

static void test_recovery_and_nominal_restore(void) {
    satellite_eps_controller_t controller;
    eps_output_t output;
    eps_input_t input;

    satellite_eps_controller_init(&controller);
    input = make_input(false, 16u, 6940u, 0u, 18u, false, false);
    output = satellite_eps_controller_step(&controller, &input);
    assert(output.state == EPS_STATE_SURVIVAL);

    input = make_input(true, 34u, 7420u, 48u, 18u, true, false);
    output = satellite_eps_controller_step(&controller, &input);
    assert(output.state == EPS_STATE_RECOVERY);
    assert(output.command == EPS_COMMAND_RESTORE_LOADS);

    input = make_input(true, 62u, 7580u, 58u, 26u, false, false);
    output = satellite_eps_controller_step(&controller, &input);
    assert(output.state == EPS_STATE_NOMINAL);
    assert(output.command == EPS_COMMAND_HOLD_FULL_POWER);
}

static void test_undervoltage_trips_fault(void) {
    satellite_eps_controller_t controller;
    eps_output_t output;
    eps_input_t input;

    satellite_eps_controller_init(&controller);
    input = make_input(false, 18u, 6480u, 0u, 14u, false, false);
    output = satellite_eps_controller_step(&controller, &input);

    assert(output.state == EPS_STATE_FAULT);
    assert(output.fault == EPS_FAULT_BUS_UNDERVOLTAGE);
}

static void test_deep_discharge_fault_has_priority_after_bus_ok(void) {
    satellite_eps_controller_t controller;
    eps_output_t output;
    eps_input_t input;

    satellite_eps_controller_init(&controller);
    input = make_input(false, 7u, 7100u, 0u, 10u, false, false);
    output = satellite_eps_controller_step(&controller, &input);

    assert(output.state == EPS_STATE_FAULT);
    assert(output.fault == EPS_FAULT_BATTERY_DEEP_DISCHARGE);
}

static void test_reset_clears_fault_when_safe(void) {
    satellite_eps_controller_t controller;
    eps_output_t output;
    eps_input_t input;

    satellite_eps_controller_init(&controller);
    input = make_input(false, 18u, 6480u, 0u, 14u, false, false);
    (void)satellite_eps_controller_step(&controller, &input);

    input = make_input(true, 66u, 7620u, 56u, 24u, false, true);
    output = satellite_eps_controller_step(&controller, &input);

    assert(output.command == EPS_COMMAND_RESET_EPS);
    assert(output.state == EPS_STATE_NOMINAL);
    assert(output.fault == EPS_FAULT_NONE);
}

int main(void) {
    test_nominal_daylight_keeps_full_power();
    test_eclipse_enters_shed();
    test_deep_eclipse_enters_survival();
    test_recovery_and_nominal_restore();
    test_undervoltage_trips_fault();
    test_deep_discharge_fault_has_priority_after_bus_ok();
    test_reset_clears_fault_when_safe();
    return 0;
}
