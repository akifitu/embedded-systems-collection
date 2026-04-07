#ifndef INVERTER_GUARD_TYPES_H
#define INVERTER_GUARD_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    INVERTER_STATE_WAIT_GRID = 0,
    INVERTER_STATE_SYNC,
    INVERTER_STATE_EXPORT,
    INVERTER_STATE_TRIP,
    INVERTER_STATE_COOLDOWN
} inverter_state_t;

typedef enum {
    INVERTER_COMMAND_OPEN_RELAY = 0,
    INVERTER_COMMAND_TRACK_GRID,
    INVERTER_COMMAND_CLOSE_RELAY,
    INVERTER_COMMAND_EXPORT_POWER,
    INVERTER_COMMAND_HOLD_OFF
} inverter_command_t;

typedef enum {
    INVERTER_REASON_NONE = 0,
    INVERTER_REASON_GRID_LOSS,
    INVERTER_REASON_UNDERVOLTAGE,
    INVERTER_REASON_OVERVOLTAGE,
    INVERTER_REASON_UNDERFREQUENCY,
    INVERTER_REASON_OVERFREQUENCY,
    INVERTER_REASON_ROCOF,
    INVERTER_REASON_OVERTEMP,
    INVERTER_REASON_DC_LINK_HIGH,
    INVERTER_REASON_RELAY_FAULT,
    INVERTER_REASON_COOLDOWN
} inverter_reason_t;

typedef enum {
    INVERTER_QUALITY_SEARCH = 0,
    INVERTER_QUALITY_LOCKING,
    INVERTER_QUALITY_LOCKED,
    INVERTER_QUALITY_DERATED,
    INVERTER_QUALITY_FAULT,
    INVERTER_QUALITY_RECOVERING
} inverter_quality_t;

typedef struct {
    bool grid_present;
    uint16_t voltage_rms_v;
    uint32_t frequency_mhz;
    uint16_t rocof_mhz_s;
    int16_t heatsink_c_x10;
    uint16_t dc_link_v;
    bool relay_closed_fb;
} inverter_input_t;

typedef struct {
    inverter_state_t state;
    inverter_command_t command;
    inverter_reason_t reason;
    inverter_quality_t quality;
    uint8_t power_limit_pct;
    uint8_t sync_count;
    uint8_t cooldown_remaining;
    bool relay_closed_commanded;
} inverter_output_t;

const char *inverter_state_name(inverter_state_t state);
const char *inverter_command_name(inverter_command_t command);
const char *inverter_reason_name(inverter_reason_t reason);
const char *inverter_quality_name(inverter_quality_t quality);

#endif
