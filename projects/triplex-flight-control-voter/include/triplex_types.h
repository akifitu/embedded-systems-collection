#ifndef TRIPLEX_TYPES_H
#define TRIPLEX_TYPES_H

#include <stdbool.h>

#define TRIPLEX_MASK_A 0x1u
#define TRIPLEX_MASK_B 0x2u
#define TRIPLEX_MASK_C 0x4u
#define TRIPLEX_MASK_ALL (TRIPLEX_MASK_A | TRIPLEX_MASK_B | TRIPLEX_MASK_C)

typedef enum {
    TRIPLEX_STATE_IDLE = 0,
    TRIPLEX_STATE_SYNC,
    TRIPLEX_STATE_DEGRADED,
    TRIPLEX_STATE_FAULT
} triplex_state_t;

typedef enum {
    TRIPLEX_COMMAND_HOLD_LAST_GOOD = 0,
    TRIPLEX_COMMAND_VOTE_OUTPUT,
    TRIPLEX_COMMAND_ISOLATE_LANE,
    TRIPLEX_COMMAND_LATCH_FAULT,
    TRIPLEX_COMMAND_RESET_VOTER
} triplex_command_t;

typedef enum {
    TRIPLEX_FAULT_NONE = 0,
    TRIPLEX_FAULT_LANE_A_DRIFT,
    TRIPLEX_FAULT_LANE_B_DRIFT,
    TRIPLEX_FAULT_LANE_C_DRIFT,
    TRIPLEX_FAULT_LANE_TIMEOUT,
    TRIPLEX_FAULT_SPLIT_VOTE,
    TRIPLEX_FAULT_MULTI_LANE_FAIL
} triplex_fault_t;

typedef enum {
    TRIPLEX_LANE_NONE = 0,
    TRIPLEX_LANE_A,
    TRIPLEX_LANE_B,
    TRIPLEX_LANE_C
} triplex_lane_t;

typedef struct {
    bool enable_request;
    bool reset_request;
    bool lane_a_valid;
    bool lane_b_valid;
    bool lane_c_valid;
    bool lane_a_timeout;
    bool lane_b_timeout;
    bool lane_c_timeout;
    unsigned lane_a_command_bp;
    unsigned lane_b_command_bp;
    unsigned lane_c_command_bp;
} triplex_input_t;

typedef struct {
    triplex_state_t state;
    triplex_command_t command;
    triplex_fault_t fault;
    unsigned active_mask;
    triplex_lane_t isolated_lane;
    unsigned voted_command_bp;
    unsigned spread_bp;
    bool servo_enabled;
} triplex_output_t;

const char *triplex_state_name(triplex_state_t state);
const char *triplex_command_name(triplex_command_t command);
const char *triplex_fault_name(triplex_fault_t fault);
const char *triplex_lane_name(triplex_lane_t lane);
const char *triplex_active_mask_name(unsigned mask);

#endif
