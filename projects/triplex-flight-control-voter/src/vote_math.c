#include "vote_math.h"

#include "lane_health.h"

#define TRIPLEX_VOTE_TOLERANCE_BP 40u

static unsigned command_for_lane(const triplex_input_t *input, unsigned mask) {
    if (mask == TRIPLEX_MASK_A) {
        return input->lane_a_command_bp;
    }
    if (mask == TRIPLEX_MASK_B) {
        return input->lane_b_command_bp;
    }
    return input->lane_c_command_bp;
}

static unsigned delta_bp(unsigned a, unsigned b) {
    return (a >= b) ? (a - b) : (b - a);
}

static bool within_tolerance(unsigned a, unsigned b) {
    return delta_bp(a, b) <= TRIPLEX_VOTE_TOLERANCE_BP;
}

static unsigned average2(unsigned a, unsigned b) {
    return (a + b) / 2u;
}

static unsigned average3(unsigned a, unsigned b, unsigned c) {
    return (a + b + c) / 3u;
}

triplex_vote_result_t vote_math_evaluate(const triplex_input_t *input,
                                         unsigned active_mask) {
    triplex_vote_result_t result;
    unsigned active_count;
    unsigned a;
    unsigned b;
    unsigned c;
    bool ab_ok;
    bool ac_ok;
    bool bc_ok;

    result.status = TRIPLEX_VOTE_INSUFFICIENT;
    result.voted_command_bp = 0u;
    result.spread_bp = lane_health_spread_bp(input, active_mask);

    active_count = lane_health_count(active_mask);
    if (active_count < 2u) {
        return result;
    }

    if (active_count == 2u) {
        if (active_mask == (TRIPLEX_MASK_A | TRIPLEX_MASK_B)) {
            a = input->lane_a_command_bp;
            b = input->lane_b_command_bp;
        } else if (active_mask == (TRIPLEX_MASK_A | TRIPLEX_MASK_C)) {
            a = input->lane_a_command_bp;
            b = input->lane_c_command_bp;
        } else {
            a = input->lane_b_command_bp;
            b = input->lane_c_command_bp;
        }

        if (within_tolerance(a, b)) {
            result.status = TRIPLEX_VOTE_DEGRADED_OK;
            result.voted_command_bp = average2(a, b);
        } else {
            result.status = TRIPLEX_VOTE_DUAL_DISAGREE;
        }
        return result;
    }

    a = command_for_lane(input, TRIPLEX_MASK_A);
    b = command_for_lane(input, TRIPLEX_MASK_B);
    c = command_for_lane(input, TRIPLEX_MASK_C);

    ab_ok = within_tolerance(a, b);
    ac_ok = within_tolerance(a, c);
    bc_ok = within_tolerance(b, c);

    if (ab_ok && ac_ok && bc_ok) {
        result.status = TRIPLEX_VOTE_SYNC_OK;
        result.voted_command_bp = average3(a, b, c);
        return result;
    }

    if (ab_ok && !ac_ok && !bc_ok) {
        result.status = TRIPLEX_VOTE_ISOLATE_C;
        result.voted_command_bp = average2(a, b);
        return result;
    }

    if (ac_ok && !ab_ok && !bc_ok) {
        result.status = TRIPLEX_VOTE_ISOLATE_B;
        result.voted_command_bp = average2(a, c);
        return result;
    }

    if (bc_ok && !ab_ok && !ac_ok) {
        result.status = TRIPLEX_VOTE_ISOLATE_A;
        result.voted_command_bp = average2(b, c);
        return result;
    }

    if (ab_ok || ac_ok || bc_ok) {
        result.status = TRIPLEX_VOTE_SYNC_OK;
        if (ab_ok && ac_ok) {
            result.voted_command_bp = a;
        } else if (ab_ok && bc_ok) {
            result.voted_command_bp = b;
        } else {
            result.voted_command_bp = c;
        }
        return result;
    }

    result.status = TRIPLEX_VOTE_NO_MAJORITY;
    return result;
}
