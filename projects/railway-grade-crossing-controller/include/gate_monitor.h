#ifndef GATE_MONITOR_H
#define GATE_MONITOR_H

#include <stdbool.h>

#include "crossing_types.h"

gate_position_t gate_monitor_position(bool gate_down_fb, bool gate_up_fb);
bool gate_monitor_timed_out(unsigned motion_frames);

#endif
