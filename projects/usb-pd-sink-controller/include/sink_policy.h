#ifndef SINK_POLICY_H
#define SINK_POLICY_H

#include <stdbool.h>
#include <stdint.h>

#include "pd_types.h"

typedef struct {
    bool attached;
    int board_temp_c;
    uint16_t measured_bus_mv;
    uint32_t desired_power_mw;
    uint16_t min_voltage_mv;
    uint16_t max_voltage_mv;
    uint16_t preferred_voltage_mv;
    uint16_t cable_limit_ma;
    pd_request_reply_t request_reply;
    pd_source_caps_t source_caps;
} pd_observation_t;

typedef struct {
    pd_state_t state;
    uint32_t faults;
    uint8_t retries;
    uint8_t attempt_rank;
    pd_contract_t pending;
    pd_contract_t active;
} sink_policy_t;

typedef struct {
    pd_state_t state;
    pd_contract_t requested;
    pd_contract_t active;
    uint32_t faults;
    uint8_t retries;
} sink_policy_output_t;

void sink_policy_init(sink_policy_t *policy);
sink_policy_output_t sink_policy_step(sink_policy_t *policy,
                                      const pd_observation_t *observation);

#endif
