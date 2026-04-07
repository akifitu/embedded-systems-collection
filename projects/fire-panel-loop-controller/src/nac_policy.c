#include "nac_policy.h"

nac_policy_output_t nac_policy_apply(panel_state_t state, panel_trouble_t trouble) {
    nac_policy_output_t output;

    output.nac_active = false;
    output.buzzer_active = false;
    output.alarm_led = false;
    output.trouble_led = (trouble != PANEL_TROUBLE_NONE);

    switch (state) {
        case PANEL_STATE_IDLE:
            break;

        case PANEL_STATE_VERIFY:
            output.alarm_led = true;
            break;

        case PANEL_STATE_ALARM:
            output.nac_active = true;
            output.buzzer_active = true;
            output.alarm_led = true;
            break;

        case PANEL_STATE_SILENCED:
            output.alarm_led = true;
            break;

        case PANEL_STATE_TROUBLE:
            output.buzzer_active = true;
            break;

        default:
            break;
    }

    return output;
}
