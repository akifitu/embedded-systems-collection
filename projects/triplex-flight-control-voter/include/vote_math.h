#ifndef VOTE_MATH_H
#define VOTE_MATH_H

#include "triplex_types.h"

typedef enum {
    TRIPLEX_VOTE_SYNC_OK = 0,
    TRIPLEX_VOTE_DEGRADED_OK,
    TRIPLEX_VOTE_ISOLATE_A,
    TRIPLEX_VOTE_ISOLATE_B,
    TRIPLEX_VOTE_ISOLATE_C,
    TRIPLEX_VOTE_NO_MAJORITY,
    TRIPLEX_VOTE_DUAL_DISAGREE,
    TRIPLEX_VOTE_INSUFFICIENT
} triplex_vote_status_t;

typedef struct {
    triplex_vote_status_t status;
    unsigned voted_command_bp;
    unsigned spread_bp;
} triplex_vote_result_t;

triplex_vote_result_t vote_math_evaluate(const triplex_input_t *input,
                                         unsigned active_mask);

#endif
