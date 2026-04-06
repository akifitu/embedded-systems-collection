# CAN Telemetry Node

Portable C simulation of a CAN-speaking telemetry node.

Architecture:
- [Design Notes](docs/ARCHITECTURE.md)

## Features

- Periodic status and thermal frames
- Immediate fault event reporting
- CAN ID based priority ordering
- SocketCAN-style frame formatting

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
tick=2 emitted=3
  vcan0 080 [2] 07 00 00 00 00 00 00 00
  vcan0 180 [6] 60 04 1E 00 D4 02 00 00
  vcan0 280 [4] 16 03 07 00 00 00 00 00
```

## Future Extensions

- Real SocketCAN `vcan` sender bridge
- DBC file generation
- ISO-TP transport experiment
- Fault debounce and diagnostic trouble codes
