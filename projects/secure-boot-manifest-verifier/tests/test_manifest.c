#include <assert.h>
#include <string.h>

#include "hmac_sha256.h"
#include "manifest_verifier.h"
#include "sha256.h"

static const uint8_t k_auth_key[] = {
    0x31u, 0x42u, 0x53u, 0x64u, 0x75u, 0x86u, 0x97u, 0xa8u,
    0xb9u, 0xcau, 0xdbu, 0xecu, 0xfdu, 0x10u, 0x21u, 0x32u,
};

static void write_u16_be(uint8_t *dst, uint16_t value) {
    dst[0] = (uint8_t)(value >> 8);
    dst[1] = (uint8_t)value;
}

static void write_u32_be(uint8_t *dst, uint32_t value) {
    dst[0] = (uint8_t)(value >> 24);
    dst[1] = (uint8_t)(value >> 16);
    dst[2] = (uint8_t)(value >> 8);
    dst[3] = (uint8_t)value;
}

static void sign_manifest(boot_manifest_t *manifest) {
    uint8_t auth_material[44];

    write_u32_be(auth_material + 0u, manifest->product_id);
    write_u16_be(auth_material + 4u, manifest->abi_major);
    write_u16_be(auth_material + 6u, manifest->security_counter);
    write_u32_be(auth_material + 8u, manifest->image_size);
    memcpy(auth_material + 12u, manifest->image_hash, BOOT_SHA256_SIZE);
    hmac_sha256(k_auth_key, sizeof(k_auth_key), auth_material,
                sizeof(auth_material), manifest->manifest_hmac);
}

static boot_slot_image_t make_slot(const uint8_t *bytes, size_t len,
                                   uint32_t product_id, uint16_t abi_major,
                                   uint16_t counter) {
    boot_slot_image_t slot;

    slot.bytes = bytes;
    slot.len = len;
    slot.manifest.present = true;
    slot.manifest.product_id = product_id;
    slot.manifest.abi_major = abi_major;
    slot.manifest.security_counter = counter;
    slot.manifest.image_size = (uint32_t)len;
    sha256_digest(bytes, len, slot.manifest.image_hash);
    sign_manifest(&slot.manifest);
    return slot;
}

static boot_policy_t make_policy(void) {
    boot_policy_t policy;

    policy.expected_product_id = 0x42u;
    policy.expected_abi_major = 3u;
    policy.fused_min_counter = 8u;
    policy.best_known_counter = 8u;
    policy.auth_key = k_auth_key;
    policy.auth_key_len = sizeof(k_auth_key);
    return policy;
}

static void test_golden_primary_boots(void) {
    static const uint8_t primary_bytes[] = "primary-fw-1.0.3";
    static const uint8_t recovery_bytes[] = "recovery-fw-1.1.0";
    boot_policy_t policy = make_policy();
    boot_slot_image_t primary =
        make_slot(primary_bytes, sizeof(primary_bytes) - 1u, 0x42u, 3u, 9u);
    boot_slot_image_t recovery =
        make_slot(recovery_bytes, sizeof(recovery_bytes) - 1u, 0x42u, 3u, 11u);
    boot_decision_t decision =
        manifest_verifier_select(&policy, &primary, &recovery);

    assert(decision.slot == BOOT_SLOT_PRIMARY);
    assert(decision.verdict == BOOT_VERDICT_BOOT);
    assert(decision.reason == VERIFY_REASON_OK);
}

static void test_rollback_falls_back_to_recovery(void) {
    static const uint8_t primary_bytes[] = "primary-fw-1.0.3";
    static const uint8_t recovery_bytes[] = "recovery-fw-1.1.0";
    boot_policy_t policy = make_policy();
    boot_slot_image_t primary =
        make_slot(primary_bytes, sizeof(primary_bytes) - 1u, 0x42u, 3u, 7u);
    boot_slot_image_t recovery =
        make_slot(recovery_bytes, sizeof(recovery_bytes) - 1u, 0x42u, 3u, 11u);
    boot_decision_t decision =
        manifest_verifier_select(&policy, &primary, &recovery);

    assert(decision.slot == BOOT_SLOT_RECOVERY);
    assert(decision.verdict == BOOT_VERDICT_RECOVERY);
    assert(decision.reason == VERIFY_REASON_ROLLBACK);
}

static void test_hash_tamper_detected(void) {
    static const uint8_t primary_bytes[] = "primary-fw-1.0.3";
    static const uint8_t recovery_bytes[] = "recovery-fw-1.1.0";
    boot_policy_t policy = make_policy();
    boot_slot_image_t primary =
        make_slot(primary_bytes, sizeof(primary_bytes) - 1u, 0x42u, 3u, 9u);
    boot_slot_image_t recovery =
        make_slot(recovery_bytes, sizeof(recovery_bytes) - 1u, 0x42u, 3u, 11u);
    boot_decision_t decision;

    primary.bytes = (const uint8_t *)"primary-fw-1.0.x";
    primary.len = sizeof("primary-fw-1.0.x") - 1u;
    decision = manifest_verifier_select(&policy, &primary, &recovery);

    assert(decision.slot == BOOT_SLOT_RECOVERY);
    assert(decision.reason == VERIFY_REASON_HASH_MISMATCH);
}

static void test_wrong_product_rejected_for_primary(void) {
    static const uint8_t primary_bytes[] = "primary-fw-1.0.3";
    static const uint8_t recovery_bytes[] = "recovery-fw-1.1.0";
    boot_policy_t policy = make_policy();
    boot_slot_image_t primary =
        make_slot(primary_bytes, sizeof(primary_bytes) - 1u, 0x43u, 3u, 12u);
    boot_slot_image_t recovery =
        make_slot(recovery_bytes, sizeof(recovery_bytes) - 1u, 0x42u, 3u, 11u);
    boot_decision_t decision =
        manifest_verifier_select(&policy, &primary, &recovery);

    assert(decision.slot == BOOT_SLOT_RECOVERY);
    assert(decision.reason == VERIFY_REASON_PRODUCT_MISMATCH);
}

static void test_auth_failure_rejects_all(void) {
    static const uint8_t recovery_bytes[] = "recovery-fw-1.1.0";
    boot_policy_t policy = make_policy();
    boot_slot_image_t primary = {0};
    boot_slot_image_t recovery =
        make_slot(recovery_bytes, sizeof(recovery_bytes) - 1u, 0x42u, 3u, 11u);
    boot_decision_t decision;

    recovery.manifest.manifest_hmac[0] ^= 0x55u;
    decision = manifest_verifier_select(&policy, &primary, &recovery);

    assert(decision.slot == BOOT_SLOT_NONE);
    assert(decision.verdict == BOOT_VERDICT_REJECT);
    assert(decision.reason == VERIFY_REASON_AUTH_FAILED);
}

int main(void) {
    test_golden_primary_boots();
    test_rollback_falls_back_to_recovery();
    test_hash_tamper_detected();
    test_wrong_product_rejected_for_primary();
    test_auth_failure_rejects_all();

    return 0;
}
