#include "solar_mppt_controller.h"

#include "charge_policy.h"

#define SOLAR_BULK_START_POWER_DMW 100u
#define SOLAR_ABSORB_ENTRY_MV 14350u
#define SOLAR_FLOAT_ENTRY_MV 13700u
#define SOLAR_REBULK_MV 13100u
#define SOLAR_OVERVOLT_MV 14900u
#define SOLAR_OVERTEMP_C 80u
#define SOLAR_ABSORB_HOLD_FRAMES 1u

static unsigned panel_power_dmW(const solar_input_t *input) {
    return (input->panel_mv * input->panel_ma) / 100000u;
}

static solar_fault_t active_fault(const solar_input_t *input) {
    if (input->reverse_polarity) {
        return SOLAR_FAULT_PANEL_REVERSE;
    }
    if (input->battery_mv >= SOLAR_OVERVOLT_MV) {
        return SOLAR_FAULT_BATTERY_OVERVOLT;
    }
    if (input->charger_temp_c >= SOLAR_OVERTEMP_C) {
        return SOLAR_FAULT_OVER_TEMP;
    }
    return SOLAR_FAULT_NONE;
}

static solar_output_t make_output(const solar_mppt_controller_t *controller,
                                  const solar_input_t *input,
                                  solar_command_t command,
                                  solar_fault_t fault) {
    solar_output_t output;
    charge_policy_output_t policy;

    policy = charge_policy_apply(controller->state, input);

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.duty_pct =
        policy.relay_closed ? controller->tracker.duty_pct : 0u;
    output.relay_closed = policy.relay_closed;
    output.fan_active = policy.fan_active;
    output.stage_target_mv = policy.stage_target_mv;
    output.panel_mv = input->panel_mv;
    output.panel_ma = input->panel_ma;
    output.panel_power_dmW = panel_power_dmW(input);
    output.battery_mv = input->battery_mv;
    output.charger_temp_c = input->charger_temp_c;
    return output;
}

static solar_output_t enter_fault(solar_mppt_controller_t *controller,
                                  const solar_input_t *input,
                                  solar_fault_t fault) {
    controller->state = SOLAR_STATE_FAULT;
    controller->latched_fault = fault;
    controller->absorb_frames = 0u;
    power_tracker_init(&controller->tracker, 50u);
    return make_output(controller, input, SOLAR_COMMAND_LATCH_FAULT, fault);
}

void solar_mppt_controller_init(solar_mppt_controller_t *controller) {
    controller->state = SOLAR_STATE_IDLE;
    controller->latched_fault = SOLAR_FAULT_NONE;
    controller->absorb_frames = 0u;
    power_tracker_init(&controller->tracker, 50u);
}

solar_output_t solar_mppt_controller_step(solar_mppt_controller_t *controller,
                                          const solar_input_t *input) {
    solar_fault_t fault;
    unsigned power_dmw;

    fault = active_fault(input);
    power_dmw = panel_power_dmW(input);

    if (controller->state == SOLAR_STATE_FAULT) {
        if (input->reset_request && (fault == SOLAR_FAULT_NONE)) {
            controller->state = SOLAR_STATE_IDLE;
            controller->latched_fault = SOLAR_FAULT_NONE;
            controller->absorb_frames = 0u;
            power_tracker_init(&controller->tracker, 50u);
            return make_output(controller, input,
                               SOLAR_COMMAND_RESET_CHARGER,
                               SOLAR_FAULT_NONE);
        }

        return make_output(controller, input, SOLAR_COMMAND_LATCH_FAULT,
                           controller->latched_fault);
    }

    if (fault != SOLAR_FAULT_NONE) {
        return enter_fault(controller, input, fault);
    }

    if (!input->charge_enable || (power_dmw < SOLAR_BULK_START_POWER_DMW)) {
        controller->state = SOLAR_STATE_IDLE;
        controller->absorb_frames = 0u;
        power_tracker_init(&controller->tracker, 50u);
        return make_output(controller, input, SOLAR_COMMAND_OPEN_RELAY,
                           SOLAR_FAULT_NONE);
    }

    switch (controller->state) {
        case SOLAR_STATE_IDLE:
            controller->state = SOLAR_STATE_BULK;
            power_tracker_init(&controller->tracker, 50u);
            power_tracker_step(&controller->tracker, power_dmw);
            return make_output(controller, input, SOLAR_COMMAND_SEEK_MPP,
                               SOLAR_FAULT_NONE);

        case SOLAR_STATE_BULK:
            if (input->battery_mv >= SOLAR_ABSORB_ENTRY_MV) {
                controller->state = SOLAR_STATE_ABSORB;
                controller->absorb_frames = 1u;
                controller->tracker.duty_pct = 48u;
                controller->tracker.panel_power_dmW = power_dmw;
                return make_output(controller, input, SOLAR_COMMAND_HOLD_ABSORB,
                                   SOLAR_FAULT_NONE);
            }
            power_tracker_step(&controller->tracker, power_dmw);
            return make_output(controller, input, SOLAR_COMMAND_SEEK_MPP,
                               SOLAR_FAULT_NONE);

        case SOLAR_STATE_ABSORB:
            if ((controller->absorb_frames >= SOLAR_ABSORB_HOLD_FRAMES) &&
                (input->battery_mv >= SOLAR_FLOAT_ENTRY_MV)) {
                controller->state = SOLAR_STATE_FLOAT;
                controller->tracker.duty_pct = 28u;
                controller->tracker.panel_power_dmW = power_dmw;
                return make_output(controller, input, SOLAR_COMMAND_HOLD_FLOAT,
                                   SOLAR_FAULT_NONE);
            }
            controller->absorb_frames++;
            return make_output(controller, input, SOLAR_COMMAND_HOLD_ABSORB,
                               SOLAR_FAULT_NONE);

        case SOLAR_STATE_FLOAT:
            if (input->battery_mv <= SOLAR_REBULK_MV) {
                controller->state = SOLAR_STATE_BULK;
                power_tracker_init(&controller->tracker, 50u);
                power_tracker_step(&controller->tracker, power_dmw);
                return make_output(controller, input, SOLAR_COMMAND_SEEK_MPP,
                                   SOLAR_FAULT_NONE);
            }
            return make_output(controller, input, SOLAR_COMMAND_HOLD_FLOAT,
                               SOLAR_FAULT_NONE);

        case SOLAR_STATE_FAULT:
        default:
            return make_output(controller, input, SOLAR_COMMAND_LATCH_FAULT,
                               controller->latched_fault);
    }
}
