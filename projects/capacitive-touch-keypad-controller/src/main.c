#include <stdio.h>

#include "keypad_controller.h"

static touch_sample_t make_sample(uint16_t up, uint16_t down, uint16_t ok,
                                  uint16_t back, uint16_t shield) {
    touch_sample_t sample;

    sample.raw[0] = up;
    sample.raw[1] = down;
    sample.raw[2] = ok;
    sample.raw[3] = back;
    sample.shield_raw = shield;
    return sample;
}

static void print_phase(const char *phase, const touch_output_t *output) {
    char fault_buffer[32];

    touch_faults_to_string(output->faults, fault_buffer, sizeof(fault_buffer));
    printf(
        "phase=%s active=%s event=%s moisture=%s faults=%s baselines=%u/%u/%u/%u\n",
        phase, touch_mask_name(output->active_mask),
        touch_event_name(output->event), touch_moisture_name(output->moisture),
        fault_buffer, output->baselines[0], output->baselines[1],
        output->baselines[2], output->baselines[3]);
}

int main(void) {
    keypad_controller_t controller;
    touch_output_t output;
    touch_sample_t sample;
    unsigned i;

    keypad_controller_init(&controller);
    sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
    output = keypad_controller_step(&controller, &sample);
    print_phase("idle", &output);

    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1000u, 1004u, 1078u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    print_phase("tap_ok", &output);

    for (i = 0u; i < 5u; ++i) {
        sample = make_sample(1000u, 1004u, 998u, 1085u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    print_phase("hold_back", &output);
    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }

    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1070u, 1004u, 1075u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    print_phase("combo", &output);

    sample = make_sample(1040u, 1042u, 1045u, 1038u, 1400u);
    output = keypad_controller_step(&controller, &sample);
    print_phase("moisture", &output);

    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1000u, 1082u, 998u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    print_phase("recovery", &output);

    return 0;
}
