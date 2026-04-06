# OTA Bootloader Simulator

Host-side A/B firmware update simulator inspired by bootloader flows used in
embedded products.

Architecture:
- [Design Notes](docs/ARCHITECTURE.md)

## Features

- Chunked OTA image reception
- CRC32 validation
- Secondary-slot staging
- Test upgrade and automatic rollback
- Permanent upgrade flow

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
after test upgrade reboot: v1.1.0 crc=4CE93DFC size=22 confirmed=0
reboot without confirm: v1.0.0 crc=19A140E2 size=16 confirmed=1
after permanent upgrade reboot: v1.2.0 crc=0E7C2932 size=27 confirmed=1
```

## Future Extensions

- Signature verification with real public keys
- Flash sector erase/write constraints
- Swap metadata emulation
- UART or CAN transport framing
