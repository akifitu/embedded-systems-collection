#ifndef UDS_NODE_H
#define UDS_NODE_H

#include <stddef.h>
#include <stdint.h>

#include "uds_types.h"

typedef struct {
    uds_session_t session;
    uds_security_t security;
    uint16_t current_seed;
    uds_dtc_t dtcs[UDS_MAX_DTC_COUNT];
    size_t dtc_count;
} uds_node_t;

void uds_node_init(uds_node_t *node);
uds_frame_t uds_node_handle(uds_node_t *node, const uds_frame_t *request);

#endif
