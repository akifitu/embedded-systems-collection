#ifndef UDS_TYPES_H
#define UDS_TYPES_H

#include <stddef.h>
#include <stdint.h>

#define UDS_MAX_FRAME_LEN 32u
#define UDS_MAX_DTC_COUNT 4u

typedef enum {
    UDS_SESSION_DEFAULT = 1,
    UDS_SESSION_PROGRAMMING = 2,
    UDS_SESSION_EXTENDED = 3
} uds_session_t;

typedef enum {
    UDS_SECURITY_LOCKED = 0,
    UDS_SECURITY_SEED_ISSUED,
    UDS_SECURITY_UNLOCKED
} uds_security_t;

typedef struct {
    uint16_t code;
    uint8_t status;
} uds_dtc_t;

typedef struct {
    uint8_t data[UDS_MAX_FRAME_LEN];
    size_t len;
} uds_frame_t;

const char *uds_session_name(uds_session_t session);
const char *uds_security_name(uds_security_t security);
void uds_frame_to_hex(const uds_frame_t *frame, char *buffer, size_t buffer_len);

#endif
