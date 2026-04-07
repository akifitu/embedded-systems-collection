#ifndef LANE_HEALTH_H
#define LANE_HEALTH_H

#include <stdbool.h>

#include "triplex_types.h"

unsigned lane_health_active_mask(const triplex_input_t *input,
                                 triplex_lane_t isolated_lane);
unsigned lane_health_timeout_mask(const triplex_input_t *input,
                                  triplex_lane_t isolated_lane);
unsigned lane_health_count(unsigned mask);
triplex_lane_t lane_health_first_lane(unsigned mask);
unsigned lane_health_spread_bp(const triplex_input_t *input, unsigned mask);

#endif
