#ifndef DID_TABLE_H
#define DID_TABLE_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint16_t did;
    const uint8_t *bytes;
    size_t len;
} did_entry_t;

const did_entry_t *did_table_lookup(uint16_t did);

#endif
