#ifndef REFLOW_TYPES_H
#define REFLOW_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    REFLOW_STAGE_IDLE = 0,
    REFLOW_STAGE_PREHEAT,
    REFLOW_STAGE_SOAK,
    REFLOW_STAGE_REFLOW,
    REFLOW_STAGE_COOL,
    REFLOW_STAGE_COMPLETE,
    REFLOW_STAGE_FAULT
} reflow_stage_t;

enum {
    REFLOW_FAULT_NONE = 0u,
    REFLOW_FAULT_SENSOR_OPEN = 1u << 0,
    REFLOW_FAULT_OVERTEMP = 1u << 1,
    REFLOW_FAULT_RUNAWAY = 1u << 2,
    REFLOW_FAULT_DOOR_OPEN = 1u << 3
};

typedef struct {
    float measured_c;
    float ambient_c;
    bool sensor_valid;
    bool door_closed;
} oven_sample_t;

typedef struct {
    reflow_stage_t stage;
    float target_c;
    uint8_t heater_percent;
    uint8_t fan_percent;
    uint8_t progress_percent;
    uint32_t elapsed_s;
    uint32_t faults;
} oven_output_t;

const char *reflow_stage_name(reflow_stage_t stage);
void reflow_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len);

#endif
