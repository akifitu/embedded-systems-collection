#include "inverter_guard_types.h"

const char *inverter_state_name(inverter_state_t state) {
    switch (state) {
        case INVERTER_STATE_WAIT_GRID:
            return "WAIT_GRID";
        case INVERTER_STATE_SYNC:
            return "SYNC";
        case INVERTER_STATE_EXPORT:
            return "EXPORT";
        case INVERTER_STATE_TRIP:
            return "TRIP";
        case INVERTER_STATE_COOLDOWN:
            return "COOLDOWN";
        default:
            return "UNKNOWN";
    }
}

const char *inverter_command_name(inverter_command_t command) {
    switch (command) {
        case INVERTER_COMMAND_OPEN_RELAY:
            return "OPEN_RELAY";
        case INVERTER_COMMAND_TRACK_GRID:
            return "TRACK_GRID";
        case INVERTER_COMMAND_CLOSE_RELAY:
            return "CLOSE_RELAY";
        case INVERTER_COMMAND_EXPORT_POWER:
            return "EXPORT_POWER";
        case INVERTER_COMMAND_HOLD_OFF:
            return "HOLD_OFF";
        default:
            return "UNKNOWN";
    }
}

const char *inverter_reason_name(inverter_reason_t reason) {
    switch (reason) {
        case INVERTER_REASON_NONE:
            return "NONE";
        case INVERTER_REASON_GRID_LOSS:
            return "GRID_LOSS";
        case INVERTER_REASON_UNDERVOLTAGE:
            return "UNDERVOLTAGE";
        case INVERTER_REASON_OVERVOLTAGE:
            return "OVERVOLTAGE";
        case INVERTER_REASON_UNDERFREQUENCY:
            return "UNDERFREQUENCY";
        case INVERTER_REASON_OVERFREQUENCY:
            return "OVERFREQUENCY";
        case INVERTER_REASON_ROCOF:
            return "ROCOF";
        case INVERTER_REASON_OVERTEMP:
            return "OVERTEMP";
        case INVERTER_REASON_DC_LINK_HIGH:
            return "DC_LINK_HIGH";
        case INVERTER_REASON_RELAY_FAULT:
            return "RELAY_FAULT";
        case INVERTER_REASON_COOLDOWN:
            return "COOLDOWN";
        default:
            return "UNKNOWN";
    }
}

const char *inverter_quality_name(inverter_quality_t quality) {
    switch (quality) {
        case INVERTER_QUALITY_SEARCH:
            return "SEARCH";
        case INVERTER_QUALITY_LOCKING:
            return "LOCKING";
        case INVERTER_QUALITY_LOCKED:
            return "LOCKED";
        case INVERTER_QUALITY_DERATED:
            return "DERATED";
        case INVERTER_QUALITY_FAULT:
            return "FAULT";
        case INVERTER_QUALITY_RECOVERING:
            return "RECOVERING";
        default:
            return "UNKNOWN";
    }
}
