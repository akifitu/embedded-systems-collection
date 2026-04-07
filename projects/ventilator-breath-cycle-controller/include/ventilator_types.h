#ifndef VENTILATOR_TYPES_H
#define VENTILATOR_TYPES_H

#include <stdbool.h>

typedef enum {
    VENT_STATE_STANDBY = 0,
    VENT_STATE_INHALE,
    VENT_STATE_HOLD,
    VENT_STATE_EXHALE,
    VENT_STATE_BACKUP,
    VENT_STATE_ALARM
} vent_state_t;

typedef enum {
    VENT_COMMAND_HOLD_STANDBY = 0,
    VENT_COMMAND_DELIVER_INHALE,
    VENT_COMMAND_HOLD_PLATEAU,
    VENT_COMMAND_VENT_EXHALE,
    VENT_COMMAND_START_BACKUP,
    VENT_COMMAND_LATCH_ALARM,
    VENT_COMMAND_RESET_VENT
} vent_command_t;

typedef enum {
    VENT_FAULT_NONE = 0,
    VENT_FAULT_HIGH_PRESSURE,
    VENT_FAULT_DISCONNECT,
    VENT_FAULT_GAS_SUPPLY_LOSS,
    VENT_FAULT_SENSOR_FAULT
} vent_fault_t;

typedef struct {
    bool therapy_enabled;
    bool patient_trigger;
    bool gas_supply_ok;
    bool pressure_sensor_ok;
    bool reset_request;
    unsigned airway_pressure_cmh2o;
    unsigned flow_lpm;
} vent_input_t;

typedef struct {
    vent_state_t state;
    vent_command_t command;
    vent_fault_t fault;
    bool backup_active;
    unsigned blower_pct;
    bool inspiratory_valve_open;
    bool expiratory_valve_open;
    unsigned target_pressure_cmh2o;
    unsigned airway_pressure_cmh2o;
    unsigned flow_lpm;
} vent_output_t;

const char *vent_state_name(vent_state_t state);
const char *vent_command_name(vent_command_t command);
const char *vent_fault_name(vent_fault_t fault);

#endif
