#include <stdio.h>
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

static void print_decision(const char *phase, const boot_decision_t *decision,
                           const boot_policy_t *policy) {
    printf(
        "phase=%s slot=%s verdict=%s counter=%u abi=%u product=0x%X reason=%s\n",
        phase, boot_slot_name(decision->slot), boot_verdict_name(decision->verdict),
        decision->accepted_counter, policy->expected_abi_major,
        policy->expected_product_id, verify_reason_name(decision->reason));
}

int main(void) {
    static const uint8_t primary_bytes[] = "primary-fw-1.0.3";
    static const uint8_t recovery_bytes[] = "recovery-fw-1.1.0";
    boot_policy_t policy;
    boot_slot_image_t primary;
    boot_slot_image_t recovery;
    boot_decision_t decision;

    policy.expected_product_id = 0x42u;
    policy.expected_abi_major = 3u;
    policy.fused_min_counter = 8u;
    policy.best_known_counter = 8u;
    policy.auth_key = k_auth_key;
    policy.auth_key_len = sizeof(k_auth_key);

    primary = make_slot(primary_bytes, sizeof(primary_bytes) - 1u, 0x42u, 3u, 9u);
    recovery = make_slot(recovery_bytes, sizeof(recovery_bytes) - 1u, 0x42u, 3u, 11u);
    decision = manifest_verifier_select(&policy, &primary, &recovery);
    print_decision("golden", &decision, &policy);

    primary = make_slot(primary_bytes, sizeof(primary_bytes) - 1u, 0x42u, 3u, 7u);
    decision = manifest_verifier_select(&policy, &primary, &recovery);
    print_decision("rollback", &decision, &policy);

    primary = make_slot(primary_bytes, sizeof(primary_bytes) - 1u, 0x42u, 3u, 9u);
    primary.bytes = (const uint8_t *)"primary-fw-1.0.x";
    primary.len = sizeof("primary-fw-1.0.x") - 1u;
    decision = manifest_verifier_select(&policy, &primary, &recovery);
    print_decision("tamper", &decision, &policy);

    primary = make_slot(primary_bytes, sizeof(primary_bytes) - 1u, 0x43u, 3u, 12u);
    decision = manifest_verifier_select(&policy, &primary, &recovery);
    print_decision("wrong_product", &decision, &policy);

    memset(&primary, 0, sizeof(primary));
    decision = manifest_verifier_select(&policy, &primary, &recovery);
    print_decision("recovery_only", &decision, &policy);

    recovery.manifest.manifest_hmac[0] ^= 0x55u;
    decision = manifest_verifier_select(&policy, &primary, &recovery);
    print_decision("hard_fail", &decision, &policy);

    return 0;
}
