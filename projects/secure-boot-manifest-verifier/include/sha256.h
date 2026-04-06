#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

#include "boot_types.h"

typedef struct {
    uint32_t state[8];
    uint64_t bit_len;
    uint8_t buffer[64];
    size_t buffer_len;
} sha256_ctx_t;

void sha256_init(sha256_ctx_t *ctx);
void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t len);
void sha256_final(sha256_ctx_t *ctx, uint8_t out[BOOT_SHA256_SIZE]);
void sha256_digest(const uint8_t *data, size_t len, uint8_t out[BOOT_SHA256_SIZE]);

#endif
