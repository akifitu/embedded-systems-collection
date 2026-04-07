#include <assert.h>

#include "generator_controller.h"

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

static void test_utility_loss_enters_start_delay(void) {
    generator_controller_t controller;
    gen_output_t output;

    generator_controller_init(&controller);
    output = generator_controller_step(
        &controller, &(gen_input_t){false, 25100u, 0u, 0u, true, false, false});

    assert(output.state == GEN_STATE_START_DELAY);
    assert(output.command == GEN_COMMAND_PREHEAT);
    assert(output.start_attempts == 0u);
}

static void test_successful_start_reaches_running(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t input;

    generator_controller_init(&controller);
    input = make_input(false, 25100u, 0u, 0u, true, false, false);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_START_DELAY);

    input = make_input(false, 24600u, 280u, 20u, true, false, false);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_CRANKING);
    assert(output.start_attempts == 1u);

    input = make_input(false, 25000u, 1500u, 265u, true, false, false);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_WARMUP);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_RUNNING);
    assert(output.contactor_closed);
}

static void test_utility_restore_enters_cooldown(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t input;

    generator_controller_init(&controller);
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 25100u, 0u, 0u, true, false, false});
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 24600u, 280u, 20u, true, false, false});
    input = make_input(false, 25000u, 1500u, 265u, true, false, false);
    (void)generator_controller_step(&controller, &input);
    (void)generator_controller_step(&controller, &input);

    input = make_input(true, 25100u, 1490u, 255u, true, false, false);
    output = generator_controller_step(&controller, &input);

    assert(output.state == GEN_STATE_COOLDOWN);
    assert(output.command == GEN_COMMAND_COOL_ENGINE);
    assert(!output.contactor_closed);
}

static void test_low_oil_fault_trips_running_unit(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t input;

    generator_controller_init(&controller);
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 25100u, 0u, 0u, true, false, false});
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 24600u, 280u, 20u, true, false, false});
    input = make_input(false, 25000u, 1500u, 265u, true, false, false);
    (void)generator_controller_step(&controller, &input);
    (void)generator_controller_step(&controller, &input);

    input = make_input(false, 24900u, 1460u, 90u, true, false, false);
    output = generator_controller_step(&controller, &input);

    assert(output.state == GEN_STATE_FAULT);
    assert(output.fault == GEN_FAULT_LOW_OIL);
}

static void test_crank_retry_exhaustion_faults(void) {
    generator_controller_t controller;
    gen_output_t output;
    gen_input_t input;

    generator_controller_init(&controller);
    input = make_input(false, 25100u, 0u, 0u, true, false, false);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_START_DELAY);

    input = make_input(false, 24600u, 200u, 15u, true, false, false);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_CRANKING);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_CRANKING);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_START_DELAY);

    input = make_input(false, 24400u, 220u, 15u, true, false, false);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_CRANKING);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_CRANKING);
    output = generator_controller_step(&controller, &input);
    assert(output.state == GEN_STATE_FAULT);
    assert(output.fault == GEN_FAULT_START_FAIL);
}

static void test_reset_clears_fault(void) {
    generator_controller_t controller;
    gen_output_t output;

    generator_controller_init(&controller);
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 25100u, 0u, 0u, true, false, false});
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 24600u, 280u, 20u, true, false, false});
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 25000u, 1500u, 265u, true, false, false});
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 25000u, 1500u, 265u, true, false, false});
    (void)generator_controller_step(
        &controller, &(gen_input_t){false, 24900u, 1460u, 90u, true, false, false});

    output = generator_controller_step(
        &controller, &(gen_input_t){true, 25300u, 0u, 0u, true, false, true});

    assert(output.state == GEN_STATE_IDLE);
    assert(output.fault == GEN_FAULT_NONE);
    assert(output.start_attempts == 0u);
}

int main(void) {
    test_utility_loss_enters_start_delay();
    test_successful_start_reaches_running();
    test_utility_restore_enters_cooldown();
    test_low_oil_fault_trips_running_unit();
    test_crank_retry_exhaustion_faults();
    test_reset_clears_fault();
    return 0;
}
