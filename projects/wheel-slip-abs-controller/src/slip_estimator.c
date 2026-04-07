#include "slip_estimator.h"

static uint8_t clamp_slip(unsigned value) {
    return (value > 100u) ? 100u : (uint8_t)value;
}

abs_fault_t slip_estimator_calculate(const abs_input_t *input,
                                     uint8_t slip_pct[ABS_WHEEL_COUNT]) {
    unsigned wheel;

    for (wheel = 0u; wheel < ABS_WHEEL_COUNT; ++wheel) {
        slip_pct[wheel] = 0u;
    }

    if ((input->vehicle_speed_kph_x10 < 30u) || (input->brake_pct < 5u)) {
        return ABS_FAULT_NONE;
    }

    for (wheel = 0u; wheel < ABS_WHEEL_COUNT; ++wheel) {
        unsigned delta;

        if (input->wheel_speed_kph_x10[wheel] >
            (uint16_t)(input->vehicle_speed_kph_x10 + 250u)) {
            return ABS_FAULT_WHEEL_SENSOR;
        }

        if (input->wheel_speed_kph_x10[wheel] >= input->vehicle_speed_kph_x10) {
            slip_pct[wheel] = 0u;
            continue;
        }

        delta = (unsigned)(input->vehicle_speed_kph_x10 -
                           input->wheel_speed_kph_x10[wheel]);
        slip_pct[wheel] =
            clamp_slip((delta * 100u) / input->vehicle_speed_kph_x10);
    }

    return ABS_FAULT_NONE;
}
