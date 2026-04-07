#include <stdio.h>

#include "ventilator_controller.h"

static const char *open_closed(bool value) {
    return value ? "OPEN" : "CLOSED";
}

static const char *mode_name(bool backup_active) {
    return backup_active ? "BACKUP" : "SUPPORT";
}

static vent_input_t make_input(bool therapy_enabled, bool patient_trigger,
                               bool gas_supply_ok, bool pressure_sensor_ok,
                               bool reset_request,
                               unsigned airway_pressure_cmh2o,
                               unsigned flow_lpm) {
    vent_input_t input;

    input.therapy_enabled = therapy_enabled;
    input.patient_trigger = patient_trigger;
    input.gas_supply_ok = gas_supply_ok;
    input.pressure_sensor_ok = pressure_sensor_ok;
    input.reset_request = reset_request;
    input.airway_pressure_cmh2o = airway_pressure_cmh2o;
    input.flow_lpm = flow_lpm;
    return input;
}

static void print_phase(const char *phase, const vent_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s fault=%s mode=%s blower=%u in_valve=%s "
        "ex_valve=%s target=%u pressure=%u flow=%u\n",
        phase, vent_state_name(output->state),
        vent_command_name(output->command), vent_fault_name(output->fault),
        mode_name(output->backup_active), output->blower_pct,
        open_closed(output->inspiratory_valve_open),
        open_closed(output->expiratory_valve_open),
        output->target_pressure_cmh2o, output->airway_pressure_cmh2o,
        output->flow_lpm);
}

int main(void) {
    ventilator_controller_t controller;
    ventilator_controller_t fault_controller;
    vent_output_t output;
    vent_input_t input;

    ventilator_controller_init(&controller);
    input = make_input(false, false, true, true, false, 5u, 12u);
    output = ventilator_controller_step(&controller, &input);
    print_phase("standby", &output);

    input = make_input(true, true, true, true, false, 12u, 36u);
    output = ventilator_controller_step(&controller, &input);
    print_phase("patient_trigger", &output);

    input = make_input(true, false, true, true, false, 18u, 4u);
    output = ventilator_controller_step(&controller, &input);
    print_phase("plateau", &output);

    input = make_input(true, false, true, true, false, 8u, 24u);
    output = ventilator_controller_step(&controller, &input);
    print_phase("exhale", &output);

    input = make_input(true, false, true, true, false, 10u, 32u);
    output = ventilator_controller_step(&controller, &input);
    print_phase("backup_breath", &output);

    ventilator_controller_init(&fault_controller);
    input = make_input(true, true, true, true, false, 12u, 34u);
    (void)ventilator_controller_step(&fault_controller, &input);
    input = make_input(true, false, true, true, false, 18u, 4u);
    (void)ventilator_controller_step(&fault_controller, &input);
    input = make_input(true, false, true, true, false, 8u, 24u);
    (void)ventilator_controller_step(&fault_controller, &input);
    input = make_input(true, false, true, true, false, 3u, 0u);
    output = ventilator_controller_step(&fault_controller, &input);
    print_phase("disconnect_alarm", &output);

    input = make_input(false, false, true, true, true, 5u, 10u);
    output = ventilator_controller_step(&fault_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
