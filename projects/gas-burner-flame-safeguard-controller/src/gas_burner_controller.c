#include "gas_burner_controller.h"

#include "ignition_window.h"
#include "interlock_guard.h"

static unsigned progress_for_state(const gas_burner_controller_t *controller) {
    unsigned target;

    target = ignition_window_target(controller->state);
    if (controller->state == BURNER_STATE_RUNNING) {
        return 100u;
    }
    return ignition_window_progress(controller->phase_frames, target);
}

static burner_output_t make_output(const gas_burner_controller_t *controller,
                                   const burner_input_t *input,
                                   burner_command_t command,
                                   burner_fault_t fault) {
    burner_output_t output;

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.fan_active = false;
    output.igniter_active = false;
    output.gas_valve_open = false;
    output.flame_proven =
        (controller->state == BURNER_STATE_RUNNING) && input->flame_present;
    output.progress_pct = progress_for_state(controller);

    switch (controller->state) {
        case BURNER_STATE_PREPURGE:
            output.fan_active = true;
            break;

        case BURNER_STATE_IGNITION:
            output.fan_active = true;
            output.igniter_active = true;
            output.gas_valve_open = true;
            break;

        case BURNER_STATE_RUNNING:
            output.fan_active = true;
            output.gas_valve_open = true;
            break;

        case BURNER_STATE_POSTPURGE:
            output.fan_active = true;
            break;

        case BURNER_STATE_IDLE:
        case BURNER_STATE_LOCKOUT:
        default:
            break;
    }

    return output;
}

static burner_output_t enter_lockout(gas_burner_controller_t *controller,
                                     const burner_input_t *input,
                                     burner_fault_t fault) {
    controller->state = BURNER_STATE_LOCKOUT;
    controller->latched_fault = fault;
    controller->phase_frames = 0u;
    return make_output(controller, input, BURNER_COMMAND_LATCH_LOCKOUT, fault);
}

void gas_burner_controller_init(gas_burner_controller_t *controller) {
    controller->state = BURNER_STATE_IDLE;
    controller->latched_fault = BURNER_FAULT_NONE;
    controller->phase_frames = 0u;
}

burner_output_t gas_burner_controller_step(gas_burner_controller_t *controller,
                                           const burner_input_t *input) {
    burner_fault_t active_fault;
    unsigned target_frames;

    active_fault = interlock_guard_fault(controller->state, input);

    if (controller->state == BURNER_STATE_LOCKOUT) {
        if (input->reset_request && input->limit_closed && !input->flame_present) {
            controller->state = BURNER_STATE_IDLE;
            controller->latched_fault = BURNER_FAULT_NONE;
            controller->phase_frames = 0u;
            return make_output(controller, input, BURNER_COMMAND_RESET_BURNER,
                               BURNER_FAULT_NONE);
        }

        return make_output(controller, input, BURNER_COMMAND_LATCH_LOCKOUT,
                           controller->latched_fault);
    }

    if (active_fault != BURNER_FAULT_NONE) {
        return enter_lockout(controller, input, active_fault);
    }

    switch (controller->state) {
        case BURNER_STATE_IDLE:
            if (input->call_for_heat) {
                controller->state = BURNER_STATE_PREPURGE;
                controller->phase_frames = 1u;
                return make_output(controller, input,
                                   BURNER_COMMAND_START_PREPURGE,
                                   BURNER_FAULT_NONE);
            }
            return make_output(controller, input, BURNER_COMMAND_HOLD_OFF,
                               BURNER_FAULT_NONE);

        case BURNER_STATE_PREPURGE:
            target_frames = ignition_window_target(controller->state);
            if (ignition_window_complete(controller->phase_frames + 1u,
                                         target_frames)) {
                controller->state = BURNER_STATE_IGNITION;
                controller->phase_frames = 1u;
                return make_output(controller, input, BURNER_COMMAND_TRIAL_IGNITE,
                                   BURNER_FAULT_NONE);
            }
            controller->phase_frames++;
            return make_output(controller, input, BURNER_COMMAND_START_PREPURGE,
                               BURNER_FAULT_NONE);

        case BURNER_STATE_IGNITION:
            if (input->flame_present) {
                controller->state = BURNER_STATE_RUNNING;
                controller->phase_frames = 0u;
                return make_output(controller, input, BURNER_COMMAND_HOLD_FLAME,
                                   BURNER_FAULT_NONE);
            }
            target_frames = ignition_window_target(controller->state);
            if (ignition_window_complete(controller->phase_frames + 1u,
                                         target_frames)) {
                return enter_lockout(controller, input, BURNER_FAULT_IGNITION_FAIL);
            }
            controller->phase_frames++;
            return make_output(controller, input, BURNER_COMMAND_TRIAL_IGNITE,
                               BURNER_FAULT_NONE);

        case BURNER_STATE_RUNNING:
            if (!input->call_for_heat) {
                controller->state = BURNER_STATE_POSTPURGE;
                controller->phase_frames = 1u;
                return make_output(controller, input, BURNER_COMMAND_POSTPURGE_FAN,
                                   BURNER_FAULT_NONE);
            }
            return make_output(controller, input, BURNER_COMMAND_HOLD_FLAME,
                               BURNER_FAULT_NONE);

        case BURNER_STATE_POSTPURGE:
            target_frames = ignition_window_target(controller->state);
            if (ignition_window_complete(controller->phase_frames + 1u,
                                         target_frames)) {
                controller->state = BURNER_STATE_IDLE;
                controller->phase_frames = 0u;
                return make_output(controller, input, BURNER_COMMAND_HOLD_OFF,
                                   BURNER_FAULT_NONE);
            }
            controller->phase_frames++;
            return make_output(controller, input, BURNER_COMMAND_POSTPURGE_FAN,
                               BURNER_FAULT_NONE);

        case BURNER_STATE_LOCKOUT:
        default:
            return make_output(controller, input, BURNER_COMMAND_LATCH_LOCKOUT,
                               controller->latched_fault);
    }
}
