#include "pdo_selector.h"

#include <stddef.h>

typedef struct {
    pd_contract_t contract;
    unsigned long long score;
} scored_contract_t;

static uint16_t min_u16(uint16_t lhs, uint16_t rhs) {
    return lhs < rhs ? lhs : rhs;
}

static uint16_t abs_diff_u16(uint16_t lhs, uint16_t rhs) {
    return lhs > rhs ? (uint16_t)(lhs - rhs) : (uint16_t)(rhs - lhs);
}

static uint16_t current_for_power(uint32_t power_mw, uint16_t voltage_mv) {
    if (voltage_mv == 0u) {
        return 0u;
    }

    return (uint16_t)((power_mw * 1000u + voltage_mv - 1u) / voltage_mv);
}

static unsigned long long score_contract(const pd_contract_t *contract,
                                         const pd_selection_request_t *request) {
    uint32_t power_gap_mw = 0u;
    uint32_t excess_power_mw = 0u;
    uint16_t voltage_gap_mv =
        abs_diff_u16(contract->voltage_mv, request->preferred_voltage_mv);

    if (contract->power_mw < request->desired_power_mw) {
        power_gap_mw = request->desired_power_mw - contract->power_mw;
    } else {
        excess_power_mw = contract->power_mw - request->desired_power_mw;
    }

    return (unsigned long long)power_gap_mw * 1000ull +
           (unsigned long long)voltage_gap_mv * 10ull +
           (unsigned long long)contract->current_ma +
           (unsigned long long)(excess_power_mw / 100u);
}

static pd_contract_t invalid_contract(void) {
    pd_contract_t contract = {0};
    return contract;
}

pd_contract_t pdo_selector_choose(const pd_source_caps_t *caps,
                                  const pd_selection_request_t *request,
                                  uint8_t rank) {
    scored_contract_t candidates[PD_MAX_PDOS];
    uint8_t candidate_count = 0u;
    uint8_t i;

    for (i = 0u; i < caps->count && i < PD_MAX_PDOS; ++i) {
        pd_contract_t contract;
        uint16_t allowed_current_ma;
        uint16_t requested_current_ma;
        size_t insert_at;

        if (caps->pdos[i].voltage_mv < request->min_voltage_mv ||
            caps->pdos[i].voltage_mv > request->max_voltage_mv) {
            continue;
        }

        allowed_current_ma = caps->pdos[i].max_current_ma;
        if (request->cable_limit_ma != 0u) {
            allowed_current_ma = min_u16(allowed_current_ma,
                                         request->cable_limit_ma);
        }

        if (allowed_current_ma == 0u) {
            continue;
        }

        requested_current_ma =
            current_for_power(request->desired_power_mw, caps->pdos[i].voltage_mv);
        if (requested_current_ma == 0u) {
            requested_current_ma = allowed_current_ma;
        }
        requested_current_ma = min_u16(requested_current_ma, allowed_current_ma);

        contract.valid = true;
        contract.pdo_index = i;
        contract.voltage_mv = caps->pdos[i].voltage_mv;
        contract.current_ma = requested_current_ma;
        contract.power_mw =
            (uint32_t)contract.voltage_mv * contract.current_ma / 1000u;

        insert_at = candidate_count;
        while (insert_at > 0u &&
               score_contract(&contract, request) <
                   candidates[insert_at - 1u].score) {
            candidates[insert_at] = candidates[insert_at - 1u];
            --insert_at;
        }

        candidates[insert_at].contract = contract;
        candidates[insert_at].score = score_contract(&contract, request);
        candidate_count++;
    }

    if (rank >= candidate_count) {
        return invalid_contract();
    }

    return candidates[rank].contract;
}

pd_contract_t pdo_selector_safe_fallback(const pd_source_caps_t *caps) {
    uint8_t i;

    for (i = 0u; i < caps->count && i < PD_MAX_PDOS; ++i) {
        if (caps->pdos[i].voltage_mv == 5000u) {
            pd_contract_t contract;

            contract.valid = true;
            contract.pdo_index = i;
            contract.voltage_mv = 5000u;
            contract.current_ma = min_u16(caps->pdos[i].max_current_ma, 500u);
            contract.power_mw =
                (uint32_t)contract.voltage_mv * contract.current_ma / 1000u;
            return contract;
        }
    }

    return invalid_contract();
}
