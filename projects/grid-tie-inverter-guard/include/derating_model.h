#ifndef DERATING_MODEL_H
#define DERATING_MODEL_H

#include <stdint.h>

uint8_t derating_model_limit_pct(int16_t heatsink_c_x10, uint16_t dc_link_v);

#endif
