# Architecture

## Overview

The project models the policy layer of a secure boot chain. It assumes that the
bootloader can read image bytes and a signed manifest from two slots:

1. Compute SHA-256 over the slot image
2. Verify the manifest HMAC over metadata and image hash
3. Check product ID and ABI compatibility
4. Enforce a monotonic anti-rollback security counter
5. Choose primary, recovery, or reject

## Modules

- `boot_types.*`: shared enums, manifest types, and formatting helpers
- `sha256.*`: SHA-256 digest implementation
- `hmac_sha256.*`: HMAC-SHA256 implementation
- `manifest_verifier.*`: slot verification and boot-selection policy
- `main.c`: deterministic boot scenarios

## Decision Model

- `BOOT`: a trusted slot should boot now
- `RECOVERY`: primary failed verification and recovery is selected
- `REJECT`: no trusted slot is available

## Failure Reasons

- `HASH_MISMATCH`: computed image hash does not match the manifest
- `AUTH_FAILED`: manifest HMAC does not verify
- `ROLLBACK`: security counter is below the fused minimum
- `ABI_MISMATCH`: image ABI is not compatible with the platform
- `PRODUCT_MISMATCH`: manifest targets another product ID
- `MISSING_SLOT`: slot is absent or empty
