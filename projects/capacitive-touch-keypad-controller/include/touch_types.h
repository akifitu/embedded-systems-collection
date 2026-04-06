#ifndef TOUCH_TYPES_H
#define TOUCH_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define TOUCH_KEY_COUNT 4u

enum {
    TOUCH_KEY_UP = 1u << 0,
    TOUCH_KEY_DOWN = 1u << 1,
    TOUCH_KEY_OK = 1u << 2,
    TOUCH_KEY_BACK = 1u << 3
};

typedef enum {
    TOUCH_EVENT_NONE = 0,
    TOUCH_EVENT_TAP_UP,
    TOUCH_EVENT_TAP_DOWN,
    TOUCH_EVENT_TAP_OK,
    TOUCH_EVENT_TAP_BACK,
    TOUCH_EVENT_HOLD_BACK,
    TOUCH_EVENT_COMBO_UP_OK
} touch_event_t;

typedef enum {
    TOUCH_MOISTURE_DRY = 0,
    TOUCH_MOISTURE_WET
} touch_moisture_t;

typedef struct {
    uint16_t raw[TOUCH_KEY_COUNT];
    uint16_t shield_raw;
} touch_sample_t;

typedef struct {
    uint8_t active_mask;
    touch_event_t event;
    touch_moisture_t moisture;
    uint32_t faults;
    uint16_t baselines[TOUCH_KEY_COUNT];
} touch_output_t;

const char *touch_mask_name(uint8_t mask);
const char *touch_event_name(touch_event_t event);
const char *touch_moisture_name(touch_moisture_t moisture);
void touch_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len);

#endif
