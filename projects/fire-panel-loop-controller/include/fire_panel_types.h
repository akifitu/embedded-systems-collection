#ifndef FIRE_PANEL_TYPES_H
#define FIRE_PANEL_TYPES_H

#include <stdbool.h>

typedef enum {
    PANEL_STATE_IDLE = 0,
    PANEL_STATE_VERIFY,
    PANEL_STATE_ALARM,
    PANEL_STATE_SILENCED,
    PANEL_STATE_TROUBLE
} panel_state_t;

typedef enum {
    PANEL_COMMAND_MONITOR_LOOP = 0,
    PANEL_COMMAND_START_VERIFY,
    PANEL_COMMAND_ACTIVATE_NACS,
    PANEL_COMMAND_SILENCE_NACS,
    PANEL_COMMAND_LATCH_TROUBLE,
    PANEL_COMMAND_RESET_PANEL
} panel_command_t;

typedef enum {
    PANEL_ALARM_NONE = 0,
    PANEL_ALARM_SMOKE,
    PANEL_ALARM_MANUAL_PULL
} panel_alarm_t;

typedef enum {
    PANEL_TROUBLE_NONE = 0,
    PANEL_TROUBLE_LOOP_OPEN,
    PANEL_TROUBLE_SENSOR_DIRTY
} panel_trouble_t;

typedef struct {
    bool smoke_alarm;
    bool manual_pull;
    bool loop_open;
    bool sensor_dirty;
    bool silence_request;
    bool reset_request;
} fire_panel_input_t;

typedef struct {
    panel_state_t state;
    panel_command_t command;
    panel_alarm_t alarm;
    panel_trouble_t trouble;
    bool nac_active;
    bool buzzer_active;
    bool alarm_led;
    bool trouble_led;
    unsigned verify_percent;
} fire_panel_output_t;

const char *panel_state_name(panel_state_t state);
const char *panel_command_name(panel_command_t command);
const char *panel_alarm_name(panel_alarm_t alarm);
const char *panel_trouble_name(panel_trouble_t trouble);

#endif
