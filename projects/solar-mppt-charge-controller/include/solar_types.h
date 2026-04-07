#ifndef SOLAR_TYPES_H
#define SOLAR_TYPES_H

#include <stdbool.h>

typedef enum {
    SOLAR_STATE_IDLE = 0,
    SOLAR_STATE_BULK,
    SOLAR_STATE_ABSORB,
    SOLAR_STATE_FLOAT,
    SOLAR_STATE_FAULT
} solar_state_t;

typedef enum {
    SOLAR_COMMAND_OPEN_RELAY = 0,
    SOLAR_COMMAND_SEEK_MPP,
    SOLAR_COMMAND_HOLD_ABSORB,
    SOLAR_COMMAND_HOLD_FLOAT,
    SOLAR_COMMAND_LATCH_FAULT,
    SOLAR_COMMAND_RESET_CHARGER
} solar_command_t;

typedef enum {
    SOLAR_FAULT_NONE = 0,
    SOLAR_FAULT_BATTERY_OVERVOLT,
    SOLAR_FAULT_PANEL_REVERSE,
    SOLAR_FAULT_OVER_TEMP
} solar_fault_t;

typedef struct {
    bool charge_enable;
    bool reset_request;
    bool reverse_polarity;
    unsigned panel_mv;
    unsigned panel_ma;
    unsigned battery_mv;
    unsigned charger_temp_c;
} solar_input_t;

typedef struct {
    solar_state_t state;
    solar_command_t command;
    solar_fault_t fault;
    unsigned duty_pct;
    bool relay_closed;
    bool fan_active;
    unsigned stage_target_mv;
    unsigned panel_mv;
    unsigned panel_ma;
    unsigned panel_power_dmW;
    unsigned battery_mv;
    unsigned charger_temp_c;
} solar_output_t;

const char *solar_state_name(solar_state_t state);
const char *solar_command_name(solar_command_t command);
const char *solar_fault_name(solar_fault_t fault);

#endif
