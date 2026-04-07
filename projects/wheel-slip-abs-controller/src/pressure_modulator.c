#include "pressure_modulator.h"

abs_valve_t pressure_modulator_command(uint8_t slip_pct) {
    if (slip_pct >= 22u) {
        return ABS_VALVE_RELEASE;
    }
    if (slip_pct >= 14u) {
        return ABS_VALVE_HOLD;
    }
    return ABS_VALVE_APPLY;
}
