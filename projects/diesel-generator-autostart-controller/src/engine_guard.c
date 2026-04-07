#include "engine_guard.h"

#define GEN_LOW_BATTERY_MV 23000u
#define GEN_START_RPM 1200u
#define GEN_MIN_OIL_KPA 180u

bool engine_guard_battery_ok(const gen_input_t *input) {
    return input->battery_mv >= GEN_LOW_BATTERY_MV;
}

bool engine_guard_started(const gen_input_t *input) {
    return (input->engine_rpm >= GEN_START_RPM) &&
           (input->oil_pressure_kpa >= GEN_MIN_OIL_KPA) && input->fuel_ok;
}

bool engine_guard_oil_ok(const gen_input_t *input) {
    return input->oil_pressure_kpa >= GEN_MIN_OIL_KPA;
}
