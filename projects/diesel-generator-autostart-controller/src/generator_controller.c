#include "generator_controller.h"

#include "crank_manager.h"
#include "engine_guard.h"

#define GEN_WARMUP_TARGET 2u
#define GEN_COOLDOWN_TARGET 2u

static gen_output_t make_output(const generator_controller_t *controller,
                                gen_command_t command, gen_fault_t fault,
                                bool contactor_closed) {
    gen_output_t output;

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.start_attempts = controller->start_attempts;
    output.contactor_closed = contactor_closed;
    return output;
}

static gen_output_t enter_fault(generator_controller_t *controller,
                                gen_fault_t fault) {
    controller->state = GEN_STATE_FAULT;
    controller->latched_fault = fault;
    controller->crank_frames = 0u;
    controller->warmup_frames = 0u;
    controller->cooldown_frames = 0u;
    return make_output(controller, GEN_COMMAND_STOP_ENGINE, fault, false);
}

void generator_controller_init(generator_controller_t *controller) {
    controller->state = GEN_STATE_IDLE;
    controller->latched_fault = GEN_FAULT_NONE;
    controller->start_attempts = 0u;
    controller->crank_frames = 0u;
    controller->warmup_frames = 0u;
    controller->cooldown_frames = 0u;
}

gen_output_t generator_controller_step(generator_controller_t *controller,
                                       const gen_input_t *input) {
    if (input->emergency_stop) {
        return enter_fault(controller, GEN_FAULT_E_STOP);
    }

    if (controller->state == GEN_STATE_FAULT) {
        if (input->reset_request && input->utility_present &&
            (input->engine_rpm == 0u) && !input->emergency_stop) {
            controller->state = GEN_STATE_IDLE;
            controller->latched_fault = GEN_FAULT_NONE;
            controller->start_attempts = 0u;
            return make_output(controller, GEN_COMMAND_OPEN_CONTACTOR,
                               GEN_FAULT_NONE, false);
        }
        return make_output(controller, GEN_COMMAND_STOP_ENGINE,
                           controller->latched_fault, false);
    }

    if ((controller->state == GEN_STATE_WARMUP ||
         controller->state == GEN_STATE_RUNNING) &&
        !engine_guard_oil_ok(input)) {
        return enter_fault(controller, GEN_FAULT_LOW_OIL);
    }

    if (input->utility_present) {
        if (controller->state == GEN_STATE_RUNNING ||
            controller->state == GEN_STATE_WARMUP ||
            controller->state == GEN_STATE_CRANKING ||
            controller->state == GEN_STATE_START_DELAY) {
            controller->state = GEN_STATE_COOLDOWN;
            controller->cooldown_frames = 1u;
            return make_output(controller, GEN_COMMAND_COOL_ENGINE,
                               GEN_FAULT_NONE, false);
        }

        if (controller->state == GEN_STATE_COOLDOWN) {
            if ((input->engine_rpm == 0u) ||
                (controller->cooldown_frames >= GEN_COOLDOWN_TARGET)) {
                controller->state = GEN_STATE_IDLE;
                controller->start_attempts = 0u;
                controller->cooldown_frames = 0u;
                return make_output(controller, GEN_COMMAND_OPEN_CONTACTOR,
                                   GEN_FAULT_NONE, false);
            }
            controller->cooldown_frames++;
            return make_output(controller, GEN_COMMAND_COOL_ENGINE,
                               GEN_FAULT_NONE, false);
        }

        controller->state = GEN_STATE_IDLE;
        controller->latched_fault = GEN_FAULT_NONE;
        controller->start_attempts = 0u;
        return make_output(controller, GEN_COMMAND_OPEN_CONTACTOR, GEN_FAULT_NONE,
                           false);
    }

    switch (controller->state) {
        case GEN_STATE_IDLE:
            if (!engine_guard_battery_ok(input)) {
                return enter_fault(controller, GEN_FAULT_LOW_BATTERY);
            }
            controller->state = GEN_STATE_START_DELAY;
            return make_output(controller, GEN_COMMAND_PREHEAT, GEN_FAULT_NONE,
                               false);

        case GEN_STATE_START_DELAY:
            if (!engine_guard_battery_ok(input)) {
                return enter_fault(controller, GEN_FAULT_LOW_BATTERY);
            }
            controller->state = GEN_STATE_CRANKING;
            controller->start_attempts =
                crank_manager_next_attempt(controller->start_attempts);
            controller->crank_frames = 1u;
            return make_output(controller, GEN_COMMAND_CRANK_START,
                               GEN_FAULT_NONE, false);

        case GEN_STATE_CRANKING:
            if (engine_guard_started(input)) {
                controller->state = GEN_STATE_WARMUP;
                controller->warmup_frames = 1u;
                controller->crank_frames = 0u;
                return make_output(controller, GEN_COMMAND_OPEN_CONTACTOR,
                                   GEN_FAULT_NONE, false);
            }
            if (!engine_guard_battery_ok(input)) {
                return enter_fault(controller, GEN_FAULT_LOW_BATTERY);
            }
            if (crank_manager_timed_out(controller->crank_frames)) {
                controller->crank_frames = 0u;
                if (crank_manager_attempts_exhausted(controller->start_attempts)) {
                    return enter_fault(controller, GEN_FAULT_START_FAIL);
                }
                controller->state = GEN_STATE_START_DELAY;
                return make_output(controller, GEN_COMMAND_PREHEAT,
                                   GEN_FAULT_NONE, false);
            }
            controller->crank_frames++;
            return make_output(controller, GEN_COMMAND_CRANK_START,
                               GEN_FAULT_NONE, false);

        case GEN_STATE_WARMUP:
            if (!engine_guard_started(input)) {
                return enter_fault(controller, GEN_FAULT_START_FAIL);
            }
            if ((uint8_t)(controller->warmup_frames + 1u) >= GEN_WARMUP_TARGET) {
                controller->state = GEN_STATE_RUNNING;
                return make_output(controller, GEN_COMMAND_CLOSE_CONTACTOR,
                                   GEN_FAULT_NONE, true);
            }
            controller->warmup_frames++;
            return make_output(controller, GEN_COMMAND_OPEN_CONTACTOR,
                               GEN_FAULT_NONE, false);

        case GEN_STATE_RUNNING:
            return make_output(controller, GEN_COMMAND_CLOSE_CONTACTOR,
                               GEN_FAULT_NONE, true);

        case GEN_STATE_COOLDOWN:
            controller->cooldown_frames = 1u;
            return make_output(controller, GEN_COMMAND_COOL_ENGINE,
                               GEN_FAULT_NONE, false);

        case GEN_STATE_FAULT:
        default:
            return make_output(controller, GEN_COMMAND_STOP_ENGINE,
                               controller->latched_fault, false);
    }
}
