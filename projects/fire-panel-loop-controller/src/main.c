#include <stdio.h>

#include "fire_panel_controller.h"

static const char *on_off(bool value) {
    return value ? "ON" : "OFF";
}

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

static void print_phase(const char *phase, const fire_panel_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s alarm=%s trouble=%s nac=%s buzzer=%s "
        "alarm_led=%s trouble_led=%s verify=%u\n",
        phase, panel_state_name(output->state), panel_command_name(output->command),
        panel_alarm_name(output->alarm), panel_trouble_name(output->trouble),
        on_off(output->nac_active), on_off(output->buzzer_active),
        on_off(output->alarm_led), on_off(output->trouble_led),
        output->verify_percent);
}

int main(void) {
    fire_panel_controller_t smoke_controller;
    fire_panel_controller_t pull_controller;
    fire_panel_controller_t trouble_controller;
    fire_panel_output_t output;
    fire_panel_input_t input;

    fire_panel_controller_init(&smoke_controller);
    input = make_input(false, false, false, false, false, false);
    output = fire_panel_controller_step(&smoke_controller, &input);
    print_phase("idle", &output);

    input = make_input(true, false, false, false, false, false);
    output = fire_panel_controller_step(&smoke_controller, &input);
    print_phase("verify_smoke", &output);

    output = fire_panel_controller_step(&smoke_controller, &input);
    print_phase("confirmed_alarm", &output);

    input = make_input(false, false, false, false, true, false);
    output = fire_panel_controller_step(&smoke_controller, &input);
    print_phase("silenced", &output);

    fire_panel_controller_init(&pull_controller);
    input = make_input(false, true, false, false, false, false);
    output = fire_panel_controller_step(&pull_controller, &input);
    print_phase("manual_pull", &output);

    fire_panel_controller_init(&trouble_controller);
    input = make_input(false, false, true, false, false, false);
    output = fire_panel_controller_step(&trouble_controller, &input);
    print_phase("trouble_loop", &output);

    input = make_input(false, false, false, false, false, true);
    output = fire_panel_controller_step(&trouble_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
