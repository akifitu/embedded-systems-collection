# UDS Diagnostic Node

Portable C model of an automotive UDS diagnostic node. The project supports
session control, security access with seed/key challenge, data identifier reads,
DTC reporting and clearing, and negative-response handling for invalid access.

## Why It Matters

- Shows automotive diagnostic behavior beyond simple frame transport
- Demonstrates session/security gating and standards-style negative responses
- Maps directly to CAN ISO-TP or LIN diagnostic firmware on real ECUs

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=session req=10 03 rsp=50 03 session=EXTENDED security=LOCKED dtc=2
phase=vin req=22 F1 90 rsp=62 F1 90 41 4B 49 46 49 52 4D 57 session=EXTENDED security=LOCKED dtc=2
phase=seed req=27 01 rsp=67 01 3A C5 session=EXTENDED security=SEED_ISSUED dtc=2
phase=unlock req=27 02 96 68 rsp=67 02 session=EXTENDED security=UNLOCKED dtc=2
phase=dtc req=19 02 rsp=59 02 02 10 11 01 C2 22 02 session=EXTENDED security=UNLOCKED dtc=2
phase=clear req=14 FF FF FF rsp=54 session=EXTENDED security=UNLOCKED dtc=0
phase=bad_key req=27 02 00 00 rsp=7F 27 35 session=EXTENDED security=LOCKED dtc=2
```

## Implementation Notes

- `uds_node.c` implements session state, security access, and service dispatch
- `did_table.c` provides static DIDs such as VIN and software version
- `main.c` drives deterministic request/response examples for GitHub readers

## Hardware Path

Natural next step: connect the node to CAN ISO-TP on STM32, NXP S32K, or
another automotive MCU, and back DIDs/DTCs with real ECU storage.
