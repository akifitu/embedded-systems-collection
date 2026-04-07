#include "battery_guard.h"

#define EPS_UNDERVOLTAGE_MV 6600u
#define EPS_DEEP_DISCHARGE_SOC 8u
#define EPS_SURVIVAL_SOC 18u
#define EPS_SURVIVAL_BUS_MV 7000u
#define EPS_SHED_SOC 45u
#define EPS_RECOVERY_SOC 30u
#define EPS_RECOVERY_BUS_MV 7350u
#define EPS_RECOVERY_PANEL_W 35u
#define EPS_NOMINAL_SOC 55u
#define EPS_NOMINAL_BUS_MV 7550u
#define EPS_NOMINAL_PANEL_W 50u

eps_fault_t battery_guard_fault(const eps_input_t *input) {
    if (input->bus_mv < EPS_UNDERVOLTAGE_MV) {
        return EPS_FAULT_BUS_UNDERVOLTAGE;
    }
    if (input->battery_soc_pct <= EPS_DEEP_DISCHARGE_SOC) {
        return EPS_FAULT_BATTERY_DEEP_DISCHARGE;
    }
    return EPS_FAULT_NONE;
}

bool battery_guard_should_shed(const eps_input_t *input) {
    if (input->battery_soc_pct < EPS_SHED_SOC) {
        return true;
    }
    if (!input->in_sunlight && (input->payload_request_w > 0u)) {
        return true;
    }
    if (input->panel_power_w + 5u < input->payload_request_w) {
        return true;
    }
    return false;
}

bool battery_guard_should_survive(const eps_input_t *input) {
    if (input->battery_soc_pct < EPS_SURVIVAL_SOC) {
        return true;
    }
    if (input->bus_mv < EPS_SURVIVAL_BUS_MV) {
        return true;
    }
    if (!input->in_sunlight && (input->panel_power_w < 10u) &&
        (input->battery_soc_pct < 25u)) {
        return true;
    }
    return false;
}

bool battery_guard_can_recover(const eps_input_t *input) {
    return input->in_sunlight && (input->battery_soc_pct >= EPS_RECOVERY_SOC) &&
           (input->bus_mv >= EPS_RECOVERY_BUS_MV) &&
           (input->panel_power_w >= EPS_RECOVERY_PANEL_W);
}

bool battery_guard_can_nominal(const eps_input_t *input) {
    return input->in_sunlight && (input->battery_soc_pct >= EPS_NOMINAL_SOC) &&
           (input->bus_mv >= EPS_NOMINAL_BUS_MV) &&
           (input->panel_power_w >= EPS_NOMINAL_PANEL_W);
}
