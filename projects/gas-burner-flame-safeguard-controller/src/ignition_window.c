#include "ignition_window.h"

#define PREPURGE_TARGET_FRAMES 2u
#define IGNITION_TARGET_FRAMES 2u
#define POSTPURGE_TARGET_FRAMES 2u

bool ignition_window_complete(unsigned frames, unsigned target_frames) {
    return frames >= target_frames;
}

unsigned ignition_window_progress(unsigned frames, unsigned target_frames) {
    if (target_frames == 0u) {
        return 0u;
    }
    if (frames >= target_frames) {
        return 100u;
    }
    return (frames * 100u) / target_frames;
}

unsigned ignition_window_target(burner_state_t state) {
    switch (state) {
        case BURNER_STATE_PREPURGE:
            return PREPURGE_TARGET_FRAMES;
        case BURNER_STATE_IGNITION:
            return IGNITION_TARGET_FRAMES;
        case BURNER_STATE_POSTPURGE:
            return POSTPURGE_TARGET_FRAMES;
        default:
            return 0u;
    }
}
