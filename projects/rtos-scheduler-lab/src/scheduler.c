#include "scheduler.h"

static int select_ready_task(rsl_task_state_t *states, size_t task_count) {
    int best_index = -1;

    for (size_t i = 0; i < task_count; ++i) {
        if (!states[i].active || states[i].remaining_ticks == 0u) {
            continue;
        }

        if (best_index < 0 ||
            states[i].config.priority < states[(size_t)best_index].config.priority) {
            best_index = (int)i;
        }
    }
    return best_index;
}

static void release_jobs(rsl_task_state_t *states, size_t task_count, uint32_t tick,
                         rsl_phase_result_t *result) {
    for (size_t i = 0; i < task_count; ++i) {
        if (tick != states[i].next_release_tick) {
            continue;
        }

        if (states[i].active && states[i].remaining_ticks > 0u) {
            result->deadline_misses++;
        }

        states[i].active = true;
        states[i].remaining_ticks = states[i].config.wcet_ticks;
        states[i].absolute_deadline_tick = tick + states[i].config.deadline_ticks;
        states[i].next_release_tick += states[i].config.period_ticks;
    }
}

rsl_phase_result_t rsl_run_phase(const rsl_task_config_t *tasks, size_t task_count,
                                 uint32_t duration_ticks, uint32_t watchdog_limit) {
    rsl_task_state_t states[RSL_MAX_TASKS] = {0};
    rsl_phase_result_t result = {0};
    uint32_t ticks_since_watchdog_task = 0u;

    for (size_t i = 0; i < task_count; ++i) {
        states[i].config = tasks[i];
        states[i].next_release_tick = 0u;
    }

    result.watchdog_starved_task = "none";
    result.last_task_name = "idle";

    for (uint32_t tick = 0u; tick < duration_ticks; ++tick) {
        int selected;

        release_jobs(states, task_count, tick, &result);
        selected = select_ready_task(states, task_count);

        if (selected < 0) {
            result.idle_ticks++;
            ticks_since_watchdog_task++;
        } else {
            rsl_task_state_t *task = &states[(size_t)selected];

            result.last_task_name = task->config.name;
            task->remaining_ticks--;
            if (task->remaining_ticks == 0u) {
                task->active = false;
            }

            if (task->config.watchdog_task) {
                ticks_since_watchdog_task = 0u;
            } else {
                ticks_since_watchdog_task++;
            }
        }

        for (size_t i = 0; i < task_count; ++i) {
            if (states[i].active && states[i].remaining_ticks > 0u &&
                (tick + 1u) > states[i].absolute_deadline_tick) {
                result.deadline_misses++;
                states[i].active = false;
                states[i].remaining_ticks = 0u;
            }
        }

        if (!result.watchdog_tripped && ticks_since_watchdog_task > watchdog_limit) {
            result.watchdog_tripped = true;
            result.watchdog_starved_task = "control";
        }
    }

    return result;
}

