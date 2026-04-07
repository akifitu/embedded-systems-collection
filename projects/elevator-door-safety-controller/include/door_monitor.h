#ifndef DOOR_MONITOR_H
#define DOOR_MONITOR_H

#include <stdbool.h>

#include "door_types.h"

door_position_t door_monitor_position(bool door_open_fb, bool door_closed_fb);
bool door_monitor_timed_out(unsigned motion_frames);

#endif
