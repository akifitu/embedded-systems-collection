#include "task_catalog.h"

static size_t fill_phase(const rsl_task_config_t *source, size_t count,
                         rsl_task_config_t *tasks_out, size_t max_tasks) {
    size_t copy_count = (count < max_tasks) ? count : max_tasks;

    for (size_t i = 0; i < copy_count; ++i) {
        tasks_out[i] = source[i];
    }
    return copy_count;
}

size_t rsl_phase_nominal(rsl_task_config_t *tasks_out, size_t max_tasks) {
    static const rsl_task_config_t tasks[] = {
        {"control", 5u, 5u, 1u, 0u, true},
        {"sensor", 10u, 10u, 2u, 1u, false},
        {"telemetry", 20u, 20u, 3u, 2u, false},
        {"logger", 30u, 30u, 1u, 3u, false},
    };
    return fill_phase(tasks, sizeof(tasks) / sizeof(tasks[0]), tasks_out, max_tasks);
}

size_t rsl_phase_radio_burst(rsl_task_config_t *tasks_out, size_t max_tasks) {
    static const rsl_task_config_t tasks[] = {
        {"control", 5u, 5u, 1u, 0u, true},
        {"sensor", 10u, 10u, 2u, 1u, false},
        {"telemetry", 20u, 20u, 8u, 2u, false},
        {"logger", 30u, 30u, 1u, 3u, false},
    };
    return fill_phase(tasks, sizeof(tasks) / sizeof(tasks[0]), tasks_out, max_tasks);
}

size_t rsl_phase_sensor_stall(rsl_task_config_t *tasks_out, size_t max_tasks) {
    static const rsl_task_config_t tasks[] = {
        {"control", 5u, 5u, 1u, 1u, true},
        {"sensor", 4u, 4u, 4u, 0u, false},
        {"telemetry", 20u, 20u, 6u, 2u, false},
        {"logger", 30u, 30u, 2u, 3u, false},
    };
    return fill_phase(tasks, sizeof(tasks) / sizeof(tasks[0]), tasks_out, max_tasks);
}

size_t rsl_phase_recovery(rsl_task_config_t *tasks_out, size_t max_tasks) {
    static const rsl_task_config_t tasks[] = {
        {"control", 5u, 5u, 1u, 0u, true},
        {"sensor", 10u, 10u, 2u, 1u, false},
        {"telemetry", 20u, 20u, 4u, 2u, false},
        {"logger", 30u, 30u, 1u, 3u, false},
    };
    return fill_phase(tasks, sizeof(tasks) / sizeof(tasks[0]), tasks_out, max_tasks);
}
