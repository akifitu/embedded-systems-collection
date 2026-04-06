#include "did_table.h"

static const uint8_t k_vin[] = {'A', 'K', 'I', 'F', 'I', 'R', 'M', 'W'};
static const uint8_t k_sw[] = {'v', '1', '.', '4', '.', '2'};

static const did_entry_t k_entries[] = {
    {0xF190u, k_vin, sizeof(k_vin)},
    {0xF195u, k_sw, sizeof(k_sw)},
};

const did_entry_t *did_table_lookup(uint16_t did) {
    size_t i;

    for (i = 0u; i < sizeof(k_entries) / sizeof(k_entries[0]); ++i) {
        if (k_entries[i].did == did) {
            return &k_entries[i];
        }
    }

    return 0;
}
