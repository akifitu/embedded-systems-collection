#include "grid_monitor.h"

#define GRID_SYNC_VOLTAGE_MIN_V 215u
#define GRID_SYNC_VOLTAGE_MAX_V 245u
#define GRID_TRIP_UNDERVOLTAGE_V 200u
#define GRID_TRIP_OVERVOLTAGE_V 258u
#define GRID_SYNC_FREQ_MIN_MHZ 49800u
#define GRID_SYNC_FREQ_MAX_MHZ 50200u
#define GRID_TRIP_UNDERFREQ_MHZ 49000u
#define GRID_TRIP_OVERFREQ_MHZ 51000u
#define GRID_SYNC_ROCOF_MAX_MHZ_S 200u
#define GRID_TRIP_ROCOF_MAX_MHZ_S 800u
#define GRID_DC_LINK_TRIP_V 430u
#define GRID_OVERTEMP_TRIP_C_X10 980

grid_assessment_t grid_monitor_assess(const inverter_input_t *input) {
    grid_assessment_t assessment;

    assessment.stable = false;
    assessment.fault_reason = INVERTER_REASON_NONE;

    if (input->dc_link_v >= GRID_DC_LINK_TRIP_V) {
        assessment.fault_reason = INVERTER_REASON_DC_LINK_HIGH;
        return assessment;
    }

    if (input->heatsink_c_x10 >= GRID_OVERTEMP_TRIP_C_X10) {
        assessment.fault_reason = INVERTER_REASON_OVERTEMP;
        return assessment;
    }

    if (!input->grid_present) {
        return assessment;
    }

    if (input->voltage_rms_v < GRID_TRIP_UNDERVOLTAGE_V) {
        assessment.fault_reason = INVERTER_REASON_UNDERVOLTAGE;
        return assessment;
    }

    if (input->voltage_rms_v > GRID_TRIP_OVERVOLTAGE_V) {
        assessment.fault_reason = INVERTER_REASON_OVERVOLTAGE;
        return assessment;
    }

    if (input->frequency_mhz < GRID_TRIP_UNDERFREQ_MHZ) {
        assessment.fault_reason = INVERTER_REASON_UNDERFREQUENCY;
        return assessment;
    }

    if (input->frequency_mhz > GRID_TRIP_OVERFREQ_MHZ) {
        assessment.fault_reason = INVERTER_REASON_OVERFREQUENCY;
        return assessment;
    }

    if (input->rocof_mhz_s > GRID_TRIP_ROCOF_MAX_MHZ_S) {
        assessment.fault_reason = INVERTER_REASON_ROCOF;
        return assessment;
    }

    assessment.stable =
        (input->voltage_rms_v >= GRID_SYNC_VOLTAGE_MIN_V) &&
        (input->voltage_rms_v <= GRID_SYNC_VOLTAGE_MAX_V) &&
        (input->frequency_mhz >= GRID_SYNC_FREQ_MIN_MHZ) &&
        (input->frequency_mhz <= GRID_SYNC_FREQ_MAX_MHZ) &&
        (input->rocof_mhz_s <= GRID_SYNC_ROCOF_MAX_MHZ_S);
    return assessment;
}
