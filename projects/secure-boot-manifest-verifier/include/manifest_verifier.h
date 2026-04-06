#ifndef MANIFEST_VERIFIER_H
#define MANIFEST_VERIFIER_H

#include "boot_types.h"

boot_decision_t manifest_verifier_select(const boot_policy_t *policy,
                                         const boot_slot_image_t *primary,
                                         const boot_slot_image_t *recovery);

#endif
