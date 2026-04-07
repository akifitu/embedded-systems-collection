#include <assert.h>

#include "solar_mppt_controller.h"

static solar_input_t make_input(bool charge_enable, bool reset_request,
                                bool reverse_polarity, unsigned panel_mv,
                                unsigned panel_ma, unsigned battery_mv,
                                unsigned charger_temp_c) {
    solar_input_t input;

    input.charge_enable = charge_enable;
    input.reset_request = reset_request;
    input.reverse_polarity = reverse_polarity;
    input.panel_mv = panel_mv;
    input.panel_ma = panel_ma;
    input.battery_mv = battery_mv;
    input.charger_temp_c = charger_temp_c;
    return input;
}

static void test_idle_opens_relay_when_disabled(void) {
    solar_mppt_controller_t controller;
    solar_output_t output;
    solar_input_t input;

    solar_mppt_controller_init(&controller);
    input = make_input(false, false, false, 14000u, 400u, 11800u, 29u);
    output = solar_mppt_controller_step(&controller, &input);

    assert(output.state == SOLAR_STATE_IDLE);
    assert(output.command == SOLAR_COMMAND_OPEN_RELAY);
    assert(!output.relay_closed);
}

static void test_bulk_starts_and_tracks_mpp(void) {
    solar_mppt_controller_t controller;
    solar_output_t output;
    solar_input_t input;

    solar_mppt_controller_init(&controller);
    input = make_input(true, false, false, 18000u, 4200u, 12500u, 34u);
    output = solar_mppt_controller_step(&controller, &input);

    assert(output.state == SOLAR_STATE_BULK);
    assert(output.command == SOLAR_COMMAND_SEEK_MPP);
    assert(output.relay_closed);
    assert(output.duty_pct == 52u);
    assert(output.panel_power_dmW == 756u);
}

static void test_bulk_enters_absorb_then_float(void) {
    solar_mppt_controller_t controller;
    solar_output_t output;
    solar_input_t input;

    solar_mppt_controller_init(&controller);
    input = make_input(true, false, false, 18000u, 4200u, 12500u, 34u);
    (void)solar_mppt_controller_step(&controller, &input);
    input = make_input(true, false, false, 17200u, 3100u, 14400u, 44u);
    output = solar_mppt_controller_step(&controller, &input);
    assert(output.state == SOLAR_STATE_ABSORB);
    assert(output.command == SOLAR_COMMAND_HOLD_ABSORB);
    assert(output.fan_active);

    input = make_input(true, false, false, 17800u, 1400u, 13700u, 35u);
    output = solar_mppt_controller_step(&controller, &input);
    assert(output.state == SOLAR_STATE_FLOAT);
    assert(output.command == SOLAR_COMMAND_HOLD_FLOAT);
    assert(output.duty_pct == 28u);
}

static void test_float_rebulk_when_battery_drops(void) {
    solar_mppt_controller_t controller;
    solar_output_t output;
    solar_input_t input;

    solar_mppt_controller_init(&controller);
    input = make_input(true, false, false, 18000u, 4200u, 12500u, 34u);
    (void)solar_mppt_controller_step(&controller, &input);
    input = make_input(true, false, false, 17200u, 3100u, 14400u, 44u);
    (void)solar_mppt_controller_step(&controller, &input);
    input = make_input(true, false, false, 17800u, 1400u, 13700u, 35u);
    (void)solar_mppt_controller_step(&controller, &input);
    input = make_input(true, false, false, 18100u, 2600u, 13000u, 33u);
    output = solar_mppt_controller_step(&controller, &input);

    assert(output.state == SOLAR_STATE_BULK);
    assert(output.command == SOLAR_COMMAND_SEEK_MPP);
}

static void test_overtemp_latches_fault(void) {
    solar_mppt_controller_t controller;
    solar_output_t output;
    solar_input_t input;

    solar_mppt_controller_init(&controller);
    input = make_input(true, false, false, 18000u, 4200u, 13300u, 35u);
    (void)solar_mppt_controller_step(&controller, &input);
    input = make_input(true, false, false, 17000u, 2000u, 13900u, 83u);
    output = solar_mppt_controller_step(&controller, &input);

    assert(output.state == SOLAR_STATE_FAULT);
    assert(output.fault == SOLAR_FAULT_OVER_TEMP);
}

static void test_reverse_and_overvolt_faults(void) {
    solar_mppt_controller_t controller;
    solar_output_t output;
    solar_input_t input;

    solar_mppt_controller_init(&controller);
    input = make_input(true, false, true, 17000u, 2000u, 13000u, 30u);
    output = solar_mppt_controller_step(&controller, &input);
    assert(output.fault == SOLAR_FAULT_PANEL_REVERSE);

    solar_mppt_controller_init(&controller);
    input = make_input(true, false, false, 17000u, 2000u, 14950u, 30u);
    output = solar_mppt_controller_step(&controller, &input);
    assert(output.fault == SOLAR_FAULT_BATTERY_OVERVOLT);
}

static void test_reset_clears_fault_when_safe(void) {
    solar_mppt_controller_t controller;
    solar_output_t output;
    solar_input_t input;

    solar_mppt_controller_init(&controller);
    input = make_input(true, false, false, 18000u, 4200u, 13300u, 35u);
    (void)solar_mppt_controller_step(&controller, &input);
    input = make_input(true, false, false, 17000u, 2000u, 13900u, 83u);
    (void)solar_mppt_controller_step(&controller, &input);
    input = make_input(false, true, false, 15000u, 800u, 12900u, 31u);
    output = solar_mppt_controller_step(&controller, &input);

    assert(output.state == SOLAR_STATE_IDLE);
    assert(output.command == SOLAR_COMMAND_RESET_CHARGER);
    assert(output.fault == SOLAR_FAULT_NONE);
}

int main(void) {
    test_idle_opens_relay_when_disabled();
    test_bulk_starts_and_tracks_mpp();
    test_bulk_enters_absorb_then_float();
    test_float_rebulk_when_battery_drops();
    test_overtemp_latches_fault();
    test_reverse_and_overvolt_faults();
    test_reset_clears_fault_when_safe();
    return 0;
}
