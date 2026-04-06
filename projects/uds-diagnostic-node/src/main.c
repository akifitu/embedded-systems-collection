#include <stdio.h>

#include "uds_node.h"

static void print_phase(const char *phase, const uds_frame_t *request,
                        const uds_frame_t *response, const uds_node_t *node) {
    char req_buffer[128];
    char rsp_buffer[128];

    uds_frame_to_hex(request, req_buffer, sizeof(req_buffer));
    uds_frame_to_hex(response, rsp_buffer, sizeof(rsp_buffer));
    printf(
        "phase=%s req=%s rsp=%s session=%s security=%s dtc=%u\n",
        phase, req_buffer, rsp_buffer, uds_session_name(node->session),
        uds_security_name(node->security), (unsigned)node->dtc_count);
}

int main(void) {
    uds_node_t node;
    uds_frame_t request;
    uds_frame_t response;

    uds_node_init(&node);

    request.len = 2u;
    request.data[0] = 0x10u;
    request.data[1] = 0x03u;
    response = uds_node_handle(&node, &request);
    print_phase("session", &request, &response, &node);

    request.len = 3u;
    request.data[0] = 0x22u;
    request.data[1] = 0xF1u;
    request.data[2] = 0x90u;
    response = uds_node_handle(&node, &request);
    print_phase("vin", &request, &response, &node);

    request.len = 2u;
    request.data[0] = 0x27u;
    request.data[1] = 0x01u;
    response = uds_node_handle(&node, &request);
    print_phase("seed", &request, &response, &node);

    request.len = 4u;
    request.data[0] = 0x27u;
    request.data[1] = 0x02u;
    request.data[2] = 0x96u;
    request.data[3] = 0x68u;
    response = uds_node_handle(&node, &request);
    print_phase("unlock", &request, &response, &node);

    request.len = 2u;
    request.data[0] = 0x19u;
    request.data[1] = 0x02u;
    response = uds_node_handle(&node, &request);
    print_phase("dtc", &request, &response, &node);

    request.len = 4u;
    request.data[0] = 0x14u;
    request.data[1] = 0xFFu;
    request.data[2] = 0xFFu;
    request.data[3] = 0xFFu;
    response = uds_node_handle(&node, &request);
    print_phase("clear", &request, &response, &node);

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
    print_phase("bad_key", &request, &response, &node);

    return 0;
}
