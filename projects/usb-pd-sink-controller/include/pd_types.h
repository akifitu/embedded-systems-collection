#ifndef PD_TYPES_H
#define PD_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define PD_MAX_PDOS 7u

typedef enum {
    PD_STATE_DETACHED = 0,
    PD_STATE_REQUESTING,
    PD_STATE_READY,
    PD_STATE_DERATED,
    PD_STATE_FAULT
} pd_state_t;

typedef enum {
    PD_REPLY_NONE = 0,
    PD_REPLY_ACCEPT,
    PD_REPLY_REJECT
} pd_request_reply_t;

enum {
    PD_FAULT_NONE = 0u,
    PD_FAULT_SOURCE_MISMATCH = 1u << 0,
    PD_FAULT_OVERTEMP = 1u << 1,
    PD_FAULT_BROWNOUT = 1u << 2,
    PD_FAULT_RETRY_EXHAUSTED = 1u << 3
};

typedef struct {
    uint16_t voltage_mv;
    uint16_t max_current_ma;
} pd_pdo_t;

typedef struct {
    pd_pdo_t pdos[PD_MAX_PDOS];
    uint8_t count;
} pd_source_caps_t;

typedef struct {
    bool valid;
    uint8_t pdo_index;
    uint16_t voltage_mv;
    uint16_t current_ma;
    uint32_t power_mw;
} pd_contract_t;

const char *pd_state_name(pd_state_t state);
void pd_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len);
void pd_contract_to_string(const pd_contract_t *contract, char *buffer,
                           unsigned buffer_len);
bool pd_contract_equal(const pd_contract_t *lhs, const pd_contract_t *rhs);

#endif
