#include "charge_policy.h"

#define SOLAR_ABSORB_TARGET_MV 14400u
#define SOLAR_FLOAT_TARGET_MV 13600u

charge_policy_output_t charge_policy_apply(solar_state_t state,
                                           const solar_input_t *input) {
    charge_policy_output_t output;

    output.relay_closed = false;
    output.fan_active = false;
    output.stage_target_mv = 0u;

    switch (state) {
        case SOLAR_STATE_BULK:
            output.relay_closed = true;
            output.stage_target_mv = SOLAR_ABSORB_TARGET_MV;
            break;

        case SOLAR_STATE_ABSORB:
            output.relay_closed = true;
            output.fan_active = input->charger_temp_c >= 40u;
            output.stage_target_mv = SOLAR_ABSORB_TARGET_MV;
            break;

        case SOLAR_STATE_FLOAT:
            output.relay_closed = true;
            output.stage_target_mv = SOLAR_FLOAT_TARGET_MV;
            break;

        case SOLAR_STATE_FAULT:
            output.fan_active = input->charger_temp_c >= 70u;
            break;

        case SOLAR_STATE_IDLE:
        default:
            break;
    }

    return output;
}
