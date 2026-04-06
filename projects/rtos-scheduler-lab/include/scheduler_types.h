#ifndef SCHEDULER_TYPES_H
#define SCHEDULER_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RSL_MAX_TASKS 6u

typedef struct {
    const char *name;
    uint16_t period_ticks;
    uint16_t deadline_ticks;
    uint16_t wcet_ticks;
    uint8_t priority;
    bool watchdog_task;
} rsl_task_config_t;

typedef struct {
    rsl_task_config_t config;
    uint16_t remaining_ticks;
    uint32_t next_release_tick;
    uint32_t absolute_deadline_tick;
    bool active;
} rsl_task_state_t;

typedef struct {
    uint32_t idle_ticks;
    uint32_t deadline_misses;
    bool watchdog_tripped;
    const char *watchdog_starved_task;
    const char *last_task_name;
} rsl_phase_result_t;

#endif

