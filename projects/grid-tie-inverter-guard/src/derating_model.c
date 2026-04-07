#include "derating_model.h"

#define DERATE_START_C_X10 750
#define DERATE_END_C_X10 950

uint8_t derating_model_limit_pct(int16_t heatsink_c_x10, uint16_t dc_link_v) {
    uint8_t limit;

    if (heatsink_c_x10 <= DERATE_START_C_X10) {
        limit = 100u;
    } else if (heatsink_c_x10 >= DERATE_END_C_X10) {
        limit = 60u;
    } else {
        unsigned span;
        unsigned used;
        unsigned reduction;

        span = (unsigned)(DERATE_END_C_X10 - DERATE_START_C_X10);
        used = (unsigned)(heatsink_c_x10 - DERATE_START_C_X10);
        reduction = (used * 40u) / span;
        limit = (uint8_t)(100u - reduction);
    }

    if ((dc_link_v >= 420u) && (limit > 80u)) {
        limit = 80u;
    }
    return limit;
}
