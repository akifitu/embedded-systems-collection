#include <assert.h>
#include <stdio.h>

#include "pdo_selector.h"
#include "sink_policy.h"

static pd_source_caps_t laptop_caps(void) {
    pd_source_caps_t caps = {0};

    caps.count = 4u;
    caps.pdos[0].voltage_mv = 5000u;
    caps.pdos[0].max_current_ma = 3000u;
    caps.pdos[1].voltage_mv = 9000u;
    caps.pdos[1].max_current_ma = 3000u;
    caps.pdos[2].voltage_mv = 15000u;
    caps.pdos[2].max_current_ma = 3000u;
    caps.pdos[3].voltage_mv = 20000u;
    caps.pdos[3].max_current_ma = 2250u;
    return caps;
}

static pd_observation_t base_observation(void) {
    pd_observation_t observation;

    observation.attached = true;
    observation.board_temp_c = 50;
    observation.measured_bus_mv = 5000u;
    observation.desired_power_mw = 38000u;
    observation.min_voltage_mv = 5000u;
    observation.max_voltage_mv = 20000u;
    observation.preferred_voltage_mv = 20000u;
    observation.cable_limit_ma = 3000u;
    observation.request_reply = PD_REPLY_NONE;
    observation.source_caps = laptop_caps();
    return observation;
}

static void test_selector_prefers_20v_contract(void) {
    pd_observation_t observation;
    pd_selection_request_t request;
    pd_contract_t contract;

    observation = base_observation();
    request.desired_power_mw = 38000u;
    request.min_voltage_mv = 5000u;
    request.max_voltage_mv = 20000u;
    request.preferred_voltage_mv = 20000u;
    request.cable_limit_ma = 3000u;

    contract = pdo_selector_choose(&observation.source_caps, &request, 0u);
    assert(contract.valid);
    assert(contract.voltage_mv == 20000u);
    assert(contract.current_ma == 1900u);
    assert(contract.power_mw == 38000u);
}

static void test_thermal_derate_switches_to_9v(void) {
    sink_policy_t policy;
    pd_observation_t observation;
    sink_policy_output_t output;

    sink_policy_init(&policy);
    observation = base_observation();
    (void)sink_policy_step(&policy, &observation);
    observation.request_reply = PD_REPLY_ACCEPT;
    observation.measured_bus_mv = 20000u;
    output = sink_policy_step(&policy, &observation);
    assert(output.state == PD_STATE_READY);
    assert(output.active.voltage_mv == 20000u);

    observation.board_temp_c = 96;
    observation.request_reply = PD_REPLY_NONE;
    observation.measured_bus_mv = 20000u;
    (void)sink_policy_step(&policy, &observation);
    observation.request_reply = PD_REPLY_ACCEPT;
    output = sink_policy_step(&policy, &observation);
    observation.request_reply = PD_REPLY_NONE;
    observation.measured_bus_mv = 9000u;
    output = sink_policy_step(&policy, &observation);
    assert(output.state == PD_STATE_DERATED);
    assert(output.active.voltage_mv == 9000u);
    assert(output.active.current_ma == 2000u);
}

static void test_brownout_falls_back_to_safe_5v(void) {
    sink_policy_t policy;
    pd_observation_t observation;
    sink_policy_output_t output;

    sink_policy_init(&policy);
    observation = base_observation();
    (void)sink_policy_step(&policy, &observation);
    observation.request_reply = PD_REPLY_ACCEPT;
    observation.measured_bus_mv = 20000u;
    (void)sink_policy_step(&policy, &observation);

    observation.request_reply = PD_REPLY_NONE;
    observation.measured_bus_mv = 18400u;
    output = sink_policy_step(&policy, &observation);
    assert(output.state == PD_STATE_FAULT);
    assert((output.faults & PD_FAULT_BROWNOUT) != 0u);
    assert(output.active.voltage_mv == 5000u);
    assert(output.active.current_ma == 500u);
}

static void test_retry_exhaustion_sets_fault(void) {
    sink_policy_t policy;
    pd_observation_t observation;
    sink_policy_output_t output = {0};
    int retry;

    sink_policy_init(&policy);
    observation = base_observation();
    (void)sink_policy_step(&policy, &observation);

    for (retry = 0; retry < 3; ++retry) {
        observation.request_reply = PD_REPLY_REJECT;
        output = sink_policy_step(&policy, &observation);
    }

    assert(output.state == PD_STATE_FAULT);
    assert((output.faults & PD_FAULT_RETRY_EXHAUSTED) != 0u);
    assert(output.retries == 3u);
    assert(output.active.voltage_mv == 5000u);
    assert(output.active.current_ma == 500u);
}

static void test_source_mismatch_faults_when_only_5v_exists(void) {
    sink_policy_t policy;
    pd_observation_t observation;
    sink_policy_output_t output;

    sink_policy_init(&policy);
    observation = base_observation();
    observation.source_caps.count = 1u;
    observation.source_caps.pdos[0].voltage_mv = 5000u;
    observation.source_caps.pdos[0].max_current_ma = 3000u;
    observation.min_voltage_mv = 9000u;
    observation.preferred_voltage_mv = 9000u;
    output = sink_policy_step(&policy, &observation);

    assert(output.state == PD_STATE_FAULT);
    assert((output.faults & PD_FAULT_SOURCE_MISMATCH) != 0u);
    assert(output.active.voltage_mv == 5000u);
    assert(output.active.current_ma == 500u);
}

int main(void) {
    test_selector_prefers_20v_contract();
    test_thermal_derate_switches_to_9v();
    test_brownout_falls_back_to_safe_5v();
    test_retry_exhaustion_sets_fault();
    test_source_mismatch_faults_when_only_5v_exists();

    puts("usb-pd-sink-controller tests passed");
    return 0;
}
