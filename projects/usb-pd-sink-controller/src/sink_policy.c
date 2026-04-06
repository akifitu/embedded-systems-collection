#include "sink_policy.h"

#include "pdo_selector.h"

static uint16_t min_u16(uint16_t lhs, uint16_t rhs) {
    return lhs < rhs ? lhs : rhs;
}

static uint32_t min_u32(uint32_t lhs, uint32_t rhs) {
    return lhs < rhs ? lhs : rhs;
}

static void apply_fault(sink_policy_t *policy, uint32_t fault,
                        const pd_source_caps_t *caps) {
    policy->faults |= fault;
    policy->state = PD_STATE_FAULT;
    policy->attempt_rank = 0u;
    policy->pending = pdo_selector_safe_fallback(caps);
    policy->active = policy->pending;
}

static pd_selection_request_t derive_request(const pd_observation_t *observation,
                                             bool *derated) {
    pd_selection_request_t request;

    request.desired_power_mw = observation->desired_power_mw;
    request.min_voltage_mv = observation->min_voltage_mv;
    request.max_voltage_mv = observation->max_voltage_mv;
    request.preferred_voltage_mv = observation->preferred_voltage_mv;
    request.cable_limit_ma = observation->cable_limit_ma;

    *derated = false;
    if (observation->board_temp_c >= 90 && observation->board_temp_c < 105) {
        request.desired_power_mw = min_u32(request.desired_power_mw, 18000u);
        request.max_voltage_mv = min_u16(request.max_voltage_mv, 9000u);
        request.preferred_voltage_mv = min_u16(request.preferred_voltage_mv, 9000u);
        *derated = true;
    }

    return request;
}

static sink_policy_output_t compose_output(const sink_policy_t *policy) {
    sink_policy_output_t output;

    output.state = policy->state;
    output.requested = policy->pending;
    output.active = policy->active;
    output.faults = policy->faults;
    output.retries = policy->retries;
    return output;
}

void sink_policy_init(sink_policy_t *policy) {
    policy->state = PD_STATE_DETACHED;
    policy->faults = PD_FAULT_NONE;
    policy->retries = 0u;
    policy->attempt_rank = 0u;
    policy->pending.valid = false;
    policy->active.valid = false;
}

sink_policy_output_t sink_policy_step(sink_policy_t *policy,
                                      const pd_observation_t *observation) {
    bool derated = false;
    pd_selection_request_t request;
    pd_contract_t candidate;

    if (!observation->attached) {
        sink_policy_init(policy);
        return compose_output(policy);
    }

    if (observation->board_temp_c >= 105) {
        apply_fault(policy, PD_FAULT_OVERTEMP, &observation->source_caps);
        return compose_output(policy);
    }

    if (policy->active.valid &&
        observation->measured_bus_mv + 500u < policy->active.voltage_mv) {
        apply_fault(policy, PD_FAULT_BROWNOUT, &observation->source_caps);
        return compose_output(policy);
    }

    if (policy->faults != PD_FAULT_NONE) {
        policy->state = PD_STATE_FAULT;
        if (!policy->active.valid) {
            policy->active = pdo_selector_safe_fallback(&observation->source_caps);
            policy->pending = policy->active;
        }
        return compose_output(policy);
    }

    request = derive_request(observation, &derated);
    candidate = pdo_selector_choose(&observation->source_caps, &request,
                                    policy->attempt_rank);
    if (!candidate.valid) {
        apply_fault(policy, PD_FAULT_SOURCE_MISMATCH, &observation->source_caps);
        return compose_output(policy);
    }

    if (policy->state == PD_STATE_REQUESTING) {
        if (!pd_contract_equal(&policy->pending, &candidate)) {
            policy->pending = candidate;
        }
    } else if (!policy->active.valid ||
               !pd_contract_equal(&policy->active, &candidate)) {
        policy->pending = candidate;
        policy->state = PD_STATE_REQUESTING;
    } else {
        policy->pending = policy->active;
        policy->state = derated ? PD_STATE_DERATED : PD_STATE_READY;
        return compose_output(policy);
    }

    if (observation->request_reply == PD_REPLY_ACCEPT) {
        policy->active = policy->pending;
        policy->retries = 0u;
        policy->attempt_rank = 0u;
        policy->state = derated ? PD_STATE_DERATED : PD_STATE_READY;
    } else if (observation->request_reply == PD_REPLY_REJECT) {
        policy->retries++;
        policy->attempt_rank++;
        if (policy->retries >= 3u) {
            apply_fault(policy, PD_FAULT_RETRY_EXHAUSTED,
                        &observation->source_caps);
            return compose_output(policy);
        }

        candidate = pdo_selector_choose(&observation->source_caps, &request,
                                        policy->attempt_rank);
        if (!candidate.valid) {
            apply_fault(policy, PD_FAULT_RETRY_EXHAUSTED,
                        &observation->source_caps);
            return compose_output(policy);
        }
        policy->pending = candidate;
        policy->state = PD_STATE_REQUESTING;
    }

    return compose_output(policy);
}
