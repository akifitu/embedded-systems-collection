# Smart BMS Firmware

Host-side battery management firmware simulator written in C.

Architecture:
- [Design Notes](docs/ARCHITECTURE.md)

## Features

- Safety-focused state machine
- Fault detection and latching behavior
- Simple state-of-charge estimation
- Cell balancing decision logic
- Unit tests for common scenarios

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
step=1 state=CHARGING soc=72.02 charge=1 discharge=0 faults=none balancing=[0 0 0 1]
step=4 state=FAULT soc=72.00 charge=0 discharge=0 faults= overtemp balancing=[0 0 0 0]
```

## Future Extensions

- ADC calibration and hardware abstraction layer
- Contactor and precharge sequencing
- EEPROM-backed fault history
- CAN/UART telemetry export
