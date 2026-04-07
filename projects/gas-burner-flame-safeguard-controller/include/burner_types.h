#ifndef BURNER_TYPES_H
#define BURNER_TYPES_H

#include <stdbool.h>

typedef enum {
    BURNER_STATE_IDLE = 0,
    BURNER_STATE_PREPURGE,
    BURNER_STATE_IGNITION,
    BURNER_STATE_RUNNING,
    BURNER_STATE_POSTPURGE,
    BURNER_STATE_LOCKOUT
} burner_state_t;

typedef enum {
    BURNER_COMMAND_HOLD_OFF = 0,
    BURNER_COMMAND_START_PREPURGE,
    BURNER_COMMAND_TRIAL_IGNITE,
    BURNER_COMMAND_HOLD_FLAME,
    BURNER_COMMAND_POSTPURGE_FAN,
    BURNER_COMMAND_LATCH_LOCKOUT,
    BURNER_COMMAND_RESET_BURNER
} burner_command_t;

typedef enum {
    BURNER_FAULT_NONE = 0,
    BURNER_FAULT_AIRFLOW_FAIL,
    BURNER_FAULT_IGNITION_FAIL,
    BURNER_FAULT_INTERLOCK_OPEN,
    BURNER_FAULT_FLAME_STUCK
} burner_fault_t;

typedef struct {
    bool call_for_heat;
    bool airflow_proven;
    bool flame_present;
    bool limit_closed;
    bool reset_request;
} burner_input_t;

typedef struct {
    burner_state_t state;
    burner_command_t command;
    burner_fault_t fault;
    bool fan_active;
    bool igniter_active;
    bool gas_valve_open;
    bool flame_proven;
    unsigned progress_pct;
} burner_output_t;

const char *burner_state_name(burner_state_t state);
const char *burner_command_name(burner_command_t command);
const char *burner_fault_name(burner_fault_t fault);

#endif
