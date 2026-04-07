#include "breath_profile.h"

breath_profile_output_t breath_profile_apply(vent_state_t state) {
    breath_profile_output_t output;

    output.backup_active = false;
    output.blower_pct = 0u;
    output.inspiratory_valve_open = false;
    output.expiratory_valve_open = true;
    output.target_pressure_cmh2o = 0u;

    switch (state) {
        case VENT_STATE_INHALE:
            output.blower_pct = 68u;
            output.inspiratory_valve_open = true;
            output.expiratory_valve_open = false;
            output.target_pressure_cmh2o = 18u;
            break;

        case VENT_STATE_HOLD:
            output.blower_pct = 28u;
            output.expiratory_valve_open = false;
            output.target_pressure_cmh2o = 18u;
            break;

        case VENT_STATE_EXHALE:
            output.target_pressure_cmh2o = 5u;
            break;

        case VENT_STATE_BACKUP:
            output.backup_active = true;
            output.blower_pct = 72u;
            output.inspiratory_valve_open = true;
            output.expiratory_valve_open = false;
            output.target_pressure_cmh2o = 20u;
            break;

        case VENT_STATE_ALARM:
        case VENT_STATE_STANDBY:
        default:
            break;
    }

    return output;
}
