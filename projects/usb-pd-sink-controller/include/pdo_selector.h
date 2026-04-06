#ifndef PDO_SELECTOR_H
#define PDO_SELECTOR_H

#include <stdint.h>

#include "pd_types.h"

typedef struct {
    uint32_t desired_power_mw;
    uint16_t min_voltage_mv;
    uint16_t max_voltage_mv;
    uint16_t preferred_voltage_mv;
    uint16_t cable_limit_ma;
} pd_selection_request_t;

pd_contract_t pdo_selector_choose(const pd_source_caps_t *caps,
                                  const pd_selection_request_t *request,
                                  uint8_t rank);
pd_contract_t pdo_selector_safe_fallback(const pd_source_caps_t *caps);

#endif
