#include "bms_port_app.h"

#include "bms_port_config.h"
#include "bms_port_hal.h"

static bms_controller_t g_bms_controller;

void bms_port_app_init(void) {
    bms_port_hal_init();
    bms_controller_init(&g_bms_controller, BMS_PORT_PACK_CAPACITY_AH,
                        BMS_PORT_INITIAL_SOC_PERCENT);
}

void bms_port_app_step(void) {
    bms_sample_t sample;
    bms_output_t output;

    if (!bms_port_hal_read_sample(&sample)) {
        bms_port_hal_log("sample read failed");
        return;
    }

    output = bms_controller_step(&g_bms_controller, &sample);
    bms_port_hal_apply_output(&output);
}

void bms_port_app_run_forever(void) {
    while (1) {
        uint32_t frame_start_ms = bms_port_hal_millis();
        bms_port_app_step();

        while ((bms_port_hal_millis() - frame_start_ms) <
               BMS_PORT_SAMPLE_PERIOD_MS) {
        }
    }
}

