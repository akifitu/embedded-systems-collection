#ifndef CROSSING_TYPES_H
#define CROSSING_TYPES_H

#include <stdbool.h>

typedef enum {
    CROSSING_STATE_IDLE = 0,
    CROSSING_STATE_PREWARN,
    CROSSING_STATE_LOWERING,
    CROSSING_STATE_PROTECTED,
    CROSSING_STATE_RAISING,
    CROSSING_STATE_FAULT
} crossing_state_t;

typedef enum {
    CROSSING_COMMAND_GATE_UP = 0,
    CROSSING_COMMAND_START_WARNING,
    CROSSING_COMMAND_LOWER_GATE,
    CROSSING_COMMAND_HOLD_DOWN,
    CROSSING_COMMAND_RAISE_GATE,
    CROSSING_COMMAND_LATCH_FAULT
} crossing_command_t;

typedef enum {
    CROSSING_FAULT_NONE = 0,
    CROSSING_FAULT_LAMP_FAIL,
    CROSSING_FAULT_GATE_TIMEOUT
} crossing_fault_t;

typedef enum {
    TRAIN_ZONE_CLEAR = 0,
    TRAIN_ZONE_APPROACH,
    TRAIN_ZONE_ISLAND
} train_zone_t;

typedef enum {
    GATE_POSITION_UP = 0,
    GATE_POSITION_DOWN,
    GATE_POSITION_MOVING
} gate_position_t;

typedef struct {
    bool approach_active;
    bool island_occupied;
    bool gate_down_fb;
    bool gate_up_fb;
    bool lamp_ok;
    bool reset_request;
} crossing_input_t;

typedef struct {
    crossing_state_t state;
    crossing_command_t command;
    crossing_fault_t fault;
    train_zone_t zone;
    gate_position_t gate_position;
    bool warning_active;
    bool bell_active;
} crossing_output_t;

const char *crossing_state_name(crossing_state_t state);
const char *crossing_command_name(crossing_command_t command);
const char *crossing_fault_name(crossing_fault_t fault);
const char *train_zone_name(train_zone_t zone);
const char *gate_position_name(gate_position_t position);

#endif
