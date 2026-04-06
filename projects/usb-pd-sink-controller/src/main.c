#include <stdio.h>

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

static pd_source_caps_t high_current_caps(void) {
    pd_source_caps_t caps = {0};

    caps.count = 4u;
    caps.pdos[0].voltage_mv = 5000u;
    caps.pdos[0].max_current_ma = 3000u;
    caps.pdos[1].voltage_mv = 9000u;
    caps.pdos[1].max_current_ma = 3000u;
    caps.pdos[2].voltage_mv = 15000u;
    caps.pdos[2].max_current_ma = 3000u;
    caps.pdos[3].voltage_mv = 20000u;
    caps.pdos[3].max_current_ma = 3000u;
    return caps;
}

static pd_observation_t base_observation(pd_source_caps_t caps) {
    pd_observation_t observation;

    observation.attached = true;
    observation.board_temp_c = 52;
    observation.measured_bus_mv = 5000u;
    observation.desired_power_mw = 38000u;
    observation.min_voltage_mv = 5000u;
    observation.max_voltage_mv = 20000u;
    observation.preferred_voltage_mv = 20000u;
    observation.cable_limit_ma = 3000u;
    observation.request_reply = PD_REPLY_NONE;
    observation.source_caps = caps;
    return observation;
}

static void print_snapshot(const char *phase, const sink_policy_output_t *output) {
    char requested_buffer[32];
    char active_buffer[32];
    char fault_buffer[64];
    pd_contract_t displayed_contract =
        output->active.valid ? output->active : output->requested;

    pd_contract_to_string(&output->requested, requested_buffer,
                          sizeof(requested_buffer));
    pd_contract_to_string(&output->active, active_buffer, sizeof(active_buffer));
    pd_faults_to_string(output->faults, fault_buffer, sizeof(fault_buffer));

    printf(
        "phase=%s state=%s req=%s active=%s power=%.1fW retries=%u faults=%s\n",
        phase, pd_state_name(output->state), requested_buffer, active_buffer,
        (double)displayed_contract.power_mw / 1000.0, output->retries,
        fault_buffer);
}

int main(void) {
    sink_policy_t policy;
    pd_observation_t observation;
    sink_policy_output_t output;
    int retry;

    sink_policy_init(&policy);
    observation = base_observation(laptop_caps());
    output = sink_policy_step(&policy, &observation);
    print_snapshot("attach_request", &output);

    observation.request_reply = PD_REPLY_ACCEPT;
    observation.measured_bus_mv = 20000u;
    output = sink_policy_step(&policy, &observation);
    print_snapshot("ready", &output);

    observation.board_temp_c = 96;
    observation.request_reply = PD_REPLY_NONE;
    observation.measured_bus_mv = 20000u;
    output = sink_policy_step(&policy, &observation);
    observation.request_reply = PD_REPLY_ACCEPT;
    output = sink_policy_step(&policy, &observation);
    observation.request_reply = PD_REPLY_NONE;
    observation.measured_bus_mv = 9000u;
    output = sink_policy_step(&policy, &observation);
    print_snapshot("thermal_derate", &output);

    sink_policy_init(&policy);
    observation = base_observation(high_current_caps());
    observation.desired_power_mw = 30000u;
    observation.cable_limit_ma = 1500u;
    output = sink_policy_step(&policy, &observation);
    observation.request_reply = PD_REPLY_ACCEPT;
    observation.measured_bus_mv = 20000u;
    output = sink_policy_step(&policy, &observation);
    print_snapshot("thin_cable", &output);

    observation.request_reply = PD_REPLY_NONE;
    observation.measured_bus_mv = 18400u;
    output = sink_policy_step(&policy, &observation);
    print_snapshot("brownout", &output);

    sink_policy_init(&policy);
    observation = base_observation(laptop_caps());
    output = sink_policy_step(&policy, &observation);
    for (retry = 0; retry < 3; ++retry) {
        observation.request_reply = PD_REPLY_REJECT;
        output = sink_policy_step(&policy, &observation);
    }
    print_snapshot("reject_loop", &output);

    return 0;
}
