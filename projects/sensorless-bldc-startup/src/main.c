#include <stdbool.h>
#include <stdio.h>

#include "startup_controller.h"

static void print_snapshot(const char *phase, const bldc_output_t *output) {
    char fault_buffer[64];

    bldc_faults_to_string(output->faults, fault_buffer, sizeof(fault_buffer));
    printf(
        "phase=%s state=%s sector=%s duty=%u period=%uus rpm=%u lock=%s "
        "faults=%s\n",
        phase, bldc_state_name(output->state), bldc_sector_name(output->sector),
        output->duty_percent, output->commutation_period_us,
        output->estimated_rpm, bldc_lock_name(output->lock), fault_buffer);
}

static bldc_output_t step_once(startup_controller_t *controller, bool start,
                               bool clear_fault, uint8_t throttle_percent,
                               bool zero_cross, uint16_t phase_current_ma,
                               uint16_t measured_rpm) {
    bldc_command_t command;
    bldc_observation_t observation;

    command.start = start;
    command.clear_fault = clear_fault;
    command.throttle_percent = throttle_percent;
    observation.zero_cross = zero_cross;
    observation.phase_current_ma = phase_current_ma;
    observation.measured_rpm = measured_rpm;

    return startup_controller_step(controller, &command, &observation);
}

static void run_nominal_sequence(void) {
    startup_controller_t controller;
    bldc_output_t output = {0};
    int tick;

    startup_controller_init(&controller);

    for (tick = 0; tick < 20; ++tick) {
        output = step_once(&controller, tick == 0, false, 34u, false, 2200u, 0u);
        if (output.state == BLDC_STATE_ALIGN && tick == 4) {
            print_snapshot("align", &output);
        }
        if (output.state == BLDC_STATE_OPEN_LOOP && output.sector == BLDC_SECTOR_S3 &&
            output.commutation_period_us == 2400u) {
            print_snapshot("ramp", &output);
            break;
        }
    }

    for (tick = 0; tick < 3; ++tick) {
        output = step_once(&controller, false, false, 34u, true, 2600u, 1860u);
    }
    print_snapshot("lock", &output);

    output = step_once(&controller, false, false, 46u, true, 4100u, 1410u);
    print_snapshot("load_step", &output);
}

static void run_overcurrent_sequence(void) {
    startup_controller_t controller;
    bldc_output_t output = {0};
    int tick;

    startup_controller_init(&controller);
    for (tick = 0; tick < 20; ++tick) {
        output = step_once(&controller, tick == 0, false, 34u, false, 2200u, 0u);
    }
    for (tick = 0; tick < 3; ++tick) {
        output = step_once(&controller, false, false, 34u, true, 2600u, 1860u);
    }

    output = step_once(&controller, false, false, 52u, true, 8400u, 1500u);
    print_snapshot("overcurrent", &output);
}

static void run_no_lock_sequence(void) {
    startup_controller_t controller;
    bldc_output_t output = {0};
    int tick;

    startup_controller_init(&controller);
    for (tick = 0; tick < 50; ++tick) {
        output = step_once(&controller, tick == 0, false, 34u, false, 2100u, 0u);
        if (output.state == BLDC_STATE_FAULT) {
            break;
        }
    }

    print_snapshot("no_lock", &output);
}

int main(void) {
    run_nominal_sequence();
    run_overcurrent_sequence();
    run_no_lock_sequence();
    return 0;
}
