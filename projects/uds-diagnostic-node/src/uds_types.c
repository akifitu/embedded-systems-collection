#include "uds_types.h"

#include <stdio.h>

const char *uds_session_name(uds_session_t session) {
    switch (session) {
        case UDS_SESSION_DEFAULT:
            return "DEFAULT";
        case UDS_SESSION_PROGRAMMING:
            return "PROGRAMMING";
        case UDS_SESSION_EXTENDED:
            return "EXTENDED";
    }

    return "UNKNOWN";
}

const char *uds_security_name(uds_security_t security) {
    switch (security) {
        case UDS_SECURITY_LOCKED:
            return "LOCKED";
        case UDS_SECURITY_SEED_ISSUED:
            return "SEED_ISSUED";
        case UDS_SECURITY_UNLOCKED:
            return "UNLOCKED";
    }

    return "UNKNOWN";
}

void uds_frame_to_hex(const uds_frame_t *frame, char *buffer, size_t buffer_len) {
    size_t i;
    size_t used = 0u;

    if (buffer_len == 0u) {
        return;
    }

    buffer[0] = '\0';
    for (i = 0u; i < frame->len; ++i) {
        int written;

        written = snprintf(buffer + used, buffer_len - used, "%s%02X",
                           (i == 0u) ? "" : " ", frame->data[i]);
        if (written < 0) {
            break;
        }
        used += (size_t)written;
        if (used >= buffer_len) {
            buffer[buffer_len - 1u] = '\0';
            break;
        }
    }
}
