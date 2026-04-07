#include <stdio.h>

#include "solar_mppt_controller.h"

static const char *open_closed(bool value) {
    return value ? "CLOSED" : "OPEN";
}

static const char *on_off(bool value) {
    return value ? "ON" : "OFF";
}

static double mv_to_v(unsigned value) {
    return (double)value / 1000.0;
}

static double ma_to_a(unsigned value) {
    return (double)value / 1000.0;
}

static double dmw_to_w(unsigned value) {
    return (double)value / 10.0;
}

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

static void print_phase(const char *phase, const solar_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s fault=%s duty=%u relay=%s fan=%s "
        "stage_target=%umV panel=%.1fV %.1fA power=%.1fW batt=%.1fV temp=%uC\n",
        phase, solar_state_name(output->state),
        solar_command_name(output->command), solar_fault_name(output->fault),
        output->duty_pct, open_closed(output->relay_closed),
        on_off(output->fan_active), output->stage_target_mv,
        mv_to_v(output->panel_mv), ma_to_a(output->panel_ma),
        dmw_to_w(output->panel_power_dmW), mv_to_v(output->battery_mv),
        output->charger_temp_c);
}

int main(void) {
    solar_mppt_controller_t controller;
    solar_mppt_controller_t fault_controller;
    solar_output_t output;
    solar_input_t input;

    solar_mppt_controller_init(&controller);
    input = make_input(false, false, false, 14000u, 400u, 11800u, 29u);
    output = solar_mppt_controller_step(&controller, &input);
    print_phase("dawn_idle", &output);

    input = make_input(true, false, false, 18000u, 4200u, 12500u, 34u);
    output = solar_mppt_controller_step(&controller, &input);
    print_phase("bulk_track", &output);

    input = make_input(true, false, false, 16800u, 3900u, 13200u, 36u);
    output = solar_mppt_controller_step(&controller, &input);
    print_phase("cloud_retrack", &output);

    input = make_input(true, false, false, 17200u, 3100u, 14400u, 44u);
    output = solar_mppt_controller_step(&controller, &input);
    print_phase("absorb_hold", &output);

    input = make_input(true, false, false, 17800u, 1400u, 13700u, 35u);
    output = solar_mppt_controller_step(&controller, &input);
    print_phase("float_maintain", &output);

    solar_mppt_controller_init(&fault_controller);
    input = make_input(true, false, false, 18000u, 4200u, 13300u, 35u);
    (void)solar_mppt_controller_step(&fault_controller, &input);
    input = make_input(true, false, false, 17000u, 2000u, 13900u, 83u);
    output = solar_mppt_controller_step(&fault_controller, &input);
    print_phase("thermal_fault", &output);

    input = make_input(false, true, false, 15000u, 800u, 12900u, 31u);
    output = solar_mppt_controller_step(&fault_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
