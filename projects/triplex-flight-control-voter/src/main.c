#include <stdio.h>

#include "triplex_flight_control_voter.h"

static const char *on_off(bool value) {
    return value ? "ON" : "OFF";
}

static double bp_to_pct(unsigned value) {
    return (double)value / 100.0;
}

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

static void print_phase(const char *phase, const triplex_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s fault=%s active=%s isolated=%s "
        "voted=%.1f%% spread=%.1f%% servo=%s\n",
        phase, triplex_state_name(output->state),
        triplex_command_name(output->command),
        triplex_fault_name(output->fault),
        triplex_active_mask_name(output->active_mask),
        triplex_lane_name(output->isolated_lane),
        bp_to_pct(output->voted_command_bp), bp_to_pct(output->spread_bp),
        on_off(output->servo_enabled));
}

int main(void) {
    triplex_flight_control_voter_t controller;
    triplex_flight_control_voter_t fault_controller;
    triplex_flight_control_voter_t timeout_controller;
    triplex_input_t input;
    triplex_output_t output;

    triplex_flight_control_voter_init(&controller);
    input = make_input(false, false, true, true, true, false, false, false,
                       0u, 0u, 0u);
    output = triplex_flight_control_voter_step(&controller, &input);
    print_phase("standby", &output);

    input = make_input(true, false, true, true, true, false, false, false,
                       4185u, 4200u, 4215u);
    output = triplex_flight_control_voter_step(&controller, &input);
    print_phase("triplex_sync", &output);

    input = make_input(true, false, true, true, true, false, false, false,
                       4310u, 4330u, 4910u);
    output = triplex_flight_control_voter_step(&controller, &input);
    print_phase("lane_c_isolated", &output);

    input = make_input(true, false, true, true, true, false, false, false,
                       4345u, 4355u, 4950u);
    output = triplex_flight_control_voter_step(&controller, &input);
    print_phase("degraded_hold", &output);

    triplex_flight_control_voter_init(&fault_controller);
    input = make_input(true, false, true, true, true, false, false, false,
                       4300u, 4310u, 4290u);
    (void)triplex_flight_control_voter_step(&fault_controller, &input);
    input = make_input(true, false, true, true, true, false, false, false,
                       4320u, 4330u, 4920u);
    (void)triplex_flight_control_voter_step(&fault_controller, &input);
    input = make_input(true, false, true, true, true, false, false, false,
                       4345u, 4355u, 4960u);
    (void)triplex_flight_control_voter_step(&fault_controller, &input);
    input = make_input(true, false, true, true, true, false, false, false,
                       4050u, 4650u, 5000u);
    output = triplex_flight_control_voter_step(&fault_controller, &input);
    print_phase("split_vote_fault", &output);

    triplex_flight_control_voter_init(&timeout_controller);
    input = make_input(true, false, true, true, true, false, false, false,
                       4100u, 4090u, 4120u);
    (void)triplex_flight_control_voter_step(&timeout_controller, &input);
    input = make_input(true, false, true, true, true, false, true, false,
                       4100u, 4090u, 4120u);
    output = triplex_flight_control_voter_step(&timeout_controller, &input);
    print_phase("timeout_fault", &output);

    input = make_input(false, true, true, true, true, false, false, false,
                       0u, 0u, 0u);
    output = triplex_flight_control_voter_step(&fault_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
