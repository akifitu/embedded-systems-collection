#include <assert.h>

#include "ventilator_controller.h"

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

static void test_standby_holds_when_disabled(void) {
    ventilator_controller_t controller;
    vent_output_t output;
    vent_input_t input;

    ventilator_controller_init(&controller);
    input = make_input(false, false, true, true, false, 5u, 12u);
    output = ventilator_controller_step(&controller, &input);

    assert(output.state == VENT_STATE_STANDBY);
    assert(output.command == VENT_COMMAND_HOLD_STANDBY);
    assert(output.expiratory_valve_open);
}

static void test_patient_trigger_starts_inhale(void) {
    ventilator_controller_t controller;
    vent_output_t output;
    vent_input_t input;

    ventilator_controller_init(&controller);
    input = make_input(true, true, true, true, false, 12u, 36u);
    output = ventilator_controller_step(&controller, &input);

    assert(output.state == VENT_STATE_INHALE);
    assert(output.command == VENT_COMMAND_DELIVER_INHALE);
    assert(output.inspiratory_valve_open);
}

static void test_inhale_reaches_hold_then_exhale(void) {
    ventilator_controller_t controller;
    vent_output_t output;
    vent_input_t input;

    ventilator_controller_init(&controller);
    input = make_input(true, true, true, true, false, 12u, 36u);
    (void)ventilator_controller_step(&controller, &input);

    input = make_input(true, false, true, true, false, 18u, 4u);
    output = ventilator_controller_step(&controller, &input);
    assert(output.state == VENT_STATE_HOLD);
    assert(output.command == VENT_COMMAND_HOLD_PLATEAU);

    input = make_input(true, false, true, true, false, 8u, 24u);
    output = ventilator_controller_step(&controller, &input);
    assert(output.state == VENT_STATE_EXHALE);
    assert(output.command == VENT_COMMAND_VENT_EXHALE);
}

static void test_apnea_triggers_backup_breath(void) {
    ventilator_controller_t controller;
    vent_output_t output;
    vent_input_t input;

    ventilator_controller_init(&controller);
    input = make_input(true, true, true, true, false, 12u, 36u);
    (void)ventilator_controller_step(&controller, &input);
    input = make_input(true, false, true, true, false, 18u, 4u);
    (void)ventilator_controller_step(&controller, &input);
    input = make_input(true, false, true, true, false, 8u, 24u);
    (void)ventilator_controller_step(&controller, &input);

    input = make_input(true, false, true, true, false, 10u, 32u);
    output = ventilator_controller_step(&controller, &input);

    assert(output.state == VENT_STATE_BACKUP);
    assert(output.command == VENT_COMMAND_START_BACKUP);
    assert(output.backup_active);
}

static void test_high_pressure_latches_alarm(void) {
    ventilator_controller_t controller;
    vent_output_t output;
    vent_input_t input;

    ventilator_controller_init(&controller);
    input = make_input(true, true, true, true, false, 12u, 36u);
    (void)ventilator_controller_step(&controller, &input);
    input = make_input(true, false, true, true, false, 36u, 8u);
    output = ventilator_controller_step(&controller, &input);

    assert(output.state == VENT_STATE_ALARM);
    assert(output.fault == VENT_FAULT_HIGH_PRESSURE);
}

static void test_disconnect_alarm_in_exhale(void) {
    ventilator_controller_t controller;
    vent_output_t output;
    vent_input_t input;

    ventilator_controller_init(&controller);
    input = make_input(true, true, true, true, false, 12u, 36u);
    (void)ventilator_controller_step(&controller, &input);
    input = make_input(true, false, true, true, false, 18u, 4u);
    (void)ventilator_controller_step(&controller, &input);
    input = make_input(true, false, true, true, false, 8u, 24u);
    (void)ventilator_controller_step(&controller, &input);
    input = make_input(true, false, true, true, false, 3u, 0u);
    output = ventilator_controller_step(&controller, &input);

    assert(output.state == VENT_STATE_ALARM);
    assert(output.fault == VENT_FAULT_DISCONNECT);
}

static void test_gas_loss_and_sensor_fault_alarm(void) {
    ventilator_controller_t controller;
    vent_output_t output;
    vent_input_t input;

    ventilator_controller_init(&controller);
    input = make_input(true, false, false, true, false, 5u, 10u);
    output = ventilator_controller_step(&controller, &input);
    assert(output.fault == VENT_FAULT_GAS_SUPPLY_LOSS);

    ventilator_controller_init(&controller);
    input = make_input(true, false, true, false, false, 5u, 10u);
    output = ventilator_controller_step(&controller, &input);
    assert(output.fault == VENT_FAULT_SENSOR_FAULT);
}

static void test_reset_clears_alarm_when_safe(void) {
    ventilator_controller_t controller;
    vent_output_t output;
    vent_input_t input;

    ventilator_controller_init(&controller);
    input = make_input(true, true, true, true, false, 12u, 36u);
    (void)ventilator_controller_step(&controller, &input);
    input = make_input(true, false, true, true, false, 36u, 8u);
    (void)ventilator_controller_step(&controller, &input);

    input = make_input(false, false, true, true, true, 5u, 10u);
    output = ventilator_controller_step(&controller, &input);

    assert(output.state == VENT_STATE_STANDBY);
    assert(output.command == VENT_COMMAND_RESET_VENT);
    assert(output.fault == VENT_FAULT_NONE);
}

int main(void) {
    test_standby_holds_when_disabled();
    test_patient_trigger_starts_inhale();
    test_inhale_reaches_hold_then_exhale();
    test_apnea_triggers_backup_breath();
    test_high_pressure_latches_alarm();
    test_disconnect_alarm_in_exhale();
    test_gas_loss_and_sensor_fault_alarm();
    test_reset_clears_alarm_when_safe();
    return 0;
}
