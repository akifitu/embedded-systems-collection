#include "sha256.h"

#include <string.h>

static const uint32_t k_sha256[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
    0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
    0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
    0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
    0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
    0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
    0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
    0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
    0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
    0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u,
};

static uint32_t rotr32(uint32_t value, uint32_t shift) {
    return (value >> shift) | (value << (32u - shift));
}

static uint32_t load_be32(const uint8_t *src) {
    return ((uint32_t)src[0] << 24) | ((uint32_t)src[1] << 16) |
           ((uint32_t)src[2] << 8) | (uint32_t)src[3];
}

static void store_be32(uint32_t value, uint8_t *dst) {
    dst[0] = (uint8_t)(value >> 24);
    dst[1] = (uint8_t)(value >> 16);
    dst[2] = (uint8_t)(value >> 8);
    dst[3] = (uint8_t)value;
}

static void sha256_transform(sha256_ctx_t *ctx, const uint8_t block[64]) {
    uint32_t w[64];
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    uint32_t g;
    uint32_t h;
    unsigned i;

    for (i = 0u; i < 16u; ++i) {
        w[i] = load_be32(block + (i * 4u));
    }
    for (i = 16u; i < 64u; ++i) {
        uint32_t s0 = rotr32(w[i - 15u], 7u) ^ rotr32(w[i - 15u], 18u) ^
                      (w[i - 15u] >> 3u);
        uint32_t s1 = rotr32(w[i - 2u], 17u) ^ rotr32(w[i - 2u], 19u) ^
                      (w[i - 2u] >> 10u);

        w[i] = w[i - 16u] + s0 + w[i - 7u] + s1;
    }

    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    for (i = 0u; i < 64u; ++i) {
        uint32_t s1 = rotr32(e, 6u) ^ rotr32(e, 11u) ^ rotr32(e, 25u);
        uint32_t ch = (e & f) ^ (~e & g);
        uint32_t temp1 = h + s1 + ch + k_sha256[i] + w[i];
        uint32_t s0 = rotr32(a, 2u) ^ rotr32(a, 13u) ^ rotr32(a, 22u);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        uint32_t temp2 = s0 + maj;

        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

void sha256_init(sha256_ctx_t *ctx) {
    ctx->state[0] = 0x6a09e667u;
    ctx->state[1] = 0xbb67ae85u;
    ctx->state[2] = 0x3c6ef372u;
    ctx->state[3] = 0xa54ff53au;
    ctx->state[4] = 0x510e527fu;
    ctx->state[5] = 0x9b05688cu;
    ctx->state[6] = 0x1f83d9abu;
    ctx->state[7] = 0x5be0cd19u;
    ctx->bit_len = 0u;
    ctx->buffer_len = 0u;
}

void sha256_update(sha256_ctx_t *ctx, const uint8_t *data, size_t len) {
    size_t i;

    for (i = 0u; i < len; ++i) {
        ctx->buffer[ctx->buffer_len++] = data[i];
        if (ctx->buffer_len == 64u) {
            sha256_transform(ctx, ctx->buffer);
            ctx->bit_len += 512u;
            ctx->buffer_len = 0u;
        }
    }
}

void sha256_final(sha256_ctx_t *ctx, uint8_t out[BOOT_SHA256_SIZE]) {
    unsigned i;

    ctx->bit_len += (uint64_t)ctx->buffer_len * 8u;
    ctx->buffer[ctx->buffer_len++] = 0x80u;

    if (ctx->buffer_len > 56u) {
        while (ctx->buffer_len < 64u) {
            ctx->buffer[ctx->buffer_len++] = 0u;
        }
        sha256_transform(ctx, ctx->buffer);
        ctx->buffer_len = 0u;
    }

    while (ctx->buffer_len < 56u) {
        ctx->buffer[ctx->buffer_len++] = 0u;
    }

    for (i = 0u; i < 8u; ++i) {
        ctx->buffer[63u - i] = (uint8_t)(ctx->bit_len >> (i * 8u));
    }
    sha256_transform(ctx, ctx->buffer);

    for (i = 0u; i < 8u; ++i) {
        store_be32(ctx->state[i], out + (i * 4u));
    }

    memset(ctx, 0, sizeof(*ctx));
}

void sha256_digest(const uint8_t *data, size_t len, uint8_t out[BOOT_SHA256_SIZE]) {
    sha256_ctx_t ctx;

    sha256_init(&ctx);
    sha256_update(&ctx, data, len);
    sha256_final(&ctx, out);
}
