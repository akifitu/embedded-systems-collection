#include "solar_types.h"

const char *solar_state_name(solar_state_t state) {
    switch (state) {
        case SOLAR_STATE_IDLE:
            return "IDLE";
        case SOLAR_STATE_BULK:
            return "BULK";
        case SOLAR_STATE_ABSORB:
            return "ABSORB";
        case SOLAR_STATE_FLOAT:
            return "FLOAT";
        case SOLAR_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *solar_command_name(solar_command_t command) {
    switch (command) {
        case SOLAR_COMMAND_OPEN_RELAY:
            return "OPEN_RELAY";
        case SOLAR_COMMAND_SEEK_MPP:
            return "SEEK_MPP";
        case SOLAR_COMMAND_HOLD_ABSORB:
            return "HOLD_ABSORB";
        case SOLAR_COMMAND_HOLD_FLOAT:
            return "HOLD_FLOAT";
        case SOLAR_COMMAND_LATCH_FAULT:
            return "LATCH_FAULT";
        case SOLAR_COMMAND_RESET_CHARGER:
            return "RESET_CHARGER";
        default:
            return "UNKNOWN";
    }
}

const char *solar_fault_name(solar_fault_t fault) {
    switch (fault) {
        case SOLAR_FAULT_NONE:
            return "NONE";
        case SOLAR_FAULT_BATTERY_OVERVOLT:
            return "BATTERY_OVERVOLT";
        case SOLAR_FAULT_PANEL_REVERSE:
            return "PANEL_REVERSE";
        case SOLAR_FAULT_OVER_TEMP:
            return "OVER_TEMP";
        default:
            return "UNKNOWN";
    }
}
