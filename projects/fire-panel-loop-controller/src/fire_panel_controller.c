#include "fire_panel_controller.h"

#include "nac_policy.h"
#include "zone_evaluator.h"

#define SMOKE_VERIFY_TARGET 2u

static unsigned verify_percent(const fire_panel_controller_t *controller,
                               panel_alarm_t alarm) {
    if (controller->state == PANEL_STATE_VERIFY) {
        return (controller->smoke_verify_ticks * 100u) / SMOKE_VERIFY_TARGET;
    }
    if (((controller->state == PANEL_STATE_ALARM) ||
         (controller->state == PANEL_STATE_SILENCED)) &&
        (alarm == PANEL_ALARM_SMOKE)) {
        return 100u;
    }
    return 0u;
}

static fire_panel_output_t make_output(const fire_panel_controller_t *controller,
                                       panel_command_t command,
                                       panel_alarm_t alarm,
                                       panel_trouble_t trouble) {
    fire_panel_output_t output;
    nac_policy_output_t policy;

    policy = nac_policy_apply(controller->state, trouble);

    output.state = controller->state;
    output.command = command;
    output.alarm = alarm;
    output.trouble = trouble;
    output.nac_active = policy.nac_active;
    output.buzzer_active = policy.buzzer_active;
    output.alarm_led = policy.alarm_led;
    output.trouble_led = policy.trouble_led;
    output.verify_percent = verify_percent(controller, alarm);
    return output;
}

static panel_trouble_t visible_trouble(const fire_panel_controller_t *controller,
                                       panel_trouble_t active_trouble) {
    if (active_trouble != PANEL_TROUBLE_NONE) {
        return active_trouble;
    }
    return controller->latched_trouble;
}

static fire_panel_output_t enter_alarm(fire_panel_controller_t *controller,
                                       panel_alarm_t alarm,
                                       panel_trouble_t active_trouble) {
    controller->state = PANEL_STATE_ALARM;
    controller->latched_alarm = alarm;
    if (active_trouble != PANEL_TROUBLE_NONE) {
        controller->latched_trouble = active_trouble;
    }
    controller->smoke_verify_ticks = 0u;
    return make_output(controller, PANEL_COMMAND_ACTIVATE_NACS, alarm,
                       visible_trouble(controller, active_trouble));
}

static fire_panel_output_t enter_trouble(fire_panel_controller_t *controller,
                                         panel_trouble_t trouble) {
    controller->state = PANEL_STATE_TROUBLE;
    controller->latched_trouble = trouble;
    controller->smoke_verify_ticks = 0u;
    return make_output(controller, PANEL_COMMAND_LATCH_TROUBLE, PANEL_ALARM_NONE,
                       controller->latched_trouble);
}

void fire_panel_controller_init(fire_panel_controller_t *controller) {
    controller->state = PANEL_STATE_IDLE;
    controller->latched_alarm = PANEL_ALARM_NONE;
    controller->latched_trouble = PANEL_TROUBLE_NONE;
    controller->smoke_verify_ticks = 0u;
}

fire_panel_output_t fire_panel_controller_step(fire_panel_controller_t *controller,
                                               const fire_panel_input_t *input) {
    zone_evaluation_t evaluation;

    evaluation = zone_evaluator_eval(input);

    if ((controller->state == PANEL_STATE_ALARM) ||
        (controller->state == PANEL_STATE_SILENCED)) {
        if (evaluation.alarm == PANEL_ALARM_MANUAL_PULL) {
            controller->latched_alarm = PANEL_ALARM_MANUAL_PULL;
        }
        if (evaluation.trouble != PANEL_TROUBLE_NONE) {
            controller->latched_trouble = evaluation.trouble;
        }

        if (input->reset_request && (evaluation.alarm == PANEL_ALARM_NONE)) {
            controller->latched_alarm = PANEL_ALARM_NONE;
            controller->smoke_verify_ticks = 0u;

            if (evaluation.trouble == PANEL_TROUBLE_NONE) {
                controller->latched_trouble = PANEL_TROUBLE_NONE;
                controller->state = PANEL_STATE_IDLE;
                return make_output(controller, PANEL_COMMAND_RESET_PANEL,
                                   PANEL_ALARM_NONE, PANEL_TROUBLE_NONE);
            }

            controller->state = PANEL_STATE_TROUBLE;
            controller->latched_trouble = evaluation.trouble;
            return make_output(controller, PANEL_COMMAND_LATCH_TROUBLE,
                               PANEL_ALARM_NONE, controller->latched_trouble);
        }

        if ((controller->state == PANEL_STATE_SILENCED) &&
            (evaluation.alarm == PANEL_ALARM_MANUAL_PULL) &&
            (controller->latched_alarm != PANEL_ALARM_MANUAL_PULL)) {
            controller->latched_alarm = PANEL_ALARM_MANUAL_PULL;
            controller->state = PANEL_STATE_ALARM;
            return make_output(controller, PANEL_COMMAND_ACTIVATE_NACS,
                               controller->latched_alarm,
                               visible_trouble(controller, evaluation.trouble));
        }

        if ((controller->state == PANEL_STATE_ALARM) && input->silence_request) {
            controller->state = PANEL_STATE_SILENCED;
            return make_output(controller, PANEL_COMMAND_SILENCE_NACS,
                               controller->latched_alarm,
                               visible_trouble(controller, evaluation.trouble));
        }

        return make_output(controller,
                           (controller->state == PANEL_STATE_ALARM)
                               ? PANEL_COMMAND_ACTIVATE_NACS
                               : PANEL_COMMAND_SILENCE_NACS,
                           controller->latched_alarm,
                           visible_trouble(controller, evaluation.trouble));
    }

    if (evaluation.alarm == PANEL_ALARM_MANUAL_PULL) {
        return enter_alarm(controller, PANEL_ALARM_MANUAL_PULL, evaluation.trouble);
    }

    switch (controller->state) {
        case PANEL_STATE_IDLE:
            if (evaluation.trouble != PANEL_TROUBLE_NONE) {
                return enter_trouble(controller, evaluation.trouble);
            }
            if (evaluation.alarm == PANEL_ALARM_SMOKE) {
                controller->state = PANEL_STATE_VERIFY;
                controller->smoke_verify_ticks = 1u;
                return make_output(controller, PANEL_COMMAND_START_VERIFY,
                                   PANEL_ALARM_SMOKE, PANEL_TROUBLE_NONE);
            }
            return make_output(controller, PANEL_COMMAND_MONITOR_LOOP,
                               PANEL_ALARM_NONE, PANEL_TROUBLE_NONE);

        case PANEL_STATE_VERIFY:
            if (evaluation.trouble != PANEL_TROUBLE_NONE) {
                return enter_trouble(controller, evaluation.trouble);
            }
            if (evaluation.alarm != PANEL_ALARM_SMOKE) {
                controller->state = PANEL_STATE_IDLE;
                controller->smoke_verify_ticks = 0u;
                return make_output(controller, PANEL_COMMAND_MONITOR_LOOP,
                                   PANEL_ALARM_NONE, PANEL_TROUBLE_NONE);
            }
            if ((controller->smoke_verify_ticks + 1u) >= SMOKE_VERIFY_TARGET) {
                return enter_alarm(controller, PANEL_ALARM_SMOKE, PANEL_TROUBLE_NONE);
            }
            controller->smoke_verify_ticks++;
            return make_output(controller, PANEL_COMMAND_START_VERIFY,
                               PANEL_ALARM_SMOKE, PANEL_TROUBLE_NONE);

        case PANEL_STATE_TROUBLE:
            if (evaluation.trouble != PANEL_TROUBLE_NONE) {
                controller->latched_trouble = evaluation.trouble;
            }
            if (input->reset_request && (evaluation.trouble == PANEL_TROUBLE_NONE)) {
                controller->state = PANEL_STATE_IDLE;
                controller->latched_trouble = PANEL_TROUBLE_NONE;
                controller->smoke_verify_ticks = 0u;
                return make_output(controller, PANEL_COMMAND_RESET_PANEL,
                                   PANEL_ALARM_NONE, PANEL_TROUBLE_NONE);
            }
            return make_output(controller, PANEL_COMMAND_LATCH_TROUBLE,
                               PANEL_ALARM_NONE, controller->latched_trouble);

        case PANEL_STATE_ALARM:
        case PANEL_STATE_SILENCED:
        default:
            return make_output(controller, PANEL_COMMAND_MONITOR_LOOP,
                               PANEL_ALARM_NONE, PANEL_TROUBLE_NONE);
    }
}
