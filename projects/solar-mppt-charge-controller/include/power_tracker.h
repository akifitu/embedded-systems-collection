#ifndef POWER_TRACKER_H
#define POWER_TRACKER_H

typedef struct {
    unsigned duty_pct;
    unsigned panel_power_dmW;
    int direction;
} power_tracker_state_t;

void power_tracker_init(power_tracker_state_t *tracker, unsigned duty_pct);
void power_tracker_step(power_tracker_state_t *tracker, unsigned panel_power_dmW);

#endif
