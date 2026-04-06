#include "baseline_tracker.h"

void baseline_tracker_init(baseline_tracker_t *tracker) {
    unsigned i;

    for (i = 0u; i < TOUCH_KEY_COUNT; ++i) {
        tracker->baselines[i] = 0u;
    }
    tracker->initialized = false;
}

void baseline_tracker_step(baseline_tracker_t *tracker, const touch_sample_t *sample,
                           bool allow_adaptation, uint16_t deltas[TOUCH_KEY_COUNT]) {
    unsigned i;

    if (!tracker->initialized) {
        for (i = 0u; i < TOUCH_KEY_COUNT; ++i) {
            tracker->baselines[i] = sample->raw[i];
            deltas[i] = 0u;
        }
        tracker->initialized = true;
        return;
    }

    for (i = 0u; i < TOUCH_KEY_COUNT; ++i) {
        if (sample->raw[i] > tracker->baselines[i]) {
            deltas[i] = (uint16_t)(sample->raw[i] - tracker->baselines[i]);
        } else {
            deltas[i] = 0u;
        }

        if (allow_adaptation && sample->raw[i] < tracker->baselines[i]) {
            tracker->baselines[i] =
                (uint16_t)(tracker->baselines[i] - 1u);
        } else if (allow_adaptation && sample->raw[i] > tracker->baselines[i] &&
                   deltas[i] < 20u) {
            tracker->baselines[i] =
                (uint16_t)(tracker->baselines[i] + 1u);
        }
    }
}
