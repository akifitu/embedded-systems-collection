#include "keypad_controller.h"

enum {
    TOUCH_DELTA_THRESHOLD = 60u,
    TOUCH_DEBOUNCE_TICKS = 2u,
    TOUCH_HOLD_TICKS = 4u,
    TOUCH_FAULT_MOISTURE = 1u << 0,
    TOUCH_SHIELD_WET_THRESHOLD = 1300u
};

static touch_event_t tap_event_for_mask(uint8_t mask) {
    switch (mask) {
        case TOUCH_KEY_UP:
            return TOUCH_EVENT_TAP_UP;
        case TOUCH_KEY_DOWN:
            return TOUCH_EVENT_TAP_DOWN;
        case TOUCH_KEY_OK:
            return TOUCH_EVENT_TAP_OK;
        case TOUCH_KEY_BACK:
            return TOUCH_EVENT_TAP_BACK;
        case TOUCH_KEY_UP | TOUCH_KEY_OK:
            return TOUCH_EVENT_COMBO_UP_OK;
        default:
            return TOUCH_EVENT_NONE;
    }
}

static uint8_t mask_from_deltas(const uint16_t deltas[TOUCH_KEY_COUNT]) {
    uint8_t mask = 0u;

    if (deltas[0] >= TOUCH_DELTA_THRESHOLD) {
        mask |= TOUCH_KEY_UP;
    }
    if (deltas[1] >= TOUCH_DELTA_THRESHOLD) {
        mask |= TOUCH_KEY_DOWN;
    }
    if (deltas[2] >= TOUCH_DELTA_THRESHOLD) {
        mask |= TOUCH_KEY_OK;
    }
    if (deltas[3] >= TOUCH_DELTA_THRESHOLD) {
        mask |= TOUCH_KEY_BACK;
    }
    return mask;
}

void keypad_controller_init(keypad_controller_t *controller) {
    baseline_tracker_init(&controller->tracker);
    controller->stable_mask = 0u;
    controller->candidate_mask = 0u;
    controller->debounce_ticks = 0u;
    controller->hold_ticks = 0u;
    controller->hold_emitted = false;
}

touch_output_t keypad_controller_step(keypad_controller_t *controller,
                                      const touch_sample_t *sample) {
    touch_output_t output;
    uint16_t deltas[TOUCH_KEY_COUNT];
    uint8_t raw_mask;
    bool wet;
    bool allow_adaptation;

    wet = sample->shield_raw >= TOUCH_SHIELD_WET_THRESHOLD;
    allow_adaptation = !wet && controller->stable_mask == 0u;
    baseline_tracker_step(&controller->tracker, sample, allow_adaptation, deltas);
    raw_mask = wet ? 0u : mask_from_deltas(deltas);

    output.event = TOUCH_EVENT_NONE;
    output.faults = wet ? TOUCH_FAULT_MOISTURE : 0u;
    output.moisture = wet ? TOUCH_MOISTURE_WET : TOUCH_MOISTURE_DRY;

    if (wet) {
        controller->stable_mask = 0u;
        controller->candidate_mask = 0u;
        controller->debounce_ticks = 0u;
        controller->hold_ticks = 0u;
        controller->hold_emitted = false;
    } else {
        if (raw_mask == controller->candidate_mask) {
            if (controller->debounce_ticks < TOUCH_DEBOUNCE_TICKS) {
                controller->debounce_ticks++;
            }
        } else {
            controller->candidate_mask = raw_mask;
            controller->debounce_ticks = 1u;
        }

        if (controller->debounce_ticks >= TOUCH_DEBOUNCE_TICKS &&
            controller->stable_mask != controller->candidate_mask) {
            if (controller->stable_mask != 0u && controller->candidate_mask == 0u &&
                !controller->hold_emitted) {
                output.event = tap_event_for_mask(controller->stable_mask);
            }
            controller->stable_mask = controller->candidate_mask;
            controller->hold_ticks = 0u;
            controller->hold_emitted = false;
        }

        if (controller->stable_mask != 0u) {
            if (controller->hold_ticks < 255u) {
                controller->hold_ticks++;
            }
            if (!controller->hold_emitted &&
                controller->stable_mask == TOUCH_KEY_BACK &&
                controller->hold_ticks >= TOUCH_HOLD_TICKS) {
                output.event = TOUCH_EVENT_HOLD_BACK;
                controller->hold_emitted = true;
            }
        }
    }

    output.active_mask = controller->stable_mask;
    output.baselines[0] = controller->tracker.baselines[0];
    output.baselines[1] = controller->tracker.baselines[1];
    output.baselines[2] = controller->tracker.baselines[2];
    output.baselines[3] = controller->tracker.baselines[3];
    return output;
}
