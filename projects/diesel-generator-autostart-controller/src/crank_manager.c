#include "crank_manager.h"

#define GEN_MAX_CRANK_FRAMES 2u
#define GEN_MAX_START_ATTEMPTS 2u

uint8_t crank_manager_next_attempt(uint8_t current_attempts) {
    return (uint8_t)(current_attempts + 1u);
}

bool crank_manager_timed_out(uint8_t crank_frames) {
    return crank_frames >= GEN_MAX_CRANK_FRAMES;
}

bool crank_manager_attempts_exhausted(uint8_t attempts) {
    return attempts >= GEN_MAX_START_ATTEMPTS;
}
