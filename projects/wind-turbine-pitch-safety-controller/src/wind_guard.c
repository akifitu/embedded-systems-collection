#include "wind_guard.h"

#define START_WIND_MIN_DMPS 70u
#define START_WIND_MAX_DMPS 200u
#define STORM_HOLD_WIND_DMPS 250u
#define STORM_RELEASE_WIND_DMPS 180u
#define PARKED_RPM_MAX 200u
#define STARTUP_RPM_MAX 300u

turbine_fault_t wind_guard_fault(const turbine_input_t *input) {
    if (!input->hydraulic_ok) {
        return TURBINE_FAULT_HYDRAULIC_LOW;
    }
    if (!input->pitch_sensor_ok) {
        return TURBINE_FAULT_PITCH_SENSOR_MISMATCH;
    }
    return TURBINE_FAULT_NONE;
}

bool wind_guard_can_start(const turbine_input_t *input) {
    return input->start_request && input->grid_available && input->hydraulic_ok &&
           input->pitch_sensor_ok &&
           (input->wind_dmps >= START_WIND_MIN_DMPS) &&
           (input->wind_dmps <= START_WIND_MAX_DMPS) &&
           (input->rotor_rpm <= STARTUP_RPM_MAX);
}

bool wind_guard_should_storm_hold(const turbine_input_t *input) {
    return input->wind_dmps >= STORM_HOLD_WIND_DMPS;
}

bool wind_guard_should_feather(const turbine_input_t *input) {
    return !input->grid_available;
}

bool wind_guard_rotor_slow(const turbine_input_t *input) {
    return input->rotor_rpm <= PARKED_RPM_MAX;
}

bool wind_guard_can_park(const turbine_input_t *input) {
    return wind_guard_rotor_slow(input) &&
           (input->wind_dmps < STORM_RELEASE_WIND_DMPS);
}
