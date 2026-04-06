#include "touch_types.h"

#include <stdio.h>
#include <string.h>

const char *touch_mask_name(uint8_t mask) {
    if (mask == 0u) {
        return "NONE";
    }
    if (mask == TOUCH_KEY_UP) {
        return "UP";
    }
    if (mask == TOUCH_KEY_DOWN) {
        return "DOWN";
    }
    if (mask == TOUCH_KEY_OK) {
        return "OK";
    }
    if (mask == TOUCH_KEY_BACK) {
        return "BACK";
    }
    if (mask == (TOUCH_KEY_UP | TOUCH_KEY_OK)) {
        return "UP_OK";
    }
    return "MULTI";
}

const char *touch_event_name(touch_event_t event) {
    switch (event) {
        case TOUCH_EVENT_NONE:
            return "NONE";
        case TOUCH_EVENT_TAP_UP:
            return "TAP_UP";
        case TOUCH_EVENT_TAP_DOWN:
            return "TAP_DOWN";
        case TOUCH_EVENT_TAP_OK:
            return "TAP_OK";
        case TOUCH_EVENT_TAP_BACK:
            return "TAP_BACK";
        case TOUCH_EVENT_HOLD_BACK:
            return "HOLD_BACK";
        case TOUCH_EVENT_COMBO_UP_OK:
            return "COMBO_UP_OK";
    }

    return "UNKNOWN";
}

const char *touch_moisture_name(touch_moisture_t moisture) {
    switch (moisture) {
        case TOUCH_MOISTURE_DRY:
            return "DRY";
        case TOUCH_MOISTURE_WET:
            return "WET";
    }

    return "UNKNOWN";
}

void touch_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len) {
    if (buffer_len == 0u) {
        return;
    }

    if (faults == 0u) {
        (void)snprintf(buffer, buffer_len, "none");
    } else {
        (void)snprintf(buffer, buffer_len, "moisture");
    }
}
