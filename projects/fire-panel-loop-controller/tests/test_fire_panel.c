#include <assert.h>

#include "fire_panel_controller.h"

static fire_panel_input_t make_input(bool smoke_alarm, bool manual_pull,
                                     bool loop_open, bool sensor_dirty,
                                     bool silence_request, bool reset_request) {
    fire_panel_input_t input;

    input.smoke_alarm = smoke_alarm;
    input.manual_pull = manual_pull;
    input.loop_open = loop_open;
    input.sensor_dirty = sensor_dirty;
    input.silence_request = silence_request;
    input.reset_request = reset_request;
    return input;
}

static void test_idle_monitoring(void) {
    fire_panel_controller_t controller;
    fire_panel_output_t output;
    fire_panel_input_t input;

    fire_panel_controller_init(&controller);
    input = make_input(false, false, false, false, false, false);
    output = fire_panel_controller_step(&controller, &input);

    assert(output.state == PANEL_STATE_IDLE);
    assert(output.command == PANEL_COMMAND_MONITOR_LOOP);
    assert(output.alarm == PANEL_ALARM_NONE);
    assert(output.trouble == PANEL_TROUBLE_NONE);
}

static void test_smoke_verifies_then_alarms(void) {
    fire_panel_controller_t controller;
    fire_panel_output_t output;
    fire_panel_input_t input;

    fire_panel_controller_init(&controller);
    input = make_input(true, false, false, false, false, false);
    output = fire_panel_controller_step(&controller, &input);
    assert(output.state == PANEL_STATE_VERIFY);
    assert(output.verify_percent == 50u);

    output = fire_panel_controller_step(&controller, &input);
    assert(output.state == PANEL_STATE_ALARM);
    assert(output.command == PANEL_COMMAND_ACTIVATE_NACS);
    assert(output.alarm == PANEL_ALARM_SMOKE);
    assert(output.nac_active);
}

static void test_manual_pull_bypasses_verification(void) {
    fire_panel_controller_t controller;
    fire_panel_output_t output;
    fire_panel_input_t input;

    fire_panel_controller_init(&controller);
    input = make_input(false, true, false, false, false, false);
    output = fire_panel_controller_step(&controller, &input);

    assert(output.state == PANEL_STATE_ALARM);
    assert(output.alarm == PANEL_ALARM_MANUAL_PULL);
    assert(output.verify_percent == 0u);
}

static void test_alarm_can_be_silenced(void) {
    fire_panel_controller_t controller;
    fire_panel_output_t output;
    fire_panel_input_t input;

    fire_panel_controller_init(&controller);
    input = make_input(true, false, false, false, false, false);
    (void)fire_panel_controller_step(&controller, &input);
    (void)fire_panel_controller_step(&controller, &input);

    input = make_input(false, false, false, false, true, false);
    output = fire_panel_controller_step(&controller, &input);

    assert(output.state == PANEL_STATE_SILENCED);
    assert(output.command == PANEL_COMMAND_SILENCE_NACS);
    assert(!output.nac_active);
    assert(output.alarm_led);
}

static void test_trouble_latches_until_reset(void) {
    fire_panel_controller_t controller;
    fire_panel_output_t output;
    fire_panel_input_t input;

    fire_panel_controller_init(&controller);
    input = make_input(false, false, true, false, false, false);
    output = fire_panel_controller_step(&controller, &input);
    assert(output.state == PANEL_STATE_TROUBLE);
    assert(output.trouble == PANEL_TROUBLE_LOOP_OPEN);

    input = make_input(false, false, false, false, false, false);
    output = fire_panel_controller_step(&controller, &input);
    assert(output.state == PANEL_STATE_TROUBLE);
    assert(output.trouble == PANEL_TROUBLE_LOOP_OPEN);

    input = make_input(false, false, false, false, false, true);
    output = fire_panel_controller_step(&controller, &input);
    assert(output.state == PANEL_STATE_IDLE);
    assert(output.command == PANEL_COMMAND_RESET_PANEL);
}

static void test_sensor_dirty_maps_to_trouble(void) {
    fire_panel_controller_t controller;
    fire_panel_output_t output;
    fire_panel_input_t input;

    fire_panel_controller_init(&controller);
    input = make_input(false, false, false, true, false, false);
    output = fire_panel_controller_step(&controller, &input);

    assert(output.state == PANEL_STATE_TROUBLE);
    assert(output.trouble == PANEL_TROUBLE_SENSOR_DIRTY);
}

static void test_manual_pull_overrides_trouble(void) {
    fire_panel_controller_t controller;
    fire_panel_output_t output;
    fire_panel_input_t input;

    fire_panel_controller_init(&controller);
    input = make_input(false, false, true, false, false, false);
    (void)fire_panel_controller_step(&controller, &input);

    input = make_input(false, true, false, false, false, false);
    output = fire_panel_controller_step(&controller, &input);

    assert(output.state == PANEL_STATE_ALARM);
    assert(output.alarm == PANEL_ALARM_MANUAL_PULL);
    assert(output.trouble == PANEL_TROUBLE_LOOP_OPEN);
}

int main(void) {
    test_idle_monitoring();
    test_smoke_verifies_then_alarms();
    test_manual_pull_bypasses_verification();
    test_alarm_can_be_silenced();
    test_trouble_latches_until_reset();
    test_sensor_dirty_maps_to_trouble();
    test_manual_pull_overrides_trouble();
    return 0;
}
