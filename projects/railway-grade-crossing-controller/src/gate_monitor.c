#include "gate_monitor.h"

#define GATE_MOTION_TIMEOUT_FRAMES 3u

gate_position_t gate_monitor_position(bool gate_down_fb, bool gate_up_fb) {
    if (gate_down_fb && !gate_up_fb) {
        return GATE_POSITION_DOWN;
    }
    if (gate_up_fb && !gate_down_fb) {
        return GATE_POSITION_UP;
    }
    return GATE_POSITION_MOVING;
}

bool gate_monitor_timed_out(unsigned motion_frames) {
    return motion_frames >= GATE_MOTION_TIMEOUT_FRAMES;
}
