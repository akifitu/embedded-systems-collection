#include "fire_panel_types.h"

const char *panel_state_name(panel_state_t state) {
    switch (state) {
        case PANEL_STATE_IDLE:
            return "IDLE";
        case PANEL_STATE_VERIFY:
            return "VERIFY";
        case PANEL_STATE_ALARM:
            return "ALARM";
        case PANEL_STATE_SILENCED:
            return "SILENCED";
        case PANEL_STATE_TROUBLE:
            return "TROUBLE";
        default:
            return "UNKNOWN";
    }
}

const char *panel_command_name(panel_command_t command) {
    switch (command) {
        case PANEL_COMMAND_MONITOR_LOOP:
            return "MONITOR_LOOP";
        case PANEL_COMMAND_START_VERIFY:
            return "START_VERIFY";
        case PANEL_COMMAND_ACTIVATE_NACS:
            return "ACTIVATE_NACS";
        case PANEL_COMMAND_SILENCE_NACS:
            return "SILENCE_NACS";
        case PANEL_COMMAND_LATCH_TROUBLE:
            return "LATCH_TROUBLE";
        case PANEL_COMMAND_RESET_PANEL:
            return "RESET_PANEL";
        default:
            return "UNKNOWN";
    }
}

const char *panel_alarm_name(panel_alarm_t alarm) {
    switch (alarm) {
        case PANEL_ALARM_NONE:
            return "NONE";
        case PANEL_ALARM_SMOKE:
            return "SMOKE";
        case PANEL_ALARM_MANUAL_PULL:
            return "MANUAL_PULL";
        default:
            return "UNKNOWN";
    }
}

const char *panel_trouble_name(panel_trouble_t trouble) {
    switch (trouble) {
        case PANEL_TROUBLE_NONE:
            return "NONE";
        case PANEL_TROUBLE_LOOP_OPEN:
            return "LOOP_OPEN";
        case PANEL_TROUBLE_SENSOR_DIRTY:
            return "SENSOR_DIRTY";
        default:
            return "UNKNOWN";
    }
}
