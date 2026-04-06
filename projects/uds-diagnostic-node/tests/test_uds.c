#include <assert.h>

#include "uds_node.h"

static void test_extended_session_switch(void) {
    uds_node_t node;
    uds_frame_t request;
    uds_frame_t response;

    uds_node_init(&node);
    request.len = 2u;
    request.data[0] = 0x10u;
    request.data[1] = 0x03u;
    response = uds_node_handle(&node, &request);

    assert(response.len == 2u);
    assert(response.data[0] == 0x50u);
    assert(node.session == UDS_SESSION_EXTENDED);
}

static void test_vin_read_works(void) {
    uds_node_t node;
    uds_frame_t request;
    uds_frame_t response;

    uds_node_init(&node);
    request.len = 3u;
    request.data[0] = 0x22u;
    request.data[1] = 0xF1u;
    request.data[2] = 0x90u;
    response = uds_node_handle(&node, &request);

    assert(response.data[0] == 0x62u);
    assert(response.data[1] == 0xF1u);
    assert(response.data[2] == 0x90u);
    assert(response.data[3] == 'A');
}

static void test_unlock_success_allows_dtc_read(void) {
    uds_node_t node;
    uds_frame_t request;
    uds_frame_t response;

    uds_node_init(&node);
    request.len = 2u;
    request.data[0] = 0x10u;
    request.data[1] = 0x03u;
    (void)uds_node_handle(&node, &request);

    request.len = 2u;
    request.data[0] = 0x27u;
    request.data[1] = 0x01u;
    response = uds_node_handle(&node, &request);
    assert(response.data[0] == 0x67u);

    request.len = 4u;
    request.data[0] = 0x27u;
    request.data[1] = 0x02u;
    request.data[2] = 0x96u;
    request.data[3] = 0x68u;
    response = uds_node_handle(&node, &request);
    assert(response.data[0] == 0x67u);
    assert(node.security == UDS_SECURITY_UNLOCKED);

    request.len = 2u;
    request.data[0] = 0x19u;
    request.data[1] = 0x02u;
    response = uds_node_handle(&node, &request);
    assert(response.data[0] == 0x59u);
    assert(response.data[2] == 2u);
}

static void test_bad_key_returns_nrc(void) {
    uds_node_t node;
    uds_frame_t request;
    uds_frame_t response;

    uds_node_init(&node);
    request.len = 2u;
    request.data[0] = 0x10u;
    request.data[1] = 0x03u;
    (void)uds_node_handle(&node, &request);
    request.len = 2u;
    request.data[0] = 0x27u;
    request.data[1] = 0x01u;
    (void)uds_node_handle(&node, &request);

    request.len = 4u;
    request.data[0] = 0x27u;
    request.data[1] = 0x02u;
    request.data[2] = 0x00u;
    request.data[3] = 0x00u;
    response = uds_node_handle(&node, &request);

    assert(response.data[0] == 0x7Fu);
    assert(response.data[1] == 0x27u);
    assert(response.data[2] == 0x35u);
    assert(node.security == UDS_SECURITY_LOCKED);
}

static void test_clear_dtc_empties_store(void) {
    uds_node_t node;
    uds_frame_t request;
    uds_frame_t response;

    uds_node_init(&node);
    request.len = 2u;
    request.data[0] = 0x10u;
    request.data[1] = 0x03u;
    (void)uds_node_handle(&node, &request);
    request.len = 2u;
    request.data[0] = 0x27u;
    request.data[1] = 0x01u;
    (void)uds_node_handle(&node, &request);
    request.len = 4u;
    request.data[0] = 0x27u;
    request.data[1] = 0x02u;
    request.data[2] = 0x96u;
    request.data[3] = 0x68u;
    (void)uds_node_handle(&node, &request);

    request.len = 4u;
    request.data[0] = 0x14u;
    request.data[1] = 0xFFu;
    request.data[2] = 0xFFu;
    request.data[3] = 0xFFu;
    response = uds_node_handle(&node, &request);

    assert(response.data[0] == 0x54u);
    assert(node.dtc_count == 0u);
}

int main(void) {
    test_extended_session_switch();
    test_vin_read_works();
    test_unlock_success_allows_dtc_read();
    test_bad_key_returns_nrc();
    test_clear_dtc_empties_store();
    return 0;
}
