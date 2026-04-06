#include <stdio.h>

#include "scheduler.h"
#include "task_catalog.h"

static void print_phase(const char *label, const rsl_phase_result_t *result) {
    printf("phase=%s idle=%u misses=%u watchdog=%s starved=%s last=%s\n", label,
           result->idle_ticks, result->deadline_misses,
           result->watchdog_tripped ? "TRIPPED" : "OK",
           result->watchdog_starved_task, result->last_task_name);
}

int main(void) {
    rsl_task_config_t tasks[RSL_MAX_TASKS];
    size_t count;
    rsl_phase_result_t result;

    count = rsl_phase_nominal(tasks, RSL_MAX_TASKS);
    result = rsl_run_phase(tasks, count, 60u, 10u);
    print_phase("nominal", &result);

    count = rsl_phase_radio_burst(tasks, RSL_MAX_TASKS);
    result = rsl_run_phase(tasks, count, 60u, 10u);
    print_phase("radio_burst", &result);

    count = rsl_phase_sensor_stall(tasks, RSL_MAX_TASKS);
    result = rsl_run_phase(tasks, count, 60u, 6u);
    print_phase("sensor_stall", &result);

    count = rsl_phase_recovery(tasks, RSL_MAX_TASKS);
    result = rsl_run_phase(tasks, count, 60u, 10u);
    print_phase("recovery", &result);

    return 0;
}

