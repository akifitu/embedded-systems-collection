#ifndef ABS_TYPES_H
#define ABS_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define ABS_WHEEL_COUNT 4u

typedef enum {
    ABS_STATE_STANDBY = 0,
    ABS_STATE_BRAKING,
    ABS_STATE_ABS_ACTIVE,
    ABS_STATE_FAULT
} abs_state_t;

typedef enum {
    ABS_VALVE_HOLD = 0,
    ABS_VALVE_APPLY,
    ABS_VALVE_RELEASE
} abs_valve_t;

typedef enum {
    ABS_FAULT_NONE = 0,
    ABS_FAULT_WHEEL_SENSOR
} abs_fault_t;

typedef struct {
    uint8_t brake_pct;
    uint16_t vehicle_speed_kph_x10;
    uint16_t wheel_speed_kph_x10[ABS_WHEEL_COUNT];
} abs_input_t;

typedef struct {
    abs_state_t state;
    abs_fault_t fault;
    bool pump_enabled;
    uint8_t slip_pct[ABS_WHEEL_COUNT];
    abs_valve_t valves[ABS_WHEEL_COUNT];
} abs_output_t;

const char *abs_state_name(abs_state_t state);
const char *abs_valve_name(abs_valve_t valve);
const char *abs_fault_name(abs_fault_t fault);

#endif
