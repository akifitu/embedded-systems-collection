#include "wind_turbine_controller.h"

#include "pitch_plan.h"
#include "wind_guard.h"

static turbine_output_t make_output(const wind_turbine_controller_t *controller,
                                    const turbine_input_t *input,
                                    turbine_command_t command,
                                    turbine_fault_t fault) {
    turbine_output_t output;
    pitch_plan_output_t plan;

    plan = pitch_plan_apply(controller->state, input);

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.pitch_deg = plan.pitch_deg;
    output.generator_enabled = plan.generator_enabled;
    output.brake_applied = plan.brake_applied;
    output.wind_dmps = input->wind_dmps;
    output.rotor_rpm = input->rotor_rpm;
    return output;
}

static turbine_output_t enter_fault(wind_turbine_controller_t *controller,
                                    const turbine_input_t *input,
                                    turbine_fault_t fault) {
    controller->state = TURBINE_STATE_FAULT;
    controller->latched_fault = fault;
    return make_output(controller, input, TURBINE_COMMAND_LATCH_FAULT, fault);
}

void wind_turbine_controller_init(wind_turbine_controller_t *controller) {
    controller->state = TURBINE_STATE_PARKED;
    controller->latched_fault = TURBINE_FAULT_NONE;
}

turbine_output_t wind_turbine_controller_step(
    wind_turbine_controller_t *controller, const turbine_input_t *input) {
    turbine_fault_t active_fault;

    active_fault = wind_guard_fault(input);

    if (controller->state == TURBINE_STATE_FAULT) {
        if (input->reset_request && (active_fault == TURBINE_FAULT_NONE) &&
            wind_guard_can_park(input)) {
            controller->state = TURBINE_STATE_PARKED;
            controller->latched_fault = TURBINE_FAULT_NONE;
            return make_output(controller, input,
                               TURBINE_COMMAND_RESET_TURBINE,
                               TURBINE_FAULT_NONE);
        }

        return make_output(controller, input, TURBINE_COMMAND_LATCH_FAULT,
                           controller->latched_fault);
    }

    if (active_fault != TURBINE_FAULT_NONE) {
        return enter_fault(controller, input, active_fault);
    }

    switch (controller->state) {
        case TURBINE_STATE_PARKED:
            if (wind_guard_should_storm_hold(input)) {
                controller->state = TURBINE_STATE_STORM_HOLD;
                return make_output(controller, input,
                                   TURBINE_COMMAND_HOLD_STORM,
                                   TURBINE_FAULT_NONE);
            }
            if (wind_guard_can_start(input)) {
                controller->state = TURBINE_STATE_STARTUP;
                return make_output(controller, input,
                                   TURBINE_COMMAND_STARTUP_RELEASE,
                                   TURBINE_FAULT_NONE);
            }
            return make_output(controller, input, TURBINE_COMMAND_HOLD_PARK,
                               TURBINE_FAULT_NONE);

        case TURBINE_STATE_STARTUP:
            if (wind_guard_should_storm_hold(input)) {
                controller->state = TURBINE_STATE_STORM_HOLD;
                return make_output(controller, input,
                                   TURBINE_COMMAND_HOLD_STORM,
                                   TURBINE_FAULT_NONE);
            }
            if (wind_guard_should_feather(input)) {
                controller->state = TURBINE_STATE_FEATHERING;
                return make_output(controller, input,
                                   TURBINE_COMMAND_FEATHER_BLADES,
                                   TURBINE_FAULT_NONE);
            }
            if (input->rotor_rpm >= 900u) {
                controller->state = TURBINE_STATE_GENERATING;
                return make_output(controller, input, TURBINE_COMMAND_TRACK_POWER,
                                   TURBINE_FAULT_NONE);
            }
            return make_output(controller, input,
                               TURBINE_COMMAND_STARTUP_RELEASE,
                               TURBINE_FAULT_NONE);

        case TURBINE_STATE_GENERATING:
            if (wind_guard_should_storm_hold(input)) {
                controller->state = TURBINE_STATE_STORM_HOLD;
                return make_output(controller, input,
                                   TURBINE_COMMAND_HOLD_STORM,
                                   TURBINE_FAULT_NONE);
            }
            if (wind_guard_should_feather(input)) {
                controller->state = TURBINE_STATE_FEATHERING;
                return make_output(controller, input,
                                   TURBINE_COMMAND_FEATHER_BLADES,
                                   TURBINE_FAULT_NONE);
            }
            return make_output(controller, input, TURBINE_COMMAND_TRACK_POWER,
                               TURBINE_FAULT_NONE);

        case TURBINE_STATE_FEATHERING:
            if (wind_guard_should_storm_hold(input) &&
                wind_guard_rotor_slow(input)) {
                controller->state = TURBINE_STATE_STORM_HOLD;
                return make_output(controller, input,
                                   TURBINE_COMMAND_HOLD_STORM,
                                   TURBINE_FAULT_NONE);
            }
            if (wind_guard_can_park(input)) {
                controller->state = TURBINE_STATE_PARKED;
                return make_output(controller, input, TURBINE_COMMAND_HOLD_PARK,
                                   TURBINE_FAULT_NONE);
            }
            return make_output(controller, input,
                               TURBINE_COMMAND_FEATHER_BLADES,
                               TURBINE_FAULT_NONE);

        case TURBINE_STATE_STORM_HOLD:
            if (!wind_guard_should_storm_hold(input) && wind_guard_can_park(input)) {
                controller->state = TURBINE_STATE_PARKED;
                return make_output(controller, input, TURBINE_COMMAND_HOLD_PARK,
                                   TURBINE_FAULT_NONE);
            }
            return make_output(controller, input, TURBINE_COMMAND_HOLD_STORM,
                               TURBINE_FAULT_NONE);

        case TURBINE_STATE_FAULT:
        default:
            return make_output(controller, input, TURBINE_COMMAND_LATCH_FAULT,
                               controller->latched_fault);
    }
}
