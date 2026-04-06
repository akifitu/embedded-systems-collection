#ifndef KEYPAD_CONTROLLER_H
#define KEYPAD_CONTROLLER_H

#include <stdint.h>

#include "baseline_tracker.h"
#include "touch_types.h"

typedef struct {
    baseline_tracker_t tracker;
    uint8_t stable_mask;
    uint8_t candidate_mask;
    uint8_t debounce_ticks;
    uint8_t hold_ticks;
    bool hold_emitted;
} keypad_controller_t;

void keypad_controller_init(keypad_controller_t *controller);
touch_output_t keypad_controller_step(keypad_controller_t *controller,
                                      const touch_sample_t *sample);

#endif
