#ifndef BOOT_TYPES_H
#define BOOT_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BOOT_SHA256_SIZE 32u

typedef enum {
    BOOT_SLOT_NONE = 0,
    BOOT_SLOT_PRIMARY,
    BOOT_SLOT_RECOVERY
} boot_slot_t;

typedef enum {
    BOOT_VERDICT_REJECT = 0,
    BOOT_VERDICT_BOOT,
    BOOT_VERDICT_RECOVERY
} boot_verdict_t;

typedef enum {
    VERIFY_REASON_OK = 0,
    VERIFY_REASON_HASH_MISMATCH,
    VERIFY_REASON_AUTH_FAILED,
    VERIFY_REASON_ROLLBACK,
    VERIFY_REASON_ABI_MISMATCH,
    VERIFY_REASON_PRODUCT_MISMATCH,
    VERIFY_REASON_MISSING_SLOT
} verify_reason_t;

typedef struct {
    bool present;
    uint32_t product_id;
    uint16_t abi_major;
    uint16_t security_counter;
    uint32_t image_size;
    uint8_t image_hash[BOOT_SHA256_SIZE];
    uint8_t manifest_hmac[BOOT_SHA256_SIZE];
} boot_manifest_t;

typedef struct {
    const uint8_t *bytes;
    size_t len;
    boot_manifest_t manifest;
} boot_slot_image_t;

typedef struct {
    uint32_t expected_product_id;
    uint16_t expected_abi_major;
    uint16_t fused_min_counter;
    uint16_t best_known_counter;
    const uint8_t *auth_key;
    size_t auth_key_len;
} boot_policy_t;

typedef struct {
    boot_slot_t slot;
    boot_verdict_t verdict;
    verify_reason_t reason;
    uint16_t accepted_counter;
} boot_decision_t;

const char *boot_slot_name(boot_slot_t slot);
const char *boot_verdict_name(boot_verdict_t verdict);
const char *verify_reason_name(verify_reason_t reason);

#endif
