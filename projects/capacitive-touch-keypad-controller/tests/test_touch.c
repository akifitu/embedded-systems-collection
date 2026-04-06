#include <assert.h>

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

static void test_baseline_initializes(void) {
    keypad_controller_t controller;
    touch_output_t output;
    touch_sample_t sample;

    keypad_controller_init(&controller);
    sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
    output = keypad_controller_step(&controller, &sample);
    assert(output.baselines[0] == 1000u);
    assert(output.baselines[1] == 1004u);
    assert(output.event == TOUCH_EVENT_NONE);
}

static void test_tap_ok_detected(void) {
    keypad_controller_t controller;
    touch_output_t output = {0};
    touch_sample_t sample;
    unsigned i;

    keypad_controller_init(&controller);
    sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
    (void)keypad_controller_step(&controller, &sample);
    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1000u, 1004u, 1078u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    assert(output.event == TOUCH_EVENT_TAP_OK);
}

static void test_hold_back_detected(void) {
    keypad_controller_t controller;
    touch_output_t output = {0};
    touch_sample_t sample;
    unsigned i;

    keypad_controller_init(&controller);
    sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
    (void)keypad_controller_step(&controller, &sample);
    for (i = 0u; i < 5u; ++i) {
        sample = make_sample(1000u, 1004u, 998u, 1085u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    assert(output.event == TOUCH_EVENT_HOLD_BACK);
    assert(output.active_mask == TOUCH_KEY_BACK);
}

static void test_combo_detected(void) {
    keypad_controller_t controller;
    touch_output_t output = {0};
    touch_sample_t sample;
    unsigned i;

    keypad_controller_init(&controller);
    sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
    (void)keypad_controller_step(&controller, &sample);
    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1070u, 1004u, 1075u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    for (i = 0u; i < 2u; ++i) {
        sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
        output = keypad_controller_step(&controller, &sample);
    }
    assert(output.event == TOUCH_EVENT_COMBO_UP_OK);
}

static void test_moisture_suppresses_touch(void) {
    keypad_controller_t controller;
    touch_output_t output;
    touch_sample_t sample;

    keypad_controller_init(&controller);
    sample = make_sample(1000u, 1004u, 998u, 1002u, 900u);
    (void)keypad_controller_step(&controller, &sample);
    sample = make_sample(1040u, 1042u, 1045u, 1038u, 1400u);
    output = keypad_controller_step(&controller, &sample);
    assert(output.moisture == TOUCH_MOISTURE_WET);
    assert(output.active_mask == 0u);
    assert(output.faults != 0u);
}

int main(void) {
    test_baseline_initializes();
    test_tap_ok_detected();
    test_hold_back_detected();
    test_combo_detected();
    test_moisture_suppresses_touch();
    return 0;
}
