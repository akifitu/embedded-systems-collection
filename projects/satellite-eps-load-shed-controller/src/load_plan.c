#include "load_plan.h"

load_plan_output_t load_plan_apply(eps_state_t state, const eps_input_t *input) {
    load_plan_output_t output;

    output.payload_enabled = false;
    output.heater_enabled = false;
    output.radio_high_rate = false;
    output.adcs_full_rate = false;
    output.budget_w = 5u;

    switch (state) {
        case EPS_STATE_NOMINAL:
            output.payload_enabled = input->payload_request_w > 0u;
            output.heater_enabled = input->heater_request;
            output.radio_high_rate = true;
            output.adcs_full_rate = true;
            output.budget_w = input->panel_power_w;
            break;

        case EPS_STATE_SHED:
            output.heater_enabled =
                input->heater_request && input->in_sunlight &&
                (input->panel_power_w >= 25u);
            output.radio_high_rate = false;
            output.adcs_full_rate = true;
            output.budget_w = input->in_sunlight ? 24u : 16u;
            break;

        case EPS_STATE_SURVIVAL:
            output.budget_w = 8u;
            break;

        case EPS_STATE_RECOVERY:
            output.heater_enabled = input->heater_request;
            output.adcs_full_rate = true;
            output.budget_w = 24u;
            break;

        case EPS_STATE_FAULT:
        default:
            output.budget_w = 5u;
            break;
    }

    return output;
}
