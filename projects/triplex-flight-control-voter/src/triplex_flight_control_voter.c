#include "triplex_flight_control_voter.h"

#include "lane_health.h"
#include "vote_math.h"

static triplex_fault_t drift_fault_for_lane(triplex_lane_t lane) {
    switch (lane) {
        case TRIPLEX_LANE_A:
            return TRIPLEX_FAULT_LANE_A_DRIFT;
        case TRIPLEX_LANE_B:
            return TRIPLEX_FAULT_LANE_B_DRIFT;
        case TRIPLEX_LANE_C:
            return TRIPLEX_FAULT_LANE_C_DRIFT;
        case TRIPLEX_LANE_NONE:
        default:
            return TRIPLEX_FAULT_NONE;
    }
}

static triplex_output_t make_output(
    const triplex_flight_control_voter_t *controller,
    const triplex_input_t *input,
    triplex_command_t command,
    triplex_fault_t fault,
    unsigned voted_command_bp,
    unsigned spread_bp) {
    triplex_output_t output;

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.active_mask = controller->state == TRIPLEX_STATE_IDLE
                             ? 0u
                             : lane_health_active_mask(input,
                                                       controller->isolated_lane);
    output.isolated_lane = controller->isolated_lane;
    output.voted_command_bp = voted_command_bp;
    output.spread_bp = spread_bp;
    output.servo_enabled =
        (controller->state == TRIPLEX_STATE_SYNC) ||
        (controller->state == TRIPLEX_STATE_DEGRADED);
    return output;
}

static triplex_output_t enter_fault(
    triplex_flight_control_voter_t *controller,
    const triplex_input_t *input,
    triplex_fault_t fault,
    unsigned spread_bp) {
    controller->state = TRIPLEX_STATE_FAULT;
    controller->latched_fault = fault;
    return make_output(controller, input, TRIPLEX_COMMAND_LATCH_FAULT, fault,
                       controller->last_voted_command_bp, spread_bp);
}

void triplex_flight_control_voter_init(
    triplex_flight_control_voter_t *controller) {
    controller->state = TRIPLEX_STATE_IDLE;
    controller->latched_fault = TRIPLEX_FAULT_NONE;
    controller->isolated_lane = TRIPLEX_LANE_NONE;
    controller->last_voted_command_bp = 0u;
}

triplex_output_t triplex_flight_control_voter_step(
    triplex_flight_control_voter_t *controller,
    const triplex_input_t *input) {
    unsigned active_mask;
    unsigned timeout_mask;
    triplex_vote_result_t vote;
    triplex_lane_t lane;

    active_mask = lane_health_active_mask(input, controller->isolated_lane);
    timeout_mask = lane_health_timeout_mask(input, controller->isolated_lane);

    if (controller->state == TRIPLEX_STATE_FAULT) {
        if (input->reset_request && !input->enable_request) {
            controller->state = TRIPLEX_STATE_IDLE;
            controller->latched_fault = TRIPLEX_FAULT_NONE;
            controller->isolated_lane = TRIPLEX_LANE_NONE;
            controller->last_voted_command_bp = 0u;
            return make_output(controller, input, TRIPLEX_COMMAND_RESET_VOTER,
                               TRIPLEX_FAULT_NONE, 0u, 0u);
        }

        return make_output(controller, input, TRIPLEX_COMMAND_LATCH_FAULT,
                           controller->latched_fault,
                           controller->last_voted_command_bp,
                           lane_health_spread_bp(input, active_mask));
    }

    if (!input->enable_request) {
        controller->state = TRIPLEX_STATE_IDLE;
        controller->isolated_lane = TRIPLEX_LANE_NONE;
        controller->last_voted_command_bp = 0u;
        return make_output(controller, input, TRIPLEX_COMMAND_HOLD_LAST_GOOD,
                           TRIPLEX_FAULT_NONE, 0u, 0u);
    }

    if (timeout_mask != 0u) {
        if ((lane_health_count(timeout_mask) == 1u) &&
            (lane_health_count(active_mask) >= 2u)) {
            controller->state = TRIPLEX_STATE_DEGRADED;
            controller->isolated_lane = lane_health_first_lane(timeout_mask);
            vote = vote_math_evaluate(input, active_mask);
            if ((vote.status == TRIPLEX_VOTE_SYNC_OK) ||
                (vote.status == TRIPLEX_VOTE_DEGRADED_OK)) {
                controller->last_voted_command_bp = vote.voted_command_bp;
                return make_output(controller, input,
                                   TRIPLEX_COMMAND_ISOLATE_LANE,
                                   TRIPLEX_FAULT_LANE_TIMEOUT,
                                   vote.voted_command_bp, vote.spread_bp);
            }

            return enter_fault(controller, input, TRIPLEX_FAULT_MULTI_LANE_FAIL,
                               vote.spread_bp);
        }

        return enter_fault(controller, input, TRIPLEX_FAULT_LANE_TIMEOUT,
                           lane_health_spread_bp(input, active_mask));
    }

    vote = vote_math_evaluate(input, active_mask);
    switch (vote.status) {
        case TRIPLEX_VOTE_SYNC_OK:
            controller->state =
                (lane_health_count(active_mask) == 3u &&
                 controller->isolated_lane == TRIPLEX_LANE_NONE)
                    ? TRIPLEX_STATE_SYNC
                    : TRIPLEX_STATE_DEGRADED;
            if ((lane_health_count(active_mask) == 2u) &&
                (controller->isolated_lane == TRIPLEX_LANE_NONE)) {
                controller->isolated_lane =
                    lane_health_first_lane(TRIPLEX_MASK_ALL & ~active_mask);
            }
            controller->last_voted_command_bp = vote.voted_command_bp;
            return make_output(controller, input, TRIPLEX_COMMAND_VOTE_OUTPUT,
                               TRIPLEX_FAULT_NONE, vote.voted_command_bp,
                               vote.spread_bp);

        case TRIPLEX_VOTE_DEGRADED_OK:
            controller->state = TRIPLEX_STATE_DEGRADED;
            if (controller->isolated_lane == TRIPLEX_LANE_NONE) {
                controller->isolated_lane =
                    lane_health_first_lane(TRIPLEX_MASK_ALL & ~active_mask);
            }
            controller->last_voted_command_bp = vote.voted_command_bp;
            return make_output(controller, input, TRIPLEX_COMMAND_VOTE_OUTPUT,
                               TRIPLEX_FAULT_NONE, vote.voted_command_bp,
                               vote.spread_bp);

        case TRIPLEX_VOTE_ISOLATE_A:
            lane = TRIPLEX_LANE_A;
            break;

        case TRIPLEX_VOTE_ISOLATE_B:
            lane = TRIPLEX_LANE_B;
            break;

        case TRIPLEX_VOTE_ISOLATE_C:
            lane = TRIPLEX_LANE_C;
            break;

        case TRIPLEX_VOTE_NO_MAJORITY:
            return enter_fault(controller, input, TRIPLEX_FAULT_SPLIT_VOTE,
                               vote.spread_bp);

        case TRIPLEX_VOTE_DUAL_DISAGREE:
        case TRIPLEX_VOTE_INSUFFICIENT:
        default:
            return enter_fault(controller, input, TRIPLEX_FAULT_MULTI_LANE_FAIL,
                               vote.spread_bp);
    }

    controller->state = TRIPLEX_STATE_DEGRADED;
    controller->isolated_lane = lane;
    controller->last_voted_command_bp = vote.voted_command_bp;
    return make_output(controller, input, TRIPLEX_COMMAND_ISOLATE_LANE,
                       drift_fault_for_lane(lane), vote.voted_command_bp,
                       vote.spread_bp);
}
