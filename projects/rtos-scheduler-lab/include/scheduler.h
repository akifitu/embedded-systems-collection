#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>

#include "scheduler_types.h"

rsl_phase_result_t rsl_run_phase(const rsl_task_config_t *tasks, size_t task_count,
                                 uint32_t duration_ticks, uint32_t watchdog_limit);

#endif

