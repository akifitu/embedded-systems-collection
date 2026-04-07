#include <assert.h>

#include "inverter_guard.h"

static inverter_input_t make_input(bool grid_present, uint16_t voltage_rms_v,
                                   uint32_t frequency_mhz,
                                   uint16_t rocof_mhz_s,
                                   int16_t heatsink_c_x10, uint16_t dc_link_v,
                                   bool relay_closed_fb) {
    inverter_input_t input;

    input.grid_present = grid_present;
    input.voltage_rms_v = voltage_rms_v;
    input.frequency_mhz = frequency_mhz;
    input.rocof_mhz_s = rocof_mhz_s;
    input.heatsink_c_x10 = heatsink_c_x10;
    input.dc_link_v = dc_link_v;
    input.relay_closed_fb = relay_closed_fb;
    return input;
}

static void test_stable_grid_enters_export(void) {
    inverter_guard_t guard;
    inverter_input_t input;
    inverter_output_t output = {0};
    unsigned i;

    inverter_guard_init(&guard);
    input = make_input(true, 230u, 50000u, 80u, 450, 390u, true);
    for (i = 0u; i < 5u; ++i) {
        output = inverter_guard_step(&guard, &input);
    }

    assert(output.state == INVERTER_STATE_EXPORT);
    assert(output.command == INVERTER_COMMAND_EXPORT_POWER);
    assert(output.quality == INVERTER_QUALITY_LOCKED);
    assert(output.power_limit_pct == 100u);
}

static void test_thermal_derate_limits_export(void) {
    inverter_guard_t guard;
    inverter_input_t input;
    inverter_output_t output = {0};
    unsigned i;

    inverter_guard_init(&guard);
    input = make_input(true, 230u, 50000u, 80u, 910, 390u, true);
    for (i = 0u; i < 5u; ++i) {
        output = inverter_guard_step(&guard, &input);
    }

    assert(output.state == INVERTER_STATE_EXPORT);
    assert(output.quality == INVERTER_QUALITY_DERATED);
    assert(output.power_limit_pct == 68u);
}

static void test_undervoltage_trips_guard(void) {
    inverter_guard_t guard;
    inverter_input_t input;
    inverter_output_t output = {0};
    unsigned i;

    inverter_guard_init(&guard);
    input = make_input(true, 230u, 50000u, 80u, 450, 390u, true);
    for (i = 0u; i < 5u; ++i) {
        output = inverter_guard_step(&guard, &input);
    }
    input = make_input(true, 187u, 50000u, 80u, 450, 390u, true);
    output = inverter_guard_step(&guard, &input);

    assert(output.state == INVERTER_STATE_TRIP);
    assert(output.command == INVERTER_COMMAND_OPEN_RELAY);
    assert(output.reason == INVERTER_REASON_UNDERVOLTAGE);
}

static void test_relay_feedback_loss_trips_guard(void) {
    inverter_guard_t guard;
    inverter_input_t input;
    inverter_output_t output = {0};
    unsigned i;

    inverter_guard_init(&guard);
    input = make_input(true, 230u, 50000u, 80u, 450, 390u, true);
    for (i = 0u; i < 5u; ++i) {
        output = inverter_guard_step(&guard, &input);
    }
    input = make_input(true, 230u, 50000u, 80u, 450, 390u, false);
    output = inverter_guard_step(&guard, &input);

    assert(output.state == INVERTER_STATE_TRIP);
    assert(output.reason == INVERTER_REASON_RELAY_FAULT);
}

static void test_cooldown_recovery_relocks_export(void) {
    inverter_guard_t guard;
    inverter_input_t closed_input;
    inverter_input_t open_input;
    inverter_output_t output = {0};
    unsigned i;

    inverter_guard_init(&guard);
    closed_input = make_input(true, 230u, 50000u, 80u, 450, 390u, true);
    for (i = 0u; i < 5u; ++i) {
        output = inverter_guard_step(&guard, &closed_input);
    }

    output = inverter_guard_step(
        &guard, &(inverter_input_t){true, 187u, 50000u, 80u, 450, 390u, true});
    assert(output.state == INVERTER_STATE_TRIP);

    open_input = make_input(true, 230u, 50000u, 80u, 450, 390u, false);
    for (i = 0u; i < 4u; ++i) {
        output = inverter_guard_step(&guard, &open_input);
    }
    for (i = 0u; i < 5u; ++i) {
        output = inverter_guard_step(&guard, &closed_input);
    }

    assert(output.state == INVERTER_STATE_EXPORT);
    assert(output.reason == INVERTER_REASON_NONE);
    assert(output.command == INVERTER_COMMAND_EXPORT_POWER);
}

int main(void) {
    test_stable_grid_enters_export();
    test_thermal_derate_limits_export();
    test_undervoltage_trips_guard();
    test_relay_feedback_loss_trips_guard();
    test_cooldown_recovery_relocks_export();
    return 0;
}
