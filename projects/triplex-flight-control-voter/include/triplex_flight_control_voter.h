#ifndef TRIPLEX_FLIGHT_CONTROL_VOTER_H
#define TRIPLEX_FLIGHT_CONTROL_VOTER_H

#include "triplex_types.h"

typedef struct {
    triplex_state_t state;
    triplex_fault_t latched_fault;
    triplex_lane_t isolated_lane;
    unsigned last_voted_command_bp;
} triplex_flight_control_voter_t;

void triplex_flight_control_voter_init(
    triplex_flight_control_voter_t *controller);
triplex_output_t triplex_flight_control_voter_step(
    triplex_flight_control_voter_t *controller,
    const triplex_input_t *input);

#endif
