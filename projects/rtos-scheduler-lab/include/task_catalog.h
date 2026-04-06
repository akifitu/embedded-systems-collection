#ifndef TASK_CATALOG_H
#define TASK_CATALOG_H

#include <stddef.h>

#include "scheduler_types.h"

size_t rsl_phase_nominal(rsl_task_config_t *tasks_out, size_t max_tasks);
size_t rsl_phase_radio_burst(rsl_task_config_t *tasks_out, size_t max_tasks);
size_t rsl_phase_sensor_stall(rsl_task_config_t *tasks_out, size_t max_tasks);
size_t rsl_phase_recovery(rsl_task_config_t *tasks_out, size_t max_tasks);

#endif

