#include <stdio.h>

#include "inverter_guard.h"
#include "inverter_guard_types.h"

static void print_phase(const char *label, const inverter_input_t *input,
                        const inverter_output_t *output) {
    unsigned long whole_hz;
    unsigned long frac_hz;

    whole_hz = (unsigned long)(input->frequency_mhz / 1000u);
    frac_hz = (unsigned long)(input->frequency_mhz % 1000u);

    printf(
        "phase=%s state=%s cmd=%s reason=%s limit=%u sync=%u v=%u f=%lu.%03luHz "
        "relay=%s quality=%s\n",
        label, inverter_state_name(output->state),
        inverter_command_name(output->command),
        inverter_reason_name(output->reason), output->power_limit_pct,
        output->sync_count, input->voltage_rms_v, whole_hz, frac_hz,
        output->relay_closed_commanded ? "CLOSED" : "OPEN",
        inverter_quality_name(output->quality));
}

static inverter_output_t run_steps(inverter_guard_t *guard,
                                   const inverter_input_t *input,
                                   unsigned steps) {
    inverter_output_t output = {0};
    unsigned i;

    for (i = 0u; i < steps; ++i) {
        output = inverter_guard_step(guard, input);
    }
    return output;
}

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

static void demo_boot_wait(void) {
    inverter_guard_t guard;
    inverter_input_t input;
    inverter_output_t output;

    inverter_guard_init(&guard);
    input = make_input(false, 0u, 0u, 0u, 360, 360u, false);
    output = run_steps(&guard, &input, 1u);
    print_phase("boot_wait", &input, &output);
}

static void demo_syncing(void) {
    inverter_guard_t guard;
    inverter_input_t input;
    inverter_output_t output;

    inverter_guard_init(&guard);
    input = make_input(true, 230u, 50000u, 80u, 450, 390u, false);
    output = run_steps(&guard, &input, 3u);
    print_phase("syncing", &input, &output);
}

static void demo_export(void) {
    inverter_guard_t guard;
    inverter_input_t input;
    inverter_output_t output;

    inverter_guard_init(&guard);
    input = make_input(true, 230u, 50000u, 80u, 450, 390u, true);
    output = run_steps(&guard, &input, 5u);
    print_phase("export", &input, &output);
}

static void demo_thermal_derate(void) {
    inverter_guard_t guard;
    inverter_input_t input;
    inverter_output_t output;

    inverter_guard_init(&guard);
    input = make_input(true, 230u, 50000u, 80u, 910, 390u, true);
    output = run_steps(&guard, &input, 5u);
    print_phase("thermal_derate", &input, &output);
}

static void demo_sag_trip(void) {
    inverter_guard_t guard;
    inverter_input_t good_input;
    inverter_input_t bad_input;
    inverter_output_t output;

    inverter_guard_init(&guard);
    good_input = make_input(true, 230u, 50000u, 80u, 450, 390u, true);
    (void)run_steps(&guard, &good_input, 5u);
    bad_input = make_input(true, 187u, 50000u, 80u, 450, 390u, true);
    output = run_steps(&guard, &bad_input, 1u);
    print_phase("sag_trip", &bad_input, &output);
}

static void demo_cooldown(void) {
    inverter_guard_t guard;
    inverter_input_t good_input;
    inverter_input_t bad_input;
    inverter_output_t output;

    inverter_guard_init(&guard);
    good_input = make_input(true, 230u, 50000u, 80u, 450, 390u, true);
    (void)run_steps(&guard, &good_input, 5u);
    bad_input = make_input(true, 187u, 50000u, 80u, 450, 390u, true);
    (void)run_steps(&guard, &bad_input, 1u);
    good_input.relay_closed_fb = false;
    output = run_steps(&guard, &good_input, 1u);
    print_phase("cooldown", &good_input, &output);
}

static void demo_relock(void) {
    inverter_guard_t guard;
    inverter_input_t open_input;
    inverter_input_t closed_input;
    inverter_output_t output;

    inverter_guard_init(&guard);
    closed_input = make_input(true, 230u, 50000u, 80u, 450, 390u, true);
    (void)run_steps(&guard, &closed_input, 5u);
    output = run_steps(
        &guard, &(inverter_input_t){true, 187u, 50000u, 80u, 450, 390u, true},
        1u);
    (void)output;

    open_input = make_input(true, 230u, 50000u, 80u, 450, 390u, false);
    (void)run_steps(&guard, &open_input, 4u);
    closed_input = make_input(true, 230u, 50000u, 80u, 450, 390u, true);
    output = run_steps(&guard, &closed_input, 5u);
    print_phase("relock", &closed_input, &output);
}

int main(void) {
    demo_boot_wait();
    demo_syncing();
    demo_export();
    demo_thermal_derate();
    demo_sag_trip();
    demo_cooldown();
    demo_relock();
    return 0;
}
