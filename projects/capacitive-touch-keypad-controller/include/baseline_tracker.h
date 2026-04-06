#ifndef BASELINE_TRACKER_H
#define BASELINE_TRACKER_H

#include <stdbool.h>
#include <stdint.h>

#include "touch_types.h"

typedef struct {
    uint16_t baselines[TOUCH_KEY_COUNT];
    bool initialized;
} baseline_tracker_t;

void baseline_tracker_init(baseline_tracker_t *tracker);
void baseline_tracker_step(baseline_tracker_t *tracker, const touch_sample_t *sample,
                           bool allow_adaptation, uint16_t deltas[TOUCH_KEY_COUNT]);

#endif
