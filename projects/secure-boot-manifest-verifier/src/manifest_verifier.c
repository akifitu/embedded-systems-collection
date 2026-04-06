#include "manifest_verifier.h"

#include <string.h>

#include "hmac_sha256.h"
#include "sha256.h"

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

static void auth_material_from_manifest(const boot_manifest_t *manifest,
                                        uint8_t out[44]) {
    write_u32_be(out + 0u, manifest->product_id);
    write_u16_be(out + 4u, manifest->abi_major);
    write_u16_be(out + 6u, manifest->security_counter);
    write_u32_be(out + 8u, manifest->image_size);
    memcpy(out + 12u, manifest->image_hash, BOOT_SHA256_SIZE);
}

static verify_reason_t verify_slot(const boot_policy_t *policy,
                                   const boot_slot_image_t *slot) {
    uint8_t computed_hash[BOOT_SHA256_SIZE];
    uint8_t computed_hmac[BOOT_SHA256_SIZE];
    uint8_t auth_material[44];

    if (!slot->manifest.present || slot->bytes == 0 || slot->len == 0u) {
        return VERIFY_REASON_MISSING_SLOT;
    }
    if (slot->manifest.product_id != policy->expected_product_id) {
        return VERIFY_REASON_PRODUCT_MISMATCH;
    }
    if (slot->manifest.abi_major != policy->expected_abi_major) {
        return VERIFY_REASON_ABI_MISMATCH;
    }
    if (slot->manifest.security_counter < policy->fused_min_counter ||
        slot->manifest.security_counter < policy->best_known_counter) {
        return VERIFY_REASON_ROLLBACK;
    }
    if (slot->manifest.image_size != slot->len) {
        return VERIFY_REASON_HASH_MISMATCH;
    }

    sha256_digest(slot->bytes, slot->len, computed_hash);
    if (memcmp(computed_hash, slot->manifest.image_hash, sizeof(computed_hash)) != 0) {
        return VERIFY_REASON_HASH_MISMATCH;
    }

    auth_material_from_manifest(&slot->manifest, auth_material);
    hmac_sha256(policy->auth_key, policy->auth_key_len, auth_material,
                sizeof(auth_material), computed_hmac);
    if (memcmp(computed_hmac, slot->manifest.manifest_hmac,
               sizeof(computed_hmac)) != 0) {
        return VERIFY_REASON_AUTH_FAILED;
    }

    return VERIFY_REASON_OK;
}

boot_decision_t manifest_verifier_select(const boot_policy_t *policy,
                                         const boot_slot_image_t *primary,
                                         const boot_slot_image_t *recovery) {
    boot_decision_t decision;
    verify_reason_t primary_reason = verify_slot(policy, primary);
    verify_reason_t recovery_reason = verify_slot(policy, recovery);

    decision.slot = BOOT_SLOT_NONE;
    decision.verdict = BOOT_VERDICT_REJECT;
    decision.reason = primary_reason;
    decision.accepted_counter = policy->best_known_counter;

    if (primary_reason == VERIFY_REASON_OK) {
        decision.slot = BOOT_SLOT_PRIMARY;
        decision.verdict = BOOT_VERDICT_BOOT;
        decision.reason = VERIFY_REASON_OK;
        decision.accepted_counter = primary->manifest.security_counter;
        return decision;
    }

    if (recovery_reason == VERIFY_REASON_OK) {
        decision.slot = BOOT_SLOT_RECOVERY;
        decision.verdict = BOOT_VERDICT_RECOVERY;
        decision.reason = primary_reason;
        decision.accepted_counter = recovery->manifest.security_counter;
        return decision;
    }

    if (recovery_reason != VERIFY_REASON_MISSING_SLOT) {
        decision.reason = recovery_reason;
    }

    return decision;
}
