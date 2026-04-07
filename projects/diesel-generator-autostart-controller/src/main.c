#include <stdio.h>

#include "generator_controller.h"

static void print_phase(const char *label, const gen_input_t *input,
                        const gen_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s utility=%s rpm=%u batt=%u.%uV tries=%u "
        "contactor=%s fault=%s\n",
        label, gen_state_name(output->state), gen_command_name(output->command),
        input->utility_present ? "ON" : "OFF", input->engine_rpm,
        input->battery_mv / 1000u, (input->battery_mv % 1000u) / 100u,
        output->start_attempts, output->contactor_closed ? "CLOSED" : "OPEN",
        gen_fault_name(output->fault));
}

static gen_input_t make_input(bool utility_present, uint16_t battery_mv,
                              uint16_t engine_rpm, uint16_t oil_pressure_kpa,
                              bool fuel_ok, bool emergency_stop,
                              bool reset_request) {
    gen_input_t input;

    input.utility_present = utility_present;
    input.battery_mv = battery_mv;
    input.engine_rpm = engine_rpm;
    input.oil_pressure_kpa = oil_pressure_kpa;
    input.fuel_ok = fuel_ok;
    input.emergency_stop = emergency_stop;
    input.reset_request = reset_request;
    return input;
}

static void demo_utility_ok(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t input;

    generator_controller_init(&controller);
    input = make_input(true, 25400u, 0u, 0u, true, false, false);
    output = generator_controller_step(&controller, &input);
    print_phase("utility_ok", &input, &output);
}

static void demo_outage_detected(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t input;

    generator_controller_init(&controller);
    input = make_input(false, 25100u, 0u, 0u, true, false, false);
    output = generator_controller_step(&controller, &input);
    print_phase("outage_detected", &input, &output);
}

static void demo_cranking(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t input;

    generator_controller_init(&controller);
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 25100u, 0u, 0u, true, false, false});
    input = make_input(false, 24600u, 280u, 20u, true, false, false);
    output = generator_controller_step(&controller, &input);
    print_phase("cranking", &input, &output);
}

static void demo_warmup(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t start_delay;
    gen_input_t crank_input;
    gen_input_t warmup_input;

    generator_controller_init(&controller);
    start_delay = make_input(false, 25100u, 0u, 0u, true, false, false);
    (void)generator_controller_step(&controller, &start_delay);
    crank_input = make_input(false, 24600u, 280u, 20u, true, false, false);
    (void)generator_controller_step(&controller, &crank_input);
    warmup_input = make_input(false, 24800u, 1450u, 260u, true, false, false);
    output = generator_controller_step(&controller, &warmup_input);
    print_phase("warmup", &warmup_input, &output);
}

static void demo_running(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t start_delay;
    gen_input_t crank_input;
    gen_input_t running_input;

    generator_controller_init(&controller);
    start_delay = make_input(false, 25100u, 0u, 0u, true, false, false);
    (void)generator_controller_step(&controller, &start_delay);
    crank_input = make_input(false, 24600u, 280u, 20u, true, false, false);
    (void)generator_controller_step(&controller, &crank_input);
    running_input = make_input(false, 25000u, 1500u, 265u, true, false, false);
    (void)generator_controller_step(&controller, &running_input);
    output = generator_controller_step(&controller, &running_input);
    print_phase("running", &running_input, &output);
}

static void demo_utility_restore(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t start_delay;
    gen_input_t crank_input;
    gen_input_t running_input;
    gen_input_t utility_restore;

    generator_controller_init(&controller);
    start_delay = make_input(false, 25100u, 0u, 0u, true, false, false);
    (void)generator_controller_step(&controller, &start_delay);
    crank_input = make_input(false, 24600u, 280u, 20u, true, false, false);
    (void)generator_controller_step(&controller, &crank_input);
    running_input = make_input(false, 25000u, 1500u, 265u, true, false, false);
    (void)generator_controller_step(&controller, &running_input);
    (void)generator_controller_step(&controller, &running_input);
    utility_restore = make_input(true, 25100u, 1490u, 255u, true, false, false);
    output = generator_controller_step(&controller, &utility_restore);
    print_phase("utility_restore", &utility_restore, &output);
}

static void demo_low_oil_fault(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t start_delay;
    gen_input_t crank_input;
    gen_input_t running_input;
    gen_input_t low_oil_input;

    generator_controller_init(&controller);
    start_delay = make_input(false, 25100u, 0u, 0u, true, false, false);
    (void)generator_controller_step(&controller, &start_delay);
    crank_input = make_input(false, 24600u, 280u, 20u, true, false, false);
    (void)generator_controller_step(&controller, &crank_input);
    running_input = make_input(false, 25000u, 1500u, 265u, true, false, false);
    (void)generator_controller_step(&controller, &running_input);
    (void)generator_controller_step(&controller, &running_input);
    low_oil_input = make_input(false, 24900u, 1460u, 90u, true, false, false);
    output = generator_controller_step(&controller, &low_oil_input);
    print_phase("low_oil_fault", &low_oil_input, &output);
}

static void demo_reset_ready(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t start_delay;
    gen_input_t crank_input;
    gen_input_t running_input;
    gen_input_t low_oil_input;
    gen_input_t reset_input;

    generator_controller_init(&controller);
    start_delay = make_input(false, 25100u, 0u, 0u, true, false, false);
    (void)generator_controller_step(&controller, &start_delay);
    crank_input = make_input(false, 24600u, 280u, 20u, true, false, false);
    (void)generator_controller_step(&controller, &crank_input);
    running_input = make_input(false, 25000u, 1500u, 265u, true, false, false);
    (void)generator_controller_step(&controller, &running_input);
    (void)generator_controller_step(&controller, &running_input);
    low_oil_input = make_input(false, 24900u, 1460u, 90u, true, false, false);
    (void)generator_controller_step(&controller, &low_oil_input);
    reset_input = make_input(true, 25300u, 0u, 0u, true, false, true);
    output = generator_controller_step(&controller, &reset_input);
    print_phase("reset_ready", &reset_input, &output);
}

int main(void) {
    demo_utility_ok();
    demo_outage_detected();
    demo_cranking();
    demo_warmup();
    demo_running();
    demo_utility_restore();
    demo_low_oil_fault();
    demo_reset_ready();
    return 0;
}
