#include "door_monitor.h"

#define DOOR_MOTION_TIMEOUT_FRAMES 3u

door_position_t door_monitor_position(bool door_open_fb, bool door_closed_fb) {
    if (door_open_fb && !door_closed_fb) {
        return DOOR_POSITION_OPEN;
    }
    if (door_closed_fb && !door_open_fb) {
        return DOOR_POSITION_CLOSED;
    }
    return DOOR_POSITION_MOVING;
}

bool door_monitor_timed_out(unsigned motion_frames) {
    return motion_frames >= DOOR_MOTION_TIMEOUT_FRAMES;
}
