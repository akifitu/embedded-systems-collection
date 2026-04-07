#include "lane_health.h"

static unsigned command_for_lane(const triplex_input_t *input, unsigned mask) {
    if (mask == TRIPLEX_MASK_A) {
        return input->lane_a_command_bp;
    }
    if (mask == TRIPLEX_MASK_B) {
        return input->lane_b_command_bp;
    }
    return input->lane_c_command_bp;
}

unsigned lane_health_active_mask(const triplex_input_t *input,
                                 triplex_lane_t isolated_lane) {
    unsigned mask;

    mask = 0u;

    if (input->lane_a_valid && !input->lane_a_timeout &&
        (isolated_lane != TRIPLEX_LANE_A)) {
        mask |= TRIPLEX_MASK_A;
    }
    if (input->lane_b_valid && !input->lane_b_timeout &&
        (isolated_lane != TRIPLEX_LANE_B)) {
        mask |= TRIPLEX_MASK_B;
    }
    if (input->lane_c_valid && !input->lane_c_timeout &&
        (isolated_lane != TRIPLEX_LANE_C)) {
        mask |= TRIPLEX_MASK_C;
    }

    return mask;
}

unsigned lane_health_timeout_mask(const triplex_input_t *input,
                                  triplex_lane_t isolated_lane) {
    unsigned mask;

    mask = 0u;

    if (input->lane_a_timeout && (isolated_lane != TRIPLEX_LANE_A)) {
        mask |= TRIPLEX_MASK_A;
    }
    if (input->lane_b_timeout && (isolated_lane != TRIPLEX_LANE_B)) {
        mask |= TRIPLEX_MASK_B;
    }
    if (input->lane_c_timeout && (isolated_lane != TRIPLEX_LANE_C)) {
        mask |= TRIPLEX_MASK_C;
    }

    return mask;
}

unsigned lane_health_count(unsigned mask) {
    unsigned count;

    count = 0u;
    if ((mask & TRIPLEX_MASK_A) != 0u) {
        count++;
    }
    if ((mask & TRIPLEX_MASK_B) != 0u) {
        count++;
    }
    if ((mask & TRIPLEX_MASK_C) != 0u) {
        count++;
    }
    return count;
}

triplex_lane_t lane_health_first_lane(unsigned mask) {
    if ((mask & TRIPLEX_MASK_A) != 0u) {
        return TRIPLEX_LANE_A;
    }
    if ((mask & TRIPLEX_MASK_B) != 0u) {
        return TRIPLEX_LANE_B;
    }
    if ((mask & TRIPLEX_MASK_C) != 0u) {
        return TRIPLEX_LANE_C;
    }
    return TRIPLEX_LANE_NONE;
}

unsigned lane_health_spread_bp(const triplex_input_t *input, unsigned mask) {
    unsigned min_value;
    unsigned max_value;
    unsigned value;
    bool found;
    unsigned lane_masks[3];
    unsigned i;

    lane_masks[0] = TRIPLEX_MASK_A;
    lane_masks[1] = TRIPLEX_MASK_B;
    lane_masks[2] = TRIPLEX_MASK_C;

    found = false;
    min_value = 0u;
    max_value = 0u;

    for (i = 0u; i < 3u; ++i) {
        if ((mask & lane_masks[i]) == 0u) {
            continue;
        }

        value = command_for_lane(input, lane_masks[i]);
        if (!found) {
            min_value = value;
            max_value = value;
            found = true;
            continue;
        }

        if (value < min_value) {
            min_value = value;
        }
        if (value > max_value) {
            max_value = value;
        }
    }

    if (!found) {
        return 0u;
    }

    return max_value - min_value;
}
