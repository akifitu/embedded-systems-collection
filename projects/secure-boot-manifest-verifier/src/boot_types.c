#include "boot_types.h"

const char *boot_slot_name(boot_slot_t slot) {
    switch (slot) {
        case BOOT_SLOT_NONE:
            return "NONE";
        case BOOT_SLOT_PRIMARY:
            return "PRIMARY";
        case BOOT_SLOT_RECOVERY:
            return "RECOVERY";
    }

    return "UNKNOWN";
}

const char *boot_verdict_name(boot_verdict_t verdict) {
    switch (verdict) {
        case BOOT_VERDICT_REJECT:
            return "REJECT";
        case BOOT_VERDICT_BOOT:
            return "BOOT";
        case BOOT_VERDICT_RECOVERY:
            return "RECOVERY";
    }

    return "UNKNOWN";
}

const char *verify_reason_name(verify_reason_t reason) {
    switch (reason) {
        case VERIFY_REASON_OK:
            return "OK";
        case VERIFY_REASON_HASH_MISMATCH:
            return "HASH_MISMATCH";
        case VERIFY_REASON_AUTH_FAILED:
            return "AUTH_FAILED";
        case VERIFY_REASON_ROLLBACK:
            return "ROLLBACK";
        case VERIFY_REASON_ABI_MISMATCH:
            return "ABI_MISMATCH";
        case VERIFY_REASON_PRODUCT_MISMATCH:
            return "PRODUCT_MISMATCH";
        case VERIFY_REASON_MISSING_SLOT:
            return "MISSING_SLOT";
    }

    return "UNKNOWN";
}
