#include <assert.h>

#include "triplex_flight_control_voter.h"

static triplex_input_t make_input(bool enable_request, bool reset_request,
                                  bool lane_a_valid, bool lane_b_valid,
                                  bool lane_c_valid, bool lane_a_timeout,
                                  bool lane_b_timeout, bool lane_c_timeout,
                                  unsigned lane_a_command_bp,
                                  unsigned lane_b_command_bp,
                                  unsigned lane_c_command_bp) {
    triplex_input_t input;

    input.enable_request = enable_request;
    input.reset_request = reset_request;
    input.lane_a_valid = lane_a_valid;
    input.lane_b_valid = lane_b_valid;
    input.lane_c_valid = lane_c_valid;
    input.lane_a_timeout = lane_a_timeout;
    input.lane_b_timeout = lane_b_timeout;
    input.lane_c_timeout = lane_c_timeout;
    input.lane_a_command_bp = lane_a_command_bp;
    input.lane_b_command_bp = lane_b_command_bp;
    input.lane_c_command_bp = lane_c_command_bp;
    return input;
}

static void test_idle_holds_last_good_when_disabled(void) {
    triplex_flight_control_voter_t controller;
    triplex_input_t input;
    triplex_output_t output;

    triplex_flight_control_voter_init(&controller);
    input = make_input(false, false, true, true, true, false, false, false,
                       0u, 0u, 0u);
    output = triplex_flight_control_voter_step(&controller, &input);

    assert(output.state == TRIPLEX_STATE_IDLE);
    assert(output.command == TRIPLEX_COMMAND_HOLD_LAST_GOOD);
    assert(!output.servo_enabled);
}

static void test_triplex_sync_votes_average(void) {
    triplex_flight_control_voter_t controller;
    triplex_input_t input;
    triplex_output_t output;

    triplex_flight_control_voter_init(&controller);
    input = make_input(true, false, true, true, true, false, false, false,
                       4200u, 4210u, 4190u);
    output = triplex_flight_control_voter_step(&controller, &input);

    assert(output.state == TRIPLEX_STATE_SYNC);
    assert(output.command == TRIPLEX_COMMAND_VOTE_OUTPUT);
    assert(output.voted_command_bp == 4200u);
    assert(output.active_mask == TRIPLEX_MASK_ALL);
}

static void test_lane_c_drift_isolated(void) {
    triplex_flight_control_voter_t controller;
    triplex_input_t input;
    triplex_output_t output;

    triplex_flight_control_voter_init(&controller);
    input = make_input(true, false, true, true, true, false, false, false,
                       4200u, 4210u, 4190u);
    (void)triplex_flight_control_voter_step(&controller, &input);

    input = make_input(true, false, true, true, true, false, false, false,
                       4310u, 4330u, 4910u);
    output = triplex_flight_control_voter_step(&controller, &input);

    assert(output.state == TRIPLEX_STATE_DEGRADED);
    assert(output.command == TRIPLEX_COMMAND_ISOLATE_LANE);
    assert(output.fault == TRIPLEX_FAULT_LANE_C_DRIFT);
    assert(output.isolated_lane == TRIPLEX_LANE_C);
    assert(output.active_mask == (TRIPLEX_MASK_A | TRIPLEX_MASK_B));
}

static void test_degraded_pair_can_continue(void) {
    triplex_flight_control_voter_t controller;
    triplex_input_t input;
    triplex_output_t output;

    triplex_flight_control_voter_init(&controller);
    input = make_input(true, false, true, true, true, false, false, false,
                       4200u, 4210u, 4190u);
    (void)triplex_flight_control_voter_step(&controller, &input);
    input = make_input(true, false, true, true, true, false, false, false,
                       4310u, 4330u, 4910u);
    (void)triplex_flight_control_voter_step(&controller, &input);

    input = make_input(true, false, true, true, true, false, false, false,
                       4340u, 4360u, 4950u);
    output = triplex_flight_control_voter_step(&controller, &input);

    assert(output.state == TRIPLEX_STATE_DEGRADED);
    assert(output.command == TRIPLEX_COMMAND_VOTE_OUTPUT);
    assert(output.fault == TRIPLEX_FAULT_NONE);
    assert(output.voted_command_bp == 4350u);
}

static void test_degraded_disagreement_faults(void) {
    triplex_flight_control_voter_t controller;
    triplex_input_t input;
    triplex_output_t output;

    triplex_flight_control_voter_init(&controller);
    input = make_input(true, false, true, true, true, false, false, false,
                       4300u, 4310u, 4290u);
    (void)triplex_flight_control_voter_step(&controller, &input);
    input = make_input(true, false, true, true, true, false, false, false,
                       4320u, 4330u, 4920u);
    (void)triplex_flight_control_voter_step(&controller, &input);

    input = make_input(true, false, true, true, true, false, false, false,
                       4010u, 4610u, 5000u);
    output = triplex_flight_control_voter_step(&controller, &input);

    assert(output.state == TRIPLEX_STATE_FAULT);
    assert(output.fault == TRIPLEX_FAULT_MULTI_LANE_FAIL);
}

static void test_timeout_isolates_single_lane(void) {
    triplex_flight_control_voter_t controller;
    triplex_input_t input;
    triplex_output_t output;

    triplex_flight_control_voter_init(&controller);
    input = make_input(true, false, true, true, true, false, false, false,
                       4100u, 4090u, 4120u);
    (void)triplex_flight_control_voter_step(&controller, &input);

    input = make_input(true, false, true, true, true, false, true, false,
                       4100u, 4090u, 4120u);
    output = triplex_flight_control_voter_step(&controller, &input);

    assert(output.state == TRIPLEX_STATE_DEGRADED);
    assert(output.command == TRIPLEX_COMMAND_ISOLATE_LANE);
    assert(output.fault == TRIPLEX_FAULT_LANE_TIMEOUT);
    assert(output.isolated_lane == TRIPLEX_LANE_B);
}

static void test_reset_clears_fault_when_disabled(void) {
    triplex_flight_control_voter_t controller;
    triplex_input_t input;
    triplex_output_t output;

    triplex_flight_control_voter_init(&controller);
    input = make_input(true, false, true, true, true, false, false, false,
                       4300u, 4310u, 4290u);
    (void)triplex_flight_control_voter_step(&controller, &input);
    input = make_input(true, false, true, true, true, false, false, false,
                       4320u, 4330u, 4920u);
    (void)triplex_flight_control_voter_step(&controller, &input);
    input = make_input(true, false, true, true, true, false, false, false,
                       4010u, 4610u, 5000u);
    (void)triplex_flight_control_voter_step(&controller, &input);

    input = make_input(false, true, true, true, true, false, false, false,
                       0u, 0u, 0u);
    output = triplex_flight_control_voter_step(&controller, &input);

    assert(output.state == TRIPLEX_STATE_IDLE);
    assert(output.command == TRIPLEX_COMMAND_RESET_VOTER);
    assert(output.fault == TRIPLEX_FAULT_NONE);
}

int main(void) {
    test_idle_holds_last_good_when_disabled();
    test_triplex_sync_votes_average();
    test_lane_c_drift_isolated();
    test_degraded_pair_can_continue();
    test_degraded_disagreement_faults();
    test_timeout_isolates_single_lane();
    test_reset_clears_fault_when_disabled();
    return 0;
}
