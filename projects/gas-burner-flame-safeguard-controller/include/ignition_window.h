#ifndef IGNITION_WINDOW_H
#define IGNITION_WINDOW_H

#include "burner_types.h"

bool ignition_window_complete(unsigned frames, unsigned target_frames);
unsigned ignition_window_progress(unsigned frames, unsigned target_frames);
unsigned ignition_window_target(burner_state_t state);

#endif
