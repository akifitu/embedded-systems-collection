#ifndef TURBINE_TYPES_H
#define TURBINE_TYPES_H

#include <stdbool.h>

typedef enum {
    TURBINE_STATE_PARKED = 0,
    TURBINE_STATE_STARTUP,
    TURBINE_STATE_GENERATING,
    TURBINE_STATE_FEATHERING,
    TURBINE_STATE_STORM_HOLD,
    TURBINE_STATE_FAULT
} turbine_state_t;

typedef enum {
    TURBINE_COMMAND_HOLD_PARK = 0,
    TURBINE_COMMAND_STARTUP_RELEASE,
    TURBINE_COMMAND_TRACK_POWER,
    TURBINE_COMMAND_FEATHER_BLADES,
    TURBINE_COMMAND_HOLD_STORM,
    TURBINE_COMMAND_LATCH_FAULT,
    TURBINE_COMMAND_RESET_TURBINE
} turbine_command_t;

typedef enum {
    TURBINE_FAULT_NONE = 0,
    TURBINE_FAULT_HYDRAULIC_LOW,
    TURBINE_FAULT_PITCH_SENSOR_MISMATCH
} turbine_fault_t;

typedef struct {
    bool start_request;
    bool reset_request;
    bool grid_available;
    bool hydraulic_ok;
    bool pitch_sensor_ok;
    unsigned wind_dmps;
    unsigned rotor_rpm;
} turbine_input_t;

typedef struct {
    turbine_state_t state;
    turbine_command_t command;
    turbine_fault_t fault;
    unsigned pitch_deg;
    bool generator_enabled;
    bool brake_applied;
    unsigned wind_dmps;
    unsigned rotor_rpm;
} turbine_output_t;

const char *turbine_state_name(turbine_state_t state);
const char *turbine_command_name(turbine_command_t command);
const char *turbine_fault_name(turbine_fault_t fault);

#endif
