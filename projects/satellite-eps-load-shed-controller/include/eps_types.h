#ifndef EPS_TYPES_H
#define EPS_TYPES_H

#include <stdbool.h>

typedef enum {
    EPS_STATE_NOMINAL = 0,
    EPS_STATE_SHED,
    EPS_STATE_SURVIVAL,
    EPS_STATE_RECOVERY,
    EPS_STATE_FAULT
} eps_state_t;

typedef enum {
    EPS_COMMAND_HOLD_FULL_POWER = 0,
    EPS_COMMAND_SHED_NONCRITICAL,
    EPS_COMMAND_ENTER_SURVIVAL,
    EPS_COMMAND_RESTORE_LOADS,
    EPS_COMMAND_LATCH_FAULT,
    EPS_COMMAND_RESET_EPS
} eps_command_t;

typedef enum {
    EPS_FAULT_NONE = 0,
    EPS_FAULT_BUS_UNDERVOLTAGE,
    EPS_FAULT_BATTERY_DEEP_DISCHARGE
} eps_fault_t;

typedef struct {
    bool in_sunlight;
    unsigned battery_soc_pct;
    unsigned bus_mv;
    unsigned panel_power_w;
    unsigned payload_request_w;
    bool heater_request;
    bool reset_request;
} eps_input_t;

typedef struct {
    eps_state_t state;
    eps_command_t command;
    eps_fault_t fault;
    bool payload_enabled;
    bool heater_enabled;
    bool radio_high_rate;
    bool adcs_full_rate;
    unsigned budget_w;
    bool in_sunlight;
    unsigned battery_soc_pct;
    unsigned bus_mv;
} eps_output_t;

const char *eps_state_name(eps_state_t state);
const char *eps_command_name(eps_command_t command);
const char *eps_fault_name(eps_fault_t fault);

#endif
