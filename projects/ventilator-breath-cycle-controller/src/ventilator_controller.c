#include "ventilator_controller.h"

#include "breath_profile.h"
#include "patient_guard.h"

#define VENT_INHALE_TARGET_PRESSURE 18u
#define VENT_BACKUP_TARGET_PRESSURE 20u
#define VENT_HOLD_TARGET_FRAMES 1u

static vent_output_t make_output(const ventilator_controller_t *controller,
                                 const vent_input_t *input,
                                 vent_command_t command, vent_fault_t fault) {
    vent_output_t output;
    breath_profile_output_t profile;

    profile = breath_profile_apply(controller->state);

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.backup_active = profile.backup_active;
    output.blower_pct = profile.blower_pct;
    output.inspiratory_valve_open = profile.inspiratory_valve_open;
    output.expiratory_valve_open = profile.expiratory_valve_open;
    output.target_pressure_cmh2o = profile.target_pressure_cmh2o;
    output.airway_pressure_cmh2o = input->airway_pressure_cmh2o;
    output.flow_lpm = input->flow_lpm;
    return output;
}

static vent_output_t enter_alarm(ventilator_controller_t *controller,
                                 const vent_input_t *input,
                                 vent_fault_t fault) {
    controller->state = VENT_STATE_ALARM;
    controller->latched_fault = fault;
    controller->phase_frames = 0u;
    controller->exhale_frames = 0u;
    return make_output(controller, input, VENT_COMMAND_LATCH_ALARM, fault);
}

void ventilator_controller_init(ventilator_controller_t *controller) {
    controller->state = VENT_STATE_STANDBY;
    controller->latched_fault = VENT_FAULT_NONE;
    controller->phase_frames = 0u;
    controller->exhale_frames = 0u;
}

vent_output_t ventilator_controller_step(ventilator_controller_t *controller,
                                         const vent_input_t *input) {
    vent_fault_t active_fault;

    active_fault = patient_guard_fault(controller->state, input);

    if (controller->state == VENT_STATE_ALARM) {
        if (patient_guard_can_reset(input)) {
            controller->state = VENT_STATE_STANDBY;
            controller->latched_fault = VENT_FAULT_NONE;
            controller->phase_frames = 0u;
            controller->exhale_frames = 0u;
            return make_output(controller, input, VENT_COMMAND_RESET_VENT,
                               VENT_FAULT_NONE);
        }

        return make_output(controller, input, VENT_COMMAND_LATCH_ALARM,
                           controller->latched_fault);
    }

    if (active_fault != VENT_FAULT_NONE) {
        return enter_alarm(controller, input, active_fault);
    }

    if (!input->therapy_enabled) {
        controller->state = VENT_STATE_STANDBY;
        controller->phase_frames = 0u;
        controller->exhale_frames = 0u;
        return make_output(controller, input, VENT_COMMAND_HOLD_STANDBY,
                           VENT_FAULT_NONE);
    }

    switch (controller->state) {
        case VENT_STATE_STANDBY:
            if (input->patient_trigger) {
                controller->state = VENT_STATE_INHALE;
                controller->phase_frames = 1u;
                controller->exhale_frames = 0u;
                return make_output(controller, input,
                                   VENT_COMMAND_DELIVER_INHALE,
                                   VENT_FAULT_NONE);
            }
            return make_output(controller, input, VENT_COMMAND_HOLD_STANDBY,
                               VENT_FAULT_NONE);

        case VENT_STATE_INHALE:
            if (input->airway_pressure_cmh2o >= VENT_INHALE_TARGET_PRESSURE) {
                controller->state = VENT_STATE_HOLD;
                controller->phase_frames = 1u;
                return make_output(controller, input, VENT_COMMAND_HOLD_PLATEAU,
                                   VENT_FAULT_NONE);
            }
            controller->phase_frames++;
            return make_output(controller, input, VENT_COMMAND_DELIVER_INHALE,
                               VENT_FAULT_NONE);

        case VENT_STATE_BACKUP:
            if (input->airway_pressure_cmh2o >= VENT_BACKUP_TARGET_PRESSURE) {
                controller->state = VENT_STATE_HOLD;
                controller->phase_frames = 1u;
                return make_output(controller, input, VENT_COMMAND_HOLD_PLATEAU,
                                   VENT_FAULT_NONE);
            }
            controller->phase_frames++;
            return make_output(controller, input, VENT_COMMAND_START_BACKUP,
                               VENT_FAULT_NONE);

        case VENT_STATE_HOLD:
            if (controller->phase_frames >= VENT_HOLD_TARGET_FRAMES) {
                controller->state = VENT_STATE_EXHALE;
                controller->exhale_frames = 1u;
                controller->phase_frames = 0u;
                return make_output(controller, input, VENT_COMMAND_VENT_EXHALE,
                                   VENT_FAULT_NONE);
            }
            controller->phase_frames++;
            return make_output(controller, input, VENT_COMMAND_HOLD_PLATEAU,
                               VENT_FAULT_NONE);

        case VENT_STATE_EXHALE:
            if (input->patient_trigger) {
                controller->state = VENT_STATE_INHALE;
                controller->phase_frames = 1u;
                controller->exhale_frames = 0u;
                return make_output(controller, input,
                                   VENT_COMMAND_DELIVER_INHALE,
                                   VENT_FAULT_NONE);
            }
            if (patient_guard_apnea_due(controller->exhale_frames,
                                        input->therapy_enabled)) {
                controller->state = VENT_STATE_BACKUP;
                controller->phase_frames = 1u;
                controller->exhale_frames = 0u;
                return make_output(controller, input, VENT_COMMAND_START_BACKUP,
                                   VENT_FAULT_NONE);
            }
            controller->exhale_frames++;
            return make_output(controller, input, VENT_COMMAND_VENT_EXHALE,
                               VENT_FAULT_NONE);

        case VENT_STATE_ALARM:
        default:
            return make_output(controller, input, VENT_COMMAND_LATCH_ALARM,
                               controller->latched_fault);
    }
}
