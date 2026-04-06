# Secure Boot Manifest Verifier

Portable C model of a secure boot verifier that evaluates signed image
manifests for primary and recovery slots. The project validates image hash,
checks an HMAC-authenticated manifest, enforces anti-rollback security
counters, and decides whether to boot primary, fall back to recovery, or
reject both images.

## Why It Matters

- Shows boot-chain trust decisions instead of only application-layer security
- Demonstrates anti-rollback, product/ABI gating, and recovery slot strategy
- Maps directly to first-stage bootloader logic on MCUs and secure SoCs

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=golden slot=PRIMARY verdict=BOOT counter=9 abi=3 product=0x42 reason=OK
phase=rollback slot=RECOVERY verdict=RECOVERY counter=11 abi=3 product=0x42 reason=ROLLBACK
phase=tamper slot=RECOVERY verdict=RECOVERY counter=11 abi=3 product=0x42 reason=HASH_MISMATCH
phase=wrong_product slot=RECOVERY verdict=RECOVERY counter=11 abi=3 product=0x42 reason=PRODUCT_MISMATCH
phase=recovery_only slot=RECOVERY verdict=RECOVERY counter=11 abi=3 product=0x42 reason=MISSING_SLOT
phase=hard_fail slot=NONE verdict=REJECT counter=8 abi=3 product=0x42 reason=AUTH_FAILED
```

## Implementation Notes

- `sha256.c` and `hmac_sha256.c` compute image hash and manifest authentication
- `manifest_verifier.c` enforces product, ABI, hash, authentication, and counter checks
- `main.c` drives deterministic slot-selection scenarios for portfolio review

## Hardware Path

Natural next step: connect the verifier to a ROM or first-stage bootloader with
OTP-backed security counters and a recovery-slot policy similar to MCUboot.
