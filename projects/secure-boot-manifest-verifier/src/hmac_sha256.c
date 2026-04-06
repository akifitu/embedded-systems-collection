#include "hmac_sha256.h"

#include <string.h>

#include "sha256.h"

void hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data,
                 size_t data_len, uint8_t out[BOOT_SHA256_SIZE]) {
    uint8_t key_block[64];
    uint8_t inner_digest[BOOT_SHA256_SIZE];
    uint8_t pad[64];
    sha256_ctx_t ctx;
    size_t i;

    memset(key_block, 0, sizeof(key_block));
    if (key_len > sizeof(key_block)) {
        sha256_digest(key, key_len, key_block);
    } else if (key_len != 0u) {
        memcpy(key_block, key, key_len);
    }

    for (i = 0u; i < sizeof(pad); ++i) {
        pad[i] = (uint8_t)(key_block[i] ^ 0x36u);
    }
    sha256_init(&ctx);
    sha256_update(&ctx, pad, sizeof(pad));
    sha256_update(&ctx, data, data_len);
    sha256_final(&ctx, inner_digest);

    for (i = 0u; i < sizeof(pad); ++i) {
        pad[i] = (uint8_t)(key_block[i] ^ 0x5cu);
    }
    sha256_init(&ctx);
    sha256_update(&ctx, pad, sizeof(pad));
    sha256_update(&ctx, inner_digest, sizeof(inner_digest));
    sha256_final(&ctx, out);

    memset(key_block, 0, sizeof(key_block));
    memset(inner_digest, 0, sizeof(inner_digest));
    memset(pad, 0, sizeof(pad));
}
