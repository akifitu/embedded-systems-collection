#include "pitch_plan.h"

pitch_plan_output_t pitch_plan_apply(turbine_state_t state,
                                     const turbine_input_t *input) {
    pitch_plan_output_t output;

    output.pitch_deg = 90u;
    output.generator_enabled = false;
    output.brake_applied = true;

    switch (state) {
        case TURBINE_STATE_STARTUP:
            output.pitch_deg = 18u;
            output.brake_applied = false;
            break;

        case TURBINE_STATE_GENERATING:
            output.generator_enabled = true;
            output.brake_applied = false;
            if ((input->rotor_rpm >= 1600u) || (input->wind_dmps >= 140u)) {
                output.pitch_deg = 18u;
            } else if (input->rotor_rpm >= 1200u) {
                output.pitch_deg = 14u;
            } else {
                output.pitch_deg = 10u;
            }
            break;

        case TURBINE_STATE_FEATHERING:
            output.pitch_deg = 82u;
            output.brake_applied = false;
            break;

        case TURBINE_STATE_STORM_HOLD:
            output.pitch_deg = 88u;
            break;

        case TURBINE_STATE_PARKED:
        case TURBINE_STATE_FAULT:
        default:
            output.pitch_deg = 90u;
            break;
    }

    return output;
}
