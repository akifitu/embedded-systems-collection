# Sensorless BLDC Startup

Portable C model of a sensorless BLDC startup controller. The project aligns
the rotor, ramps six-step commutation in open loop, waits for back-EMF
zero-cross events to lock closed loop, and latches startup faults such as
overcurrent, no-lock, or stall.

## Why It Matters

- Shows motor-drive firmware thinking beyond generic control loops
- Demonstrates the hard part of sensorless BLDC: getting from standstill to lock
- Maps cleanly to STM32 timer/comparator firmware with real gate drivers

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=align state=ALIGN sector=S1 duty=18 period=0us rpm=0 lock=SEEKING faults=none
phase=ramp state=OPEN_LOOP sector=S3 duty=26 period=2400us rpm=625 lock=SEEKING faults=none
phase=lock state=CLOSED_LOOP sector=S6 duty=34 period=806us rpm=1860 lock=LOCKED faults=none
phase=load_step state=CLOSED_LOOP sector=S1 duty=46 period=1063us rpm=1410 lock=LOCKED faults=none
phase=overcurrent state=FAULT sector=S6 duty=0 period=0us rpm=0 lock=LOST faults=overcurrent
phase=no_lock state=FAULT sector=S6 duty=0 period=0us rpm=0 lock=LOST faults=no_lock
```

## Implementation Notes

- `commutation_table.c` advances the six-step sector order and estimates speed
- `startup_controller.c` models `ALIGN`, `OPEN_LOOP`, `CLOSED_LOOP`, and `FAULT`
- `main.c` runs deterministic nominal and fault startup scenarios

## Hardware Path

Natural next step: STM32 with complementary timer PWM, comparator or ADC-based
back-EMF sensing, current shunt protection, and a half-bridge gate driver.
