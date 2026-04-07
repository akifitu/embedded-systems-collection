#ifndef GENERATOR_TYPES_H
#define GENERATOR_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    GEN_STATE_IDLE = 0,
    GEN_STATE_START_DELAY,
    GEN_STATE_CRANKING,
    GEN_STATE_WARMUP,
    GEN_STATE_RUNNING,
    GEN_STATE_COOLDOWN,
    GEN_STATE_FAULT
} gen_state_t;

typedef enum {
    GEN_COMMAND_OPEN_CONTACTOR = 0,
    GEN_COMMAND_PREHEAT,
    GEN_COMMAND_CRANK_START,
    GEN_COMMAND_CLOSE_CONTACTOR,
    GEN_COMMAND_COOL_ENGINE,
    GEN_COMMAND_STOP_ENGINE
} gen_command_t;

typedef enum {
    GEN_FAULT_NONE = 0,
    GEN_FAULT_LOW_BATTERY,
    GEN_FAULT_START_FAIL,
    GEN_FAULT_LOW_OIL,
    GEN_FAULT_E_STOP
} gen_fault_t;

typedef struct {
    bool utility_present;
    uint16_t battery_mv;
    uint16_t engine_rpm;
    uint16_t oil_pressure_kpa;
    bool fuel_ok;
    bool emergency_stop;
    bool reset_request;
} gen_input_t;

typedef struct {
    gen_state_t state;
    gen_command_t command;
    gen_fault_t fault;
    uint8_t start_attempts;
    bool contactor_closed;
} gen_output_t;

const char *gen_state_name(gen_state_t state);
const char *gen_command_name(gen_command_t command);
const char *gen_fault_name(gen_fault_t fault);

#endif
