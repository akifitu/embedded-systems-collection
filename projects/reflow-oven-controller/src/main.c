#include <stdbool.h>
#include <stdio.h>

#include "reflow_controller.h"

typedef struct {
    float temperature_c;
    float ambient_c;
    bool sensor_valid;
    bool door_closed;
} oven_plant_t;

static void plant_step(oven_plant_t *plant, const oven_output_t *output) {
    float heater_effect = 4.6f * ((float)output->heater_percent / 100.0f);
    float passive_cooling = 0.020f * (plant->temperature_c - plant->ambient_c);
    float fan_cooling = 1.55f * ((float)output->fan_percent / 100.0f);

    if (output->stage == REFLOW_STAGE_REFLOW && plant->temperature_c > 220.0f) {
        heater_effect *= 0.80f;
    }

    plant->temperature_c += heater_effect - passive_cooling - fan_cooling;
    if (plant->temperature_c < plant->ambient_c) {
        plant->temperature_c = plant->ambient_c;
    }
}

static void print_snapshot(const char *phase, const oven_output_t *output,
                           float temperature_c) {
    char fault_buffer[64];

    reflow_faults_to_string(output->faults, fault_buffer, sizeof(fault_buffer));
    printf(
        "phase=%s stage=%s target=%.1f temp=%.1f heater=%u fan=%u progress=%u "
        "faults=%s\n",
        phase, reflow_stage_name(output->stage), output->target_c, temperature_c,
        output->heater_percent, output->fan_percent, output->progress_percent,
        fault_buffer);
}

static void run_nominal_demo(void) {
    reflow_controller_t controller;
    oven_output_t output;
    oven_plant_t plant = {25.0f, 25.0f, true, true};
    bool printed_preheat = false;
    bool printed_soak = false;
    bool printed_reflow = false;
    bool printed_cool = false;
    bool printed_complete = false;
    int second;

    reflow_controller_init(&controller);

    for (second = 0; second < 500; ++second) {
        oven_sample_t sample;

        sample.measured_c = plant.temperature_c;
        sample.ambient_c = plant.ambient_c;
        sample.sensor_valid = plant.sensor_valid;
        sample.door_closed = plant.door_closed;

        output = reflow_controller_step(&controller, &sample);

        if (!printed_preheat && output.stage == REFLOW_STAGE_PREHEAT &&
            sample.measured_c >= 110.0f) {
            print_snapshot("preheat", &output, sample.measured_c);
            printed_preheat = true;
        }
        if (!printed_soak && output.stage == REFLOW_STAGE_SOAK &&
            sample.measured_c >= 175.0f) {
            print_snapshot("soak", &output, sample.measured_c);
            printed_soak = true;
        }
        if (!printed_reflow && output.stage == REFLOW_STAGE_REFLOW &&
            (sample.measured_c >= 220.0f || output.progress_percent == 65u)) {
            print_snapshot("reflow", &output, sample.measured_c);
            printed_reflow = true;
        }
        if (!printed_cool && output.stage == REFLOW_STAGE_COOL &&
            sample.measured_c <= 150.0f) {
            print_snapshot("cooldown", &output, sample.measured_c);
            printed_cool = true;
        }
        if (!printed_complete && output.stage == REFLOW_STAGE_COMPLETE) {
            print_snapshot("complete", &output, sample.measured_c);
            printed_complete = true;
            break;
        }
        plant_step(&plant, &output);
    }
}

static void run_fault_demo(void) {
    reflow_controller_t controller;
    oven_sample_t sample;
    oven_output_t output;

    reflow_controller_init(&controller);
    sample.measured_c = 178.0f;
    sample.ambient_c = 25.0f;
    sample.sensor_valid = false;
    sample.door_closed = true;

    output = reflow_controller_step(&controller, &sample);
    print_snapshot("sensor_fault", &output, sample.measured_c);
}

int main(void) {
    run_nominal_demo();
    run_fault_demo();
    return 0;
}
