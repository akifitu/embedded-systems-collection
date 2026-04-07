#include "power_tracker.h"

#define DUTY_MIN 10u
#define DUTY_MAX 90u
#define DUTY_STEP 2u

void power_tracker_init(power_tracker_state_t *tracker, unsigned duty_pct) {
    tracker->duty_pct = duty_pct;
    tracker->panel_power_dmW = 0u;
    tracker->direction = 1;
}

void power_tracker_step(power_tracker_state_t *tracker, unsigned panel_power_dmW) {
    unsigned next_duty;

    if (panel_power_dmW < tracker->panel_power_dmW) {
        tracker->direction = -tracker->direction;
    }

    next_duty = tracker->duty_pct;
    if (tracker->direction > 0) {
        if ((DUTY_MAX - next_duty) < DUTY_STEP) {
            next_duty = DUTY_MAX;
            tracker->direction = -1;
        } else {
            next_duty += DUTY_STEP;
        }
    } else {
        if ((next_duty - DUTY_MIN) < DUTY_STEP) {
            next_duty = DUTY_MIN;
            tracker->direction = 1;
        } else {
            next_duty -= DUTY_STEP;
        }
    }

    tracker->duty_pct = next_duty;
    tracker->panel_power_dmW = panel_power_dmW;
}
