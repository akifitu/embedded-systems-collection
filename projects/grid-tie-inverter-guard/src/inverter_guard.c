#include "inverter_guard.h"

#include "derating_model.h"
#include "grid_monitor.h"

#define INVERTER_SYNC_TARGET 5u
#define INVERTER_COOLDOWN_FRAMES 3u

static inverter_output_t make_output(const inverter_guard_t *guard,
                                     inverter_command_t command,
                                     inverter_reason_t reason,
                                     inverter_quality_t quality,
                                     uint8_t power_limit_pct,
                                     bool relay_closed_commanded) {
    inverter_output_t output;

    output.state = guard->state;
    output.command = command;
    output.reason = reason;
    output.quality = quality;
    output.power_limit_pct = power_limit_pct;
    output.sync_count = guard->sync_count;
    output.cooldown_remaining = guard->cooldown_remaining;
    output.relay_closed_commanded = relay_closed_commanded;
    return output;
}

static inverter_output_t enter_trip(inverter_guard_t *guard,
                                    inverter_reason_t reason) {
    guard->state = INVERTER_STATE_TRIP;
    guard->sync_count = 0u;
    guard->cooldown_remaining = INVERTER_COOLDOWN_FRAMES;
    guard->latched_reason = reason;
    return make_output(guard, INVERTER_COMMAND_OPEN_RELAY, reason,
                       INVERTER_QUALITY_FAULT, 0u, false);
}

void inverter_guard_init(inverter_guard_t *guard) {
    guard->state = INVERTER_STATE_WAIT_GRID;
    guard->sync_count = 0u;
    guard->cooldown_remaining = 0u;
    guard->latched_reason = INVERTER_REASON_NONE;
}

inverter_output_t inverter_guard_step(inverter_guard_t *guard,
                                      const inverter_input_t *input) {
    grid_assessment_t assessment;
    uint8_t export_limit_pct;

    assessment = grid_monitor_assess(input);
    export_limit_pct =
        derating_model_limit_pct(input->heatsink_c_x10, input->dc_link_v);

    if ((guard->state == INVERTER_STATE_EXPORT) && !input->relay_closed_fb) {
        return enter_trip(guard, INVERTER_REASON_RELAY_FAULT);
    }

    if (!input->grid_present &&
        ((guard->state == INVERTER_STATE_SYNC) ||
         (guard->state == INVERTER_STATE_EXPORT))) {
        return enter_trip(guard, INVERTER_REASON_GRID_LOSS);
    }

    if (assessment.fault_reason != INVERTER_REASON_NONE) {
        return enter_trip(guard, assessment.fault_reason);
    }

    if (guard->state == INVERTER_STATE_TRIP) {
        guard->state = INVERTER_STATE_COOLDOWN;
        return make_output(guard, INVERTER_COMMAND_HOLD_OFF,
                           INVERTER_REASON_COOLDOWN,
                           INVERTER_QUALITY_RECOVERING, 0u, false);
    }

    if (guard->state == INVERTER_STATE_COOLDOWN) {
        if (guard->cooldown_remaining > 0u) {
            guard->cooldown_remaining--;
        }
        if (guard->cooldown_remaining > 0u) {
            return make_output(guard, INVERTER_COMMAND_HOLD_OFF,
                               INVERTER_REASON_COOLDOWN,
                               INVERTER_QUALITY_RECOVERING, 0u, false);
        }

        guard->state = INVERTER_STATE_WAIT_GRID;
        guard->sync_count = 0u;
        guard->latched_reason = INVERTER_REASON_NONE;
    }

    if (!input->grid_present) {
        guard->state = INVERTER_STATE_WAIT_GRID;
        guard->sync_count = 0u;
        guard->latched_reason = INVERTER_REASON_NONE;
        return make_output(guard, INVERTER_COMMAND_OPEN_RELAY,
                           INVERTER_REASON_NONE, INVERTER_QUALITY_SEARCH, 0u,
                           false);
    }

    switch (guard->state) {
        case INVERTER_STATE_WAIT_GRID:
            if (assessment.stable) {
                guard->state = INVERTER_STATE_SYNC;
                guard->sync_count = 1u;
                return make_output(guard, INVERTER_COMMAND_TRACK_GRID,
                                   INVERTER_REASON_NONE,
                                   INVERTER_QUALITY_LOCKING, 0u, false);
            }
            return make_output(guard, INVERTER_COMMAND_OPEN_RELAY,
                               INVERTER_REASON_NONE, INVERTER_QUALITY_SEARCH,
                               0u, false);

        case INVERTER_STATE_SYNC:
            if (!assessment.stable) {
                guard->state = INVERTER_STATE_WAIT_GRID;
                guard->sync_count = 0u;
                return make_output(guard, INVERTER_COMMAND_OPEN_RELAY,
                                   INVERTER_REASON_NONE,
                                   INVERTER_QUALITY_SEARCH, 0u, false);
            }

            if (guard->sync_count < INVERTER_SYNC_TARGET) {
                guard->sync_count++;
            }

            if (guard->sync_count >= INVERTER_SYNC_TARGET) {
                if (input->relay_closed_fb) {
                    guard->state = INVERTER_STATE_EXPORT;
                    return make_output(guard, INVERTER_COMMAND_EXPORT_POWER,
                                       INVERTER_REASON_NONE,
                                       export_limit_pct < 100u
                                           ? INVERTER_QUALITY_DERATED
                                           : INVERTER_QUALITY_LOCKED,
                                       export_limit_pct, true);
                }
                return make_output(guard, INVERTER_COMMAND_CLOSE_RELAY,
                                   INVERTER_REASON_NONE,
                                   INVERTER_QUALITY_LOCKING, 0u, true);
            }

            return make_output(guard, INVERTER_COMMAND_TRACK_GRID,
                               INVERTER_REASON_NONE, INVERTER_QUALITY_LOCKING,
                               0u, false);

        case INVERTER_STATE_EXPORT:
            return make_output(guard, INVERTER_COMMAND_EXPORT_POWER,
                               INVERTER_REASON_NONE,
                               export_limit_pct < 100u
                                   ? INVERTER_QUALITY_DERATED
                                   : INVERTER_QUALITY_LOCKED,
                               export_limit_pct, true);

        case INVERTER_STATE_TRIP:
        case INVERTER_STATE_COOLDOWN:
        default:
            return make_output(guard, INVERTER_COMMAND_OPEN_RELAY,
                               INVERTER_REASON_NONE, INVERTER_QUALITY_SEARCH,
                               0u, false);
    }
}
