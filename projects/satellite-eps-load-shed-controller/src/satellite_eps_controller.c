#include "satellite_eps_controller.h"

#include "battery_guard.h"
#include "load_plan.h"

static eps_output_t make_output(const satellite_eps_controller_t *controller,
                                const eps_input_t *input,
                                eps_command_t command,
                                eps_fault_t fault) {
    eps_output_t output;
    load_plan_output_t plan;

    plan = load_plan_apply(controller->state, input);

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.payload_enabled = plan.payload_enabled;
    output.heater_enabled = plan.heater_enabled;
    output.radio_high_rate = plan.radio_high_rate;
    output.adcs_full_rate = plan.adcs_full_rate;
    output.budget_w = plan.budget_w;
    output.in_sunlight = input->in_sunlight;
    output.battery_soc_pct = input->battery_soc_pct;
    output.bus_mv = input->bus_mv;
    return output;
}

static eps_output_t enter_fault(satellite_eps_controller_t *controller,
                                const eps_input_t *input, eps_fault_t fault) {
    controller->state = EPS_STATE_FAULT;
    controller->latched_fault = fault;
    return make_output(controller, input, EPS_COMMAND_LATCH_FAULT, fault);
}

void satellite_eps_controller_init(satellite_eps_controller_t *controller) {
    controller->state = EPS_STATE_NOMINAL;
    controller->latched_fault = EPS_FAULT_NONE;
}

eps_output_t satellite_eps_controller_step(satellite_eps_controller_t *controller,
                                           const eps_input_t *input) {
    eps_fault_t active_fault;

    active_fault = battery_guard_fault(input);

    if (controller->state == EPS_STATE_FAULT) {
        if (input->reset_request && (active_fault == EPS_FAULT_NONE)) {
            controller->latched_fault = EPS_FAULT_NONE;
            controller->state = battery_guard_can_nominal(input)
                                    ? EPS_STATE_NOMINAL
                                    : EPS_STATE_RECOVERY;
            return make_output(controller, input, EPS_COMMAND_RESET_EPS,
                               EPS_FAULT_NONE);
        }

        return make_output(controller, input, EPS_COMMAND_LATCH_FAULT,
                           controller->latched_fault);
    }

    if (active_fault != EPS_FAULT_NONE) {
        return enter_fault(controller, input, active_fault);
    }

    switch (controller->state) {
        case EPS_STATE_NOMINAL:
            if (battery_guard_should_survive(input)) {
                controller->state = EPS_STATE_SURVIVAL;
                return make_output(controller, input,
                                   EPS_COMMAND_ENTER_SURVIVAL, EPS_FAULT_NONE);
            }
            if (battery_guard_should_shed(input)) {
                controller->state = EPS_STATE_SHED;
                return make_output(controller, input,
                                   EPS_COMMAND_SHED_NONCRITICAL,
                                   EPS_FAULT_NONE);
            }
            return make_output(controller, input, EPS_COMMAND_HOLD_FULL_POWER,
                               EPS_FAULT_NONE);

        case EPS_STATE_SHED:
            if (battery_guard_should_survive(input)) {
                controller->state = EPS_STATE_SURVIVAL;
                return make_output(controller, input,
                                   EPS_COMMAND_ENTER_SURVIVAL, EPS_FAULT_NONE);
            }
            if (battery_guard_can_recover(input)) {
                controller->state = EPS_STATE_RECOVERY;
                return make_output(controller, input,
                                   EPS_COMMAND_RESTORE_LOADS, EPS_FAULT_NONE);
            }
            if (!battery_guard_should_shed(input)) {
                controller->state = EPS_STATE_NOMINAL;
                return make_output(controller, input,
                                   EPS_COMMAND_HOLD_FULL_POWER,
                                   EPS_FAULT_NONE);
            }
            return make_output(controller, input,
                               EPS_COMMAND_SHED_NONCRITICAL, EPS_FAULT_NONE);

        case EPS_STATE_SURVIVAL:
            if (battery_guard_can_recover(input)) {
                controller->state = EPS_STATE_RECOVERY;
                return make_output(controller, input,
                                   EPS_COMMAND_RESTORE_LOADS, EPS_FAULT_NONE);
            }
            return make_output(controller, input, EPS_COMMAND_ENTER_SURVIVAL,
                               EPS_FAULT_NONE);

        case EPS_STATE_RECOVERY:
            if (battery_guard_should_survive(input)) {
                controller->state = EPS_STATE_SURVIVAL;
                return make_output(controller, input,
                                   EPS_COMMAND_ENTER_SURVIVAL, EPS_FAULT_NONE);
            }
            if (battery_guard_can_nominal(input)) {
                controller->state = EPS_STATE_NOMINAL;
                return make_output(controller, input,
                                   EPS_COMMAND_HOLD_FULL_POWER,
                                   EPS_FAULT_NONE);
            }
            if (!battery_guard_can_recover(input)) {
                controller->state = EPS_STATE_SHED;
                return make_output(controller, input,
                                   EPS_COMMAND_SHED_NONCRITICAL,
                                   EPS_FAULT_NONE);
            }
            return make_output(controller, input, EPS_COMMAND_RESTORE_LOADS,
                               EPS_FAULT_NONE);

        case EPS_STATE_FAULT:
        default:
            return make_output(controller, input, EPS_COMMAND_LATCH_FAULT,
                               controller->latched_fault);
    }
}
