#include <stdio.h>

#include "scheduler.h"
#include "task_catalog.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static int test_nominal_has_idle_and_no_miss(void) {
    rsl_task_config_t tasks[RSL_MAX_TASKS];
    size_t count = rsl_phase_nominal(tasks, RSL_MAX_TASKS);
    rsl_phase_result_t result = rsl_run_phase(tasks, count, 60u, 10u);

    ASSERT_TRUE(result.idle_ticks > 0u);
    ASSERT_TRUE(result.deadline_misses == 0u);
    ASSERT_TRUE(!result.watchdog_tripped);
    return 0;
}

static int test_radio_burst_remains_schedulable(void) {
    rsl_task_config_t tasks[RSL_MAX_TASKS];
    size_t count = rsl_phase_radio_burst(tasks, RSL_MAX_TASKS);
    rsl_phase_result_t result = rsl_run_phase(tasks, count, 60u, 10u);

    ASSERT_TRUE(result.deadline_misses == 0u);
    ASSERT_TRUE(!result.watchdog_tripped);
    ASSERT_TRUE(result.last_task_name != 0);
    return 0;
}

static int test_sensor_stall_trips_watchdog_and_misses_deadlines(void) {
    rsl_task_config_t tasks[RSL_MAX_TASKS];
    size_t count = rsl_phase_sensor_stall(tasks, RSL_MAX_TASKS);
    rsl_phase_result_t result = rsl_run_phase(tasks, count, 60u, 6u);

    ASSERT_TRUE(result.deadline_misses > 0u);
    ASSERT_TRUE(result.watchdog_tripped);
    ASSERT_TRUE(result.watchdog_starved_task[0] == 'c');
    return 0;
}

static int test_recovery_returns_to_nominal_behavior(void) {
    rsl_task_config_t tasks[RSL_MAX_TASKS];
    size_t count = rsl_phase_recovery(tasks, RSL_MAX_TASKS);
    rsl_phase_result_t result = rsl_run_phase(tasks, count, 60u, 10u);

    ASSERT_TRUE(result.deadline_misses == 0u);
    ASSERT_TRUE(!result.watchdog_tripped);
    ASSERT_TRUE(result.idle_ticks > 0u);
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_nominal_has_idle_and_no_miss() == 0);
    ASSERT_TRUE(test_radio_burst_remains_schedulable() == 0);
    ASSERT_TRUE(test_sensor_stall_trips_watchdog_and_misses_deadlines() == 0);
    ASSERT_TRUE(test_recovery_returns_to_nominal_behavior() == 0);
    printf("rtos-scheduler-lab tests passed\n");
    return 0;
}
