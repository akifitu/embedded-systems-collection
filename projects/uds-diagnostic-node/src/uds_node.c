#include "uds_node.h"

#include <string.h>

#include "did_table.h"

enum {
    NRC_SERVICE_NOT_SUPPORTED = 0x11u,
    NRC_SUBFUNCTION_NOT_SUPPORTED = 0x12u,
    NRC_INCORRECT_LENGTH = 0x13u,
    NRC_CONDITIONS_NOT_CORRECT = 0x22u,
    NRC_SECURITY_ACCESS_DENIED = 0x33u,
    NRC_INVALID_KEY = 0x35u,
    NRC_REQUEST_OUT_OF_RANGE = 0x31u
};

static uds_frame_t make_negative(uint8_t sid, uint8_t nrc) {
    uds_frame_t frame;

    frame.len = 3u;
    frame.data[0] = 0x7Fu;
    frame.data[1] = sid;
    frame.data[2] = nrc;
    return frame;
}

static uint16_t uds_expected_key(uint16_t seed) {
    return (uint16_t)(seed ^ 0xACADu);
}

static uds_frame_t handle_session_control(uds_node_t *node,
                                          const uds_frame_t *request) {
    uds_frame_t response;
    uint8_t subfunction;

    if (request->len != 2u) {
        return make_negative(0x10u, NRC_INCORRECT_LENGTH);
    }

    subfunction = request->data[1];
    if (subfunction == 0x01u) {
        node->session = UDS_SESSION_DEFAULT;
        node->security = UDS_SECURITY_LOCKED;
    } else if (subfunction == 0x02u) {
        node->session = UDS_SESSION_PROGRAMMING;
        node->security = UDS_SECURITY_LOCKED;
    } else if (subfunction == 0x03u) {
        node->session = UDS_SESSION_EXTENDED;
        node->security = UDS_SECURITY_LOCKED;
    } else {
        return make_negative(0x10u, NRC_SUBFUNCTION_NOT_SUPPORTED);
    }

    response.len = 2u;
    response.data[0] = 0x50u;
    response.data[1] = subfunction;
    return response;
}

static uds_frame_t handle_read_did(const uds_node_t *node,
                                   const uds_frame_t *request) {
    uds_frame_t response;
    const did_entry_t *entry;
    uint16_t did;

    (void)node;
    if (request->len != 3u) {
        return make_negative(0x22u, NRC_INCORRECT_LENGTH);
    }

    did = (uint16_t)((uint16_t)request->data[1] << 8) | request->data[2];
    entry = did_table_lookup(did);
    if (entry == 0) {
        return make_negative(0x22u, NRC_REQUEST_OUT_OF_RANGE);
    }

    response.len = 3u + entry->len;
    response.data[0] = 0x62u;
    response.data[1] = request->data[1];
    response.data[2] = request->data[2];
    memcpy(response.data + 3u, entry->bytes, entry->len);
    return response;
}

static uds_frame_t handle_security_access(uds_node_t *node,
                                          const uds_frame_t *request) {
    uds_frame_t response;

    if (request->len < 2u) {
        return make_negative(0x27u, NRC_INCORRECT_LENGTH);
    }
    if (node->session != UDS_SESSION_EXTENDED &&
        node->session != UDS_SESSION_PROGRAMMING) {
        return make_negative(0x27u, NRC_CONDITIONS_NOT_CORRECT);
    }

    if (request->data[1] == 0x01u) {
        node->current_seed = 0x3AC5u;
        node->security = UDS_SECURITY_SEED_ISSUED;
        response.len = 4u;
        response.data[0] = 0x67u;
        response.data[1] = 0x01u;
        response.data[2] = (uint8_t)(node->current_seed >> 8);
        response.data[3] = (uint8_t)node->current_seed;
        return response;
    }

    if (request->data[1] == 0x02u) {
        uint16_t supplied_key;

        if (request->len != 4u) {
            return make_negative(0x27u, NRC_INCORRECT_LENGTH);
        }
        if (node->security != UDS_SECURITY_SEED_ISSUED) {
            return make_negative(0x27u, NRC_SECURITY_ACCESS_DENIED);
        }

        supplied_key = (uint16_t)((uint16_t)request->data[2] << 8) |
                       request->data[3];
        if (supplied_key != uds_expected_key(node->current_seed)) {
            node->security = UDS_SECURITY_LOCKED;
            return make_negative(0x27u, NRC_INVALID_KEY);
        }

        node->security = UDS_SECURITY_UNLOCKED;
        response.len = 2u;
        response.data[0] = 0x67u;
        response.data[1] = 0x02u;
        return response;
    }

    return make_negative(0x27u, NRC_SUBFUNCTION_NOT_SUPPORTED);
}

static uds_frame_t handle_read_dtc(const uds_node_t *node,
                                   const uds_frame_t *request) {
    uds_frame_t response;
    size_t i;

    if (request->len != 2u || request->data[1] != 0x02u) {
        return make_negative(0x19u, NRC_REQUEST_OUT_OF_RANGE);
    }
    if (node->security != UDS_SECURITY_UNLOCKED) {
        return make_negative(0x19u, NRC_SECURITY_ACCESS_DENIED);
    }

    response.len = 3u + node->dtc_count * 3u;
    response.data[0] = 0x59u;
    response.data[1] = 0x02u;
    response.data[2] = (uint8_t)node->dtc_count;
    for (i = 0u; i < node->dtc_count; ++i) {
        response.data[3u + (i * 3u)] = (uint8_t)(node->dtcs[i].code >> 8);
        response.data[4u + (i * 3u)] = (uint8_t)node->dtcs[i].code;
        response.data[5u + (i * 3u)] = node->dtcs[i].status;
    }
    return response;
}

static uds_frame_t handle_clear_dtc(uds_node_t *node,
                                    const uds_frame_t *request) {
    uds_frame_t response;

    if (request->len != 4u) {
        return make_negative(0x14u, NRC_INCORRECT_LENGTH);
    }
    if (node->security != UDS_SECURITY_UNLOCKED) {
        return make_negative(0x14u, NRC_SECURITY_ACCESS_DENIED);
    }

    node->dtc_count = 0u;
    response.len = 1u;
    response.data[0] = 0x54u;
    return response;
}

void uds_node_init(uds_node_t *node) {
    node->session = UDS_SESSION_DEFAULT;
    node->security = UDS_SECURITY_LOCKED;
    node->current_seed = 0u;
    node->dtc_count = 2u;
    node->dtcs[0].code = 0x1011u;
    node->dtcs[0].status = 0x01u;
    node->dtcs[1].code = 0xC222u;
    node->dtcs[1].status = 0x02u;
}

uds_frame_t uds_node_handle(uds_node_t *node, const uds_frame_t *request) {
    if (request->len == 0u) {
        return make_negative(0x00u, NRC_INCORRECT_LENGTH);
    }

    switch (request->data[0]) {
        case 0x10u:
            return handle_session_control(node, request);
        case 0x22u:
            return handle_read_did(node, request);
        case 0x27u:
            return handle_security_access(node, request);
        case 0x19u:
            return handle_read_dtc(node, request);
        case 0x14u:
            return handle_clear_dtc(node, request);
        default:
            return make_negative(request->data[0], NRC_SERVICE_NOT_SUPPORTED);
    }
}
