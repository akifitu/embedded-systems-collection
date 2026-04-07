#include <stdio.h>

#include "gas_burner_controller.h"

static const char *on_off(bool value) {
    return value ? "ON" : "OFF";
}

static const char *open_closed(bool value) {
    return value ? "OPEN" : "CLOSED";
}

static const char *yes_no(bool value) {
    return value ? "YES" : "NO";
}

static burner_input_t make_input(bool call_for_heat, bool airflow_proven,
                                 bool flame_present, bool limit_closed,
                                 bool reset_request) {
    burner_input_t input;

    input.call_for_heat = call_for_heat;
    input.airflow_proven = airflow_proven;
    input.flame_present = flame_present;
    input.limit_closed = limit_closed;
    input.reset_request = reset_request;
    return input;
}

static void print_phase(const char *phase, const burner_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s fault=%s fan=%s igniter=%s valve=%s "
        "flame=%s progress=%u\n",
        phase, burner_state_name(output->state),
        burner_command_name(output->command), burner_fault_name(output->fault),
        on_off(output->fan_active), on_off(output->igniter_active),
        open_closed(output->gas_valve_open), yes_no(output->flame_proven),
        output->progress_pct);
}

int main(void) {
    gas_burner_controller_t controller;
    gas_burner_controller_t fault_controller;
    burner_output_t output;
    burner_input_t input;

    gas_burner_controller_init(&controller);
    input = make_input(false, false, false, true, false);
    output = gas_burner_controller_step(&controller, &input);
    print_phase("idle", &output);

    input = make_input(true, true, false, true, false);
    output = gas_burner_controller_step(&controller, &input);
    print_phase("prepurge", &output);

    output = gas_burner_controller_step(&controller, &input);
    print_phase("ignition_trial", &output);

    input = make_input(true, true, true, true, false);
    output = gas_burner_controller_step(&controller, &input);
    print_phase("flame_proven", &output);

    input = make_input(false, true, false, true, false);
    output = gas_burner_controller_step(&controller, &input);
    print_phase("postpurge", &output);

    gas_burner_controller_init(&fault_controller);
    input = make_input(true, true, false, true, false);
    (void)gas_burner_controller_step(&fault_controller, &input);
    (void)gas_burner_controller_step(&fault_controller, &input);
    output = gas_burner_controller_step(&fault_controller, &input);
    print_phase("ignition_fail", &output);

    input = make_input(false, true, false, true, true);
    output = gas_burner_controller_step(&fault_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
