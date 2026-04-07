#ifndef CRANK_MANAGER_H
#define CRANK_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

uint8_t crank_manager_next_attempt(uint8_t current_attempts);
bool crank_manager_timed_out(uint8_t crank_frames);
bool crank_manager_attempts_exhausted(uint8_t attempts);

#endif
