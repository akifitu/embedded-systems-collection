# Railway Grade Crossing Controller

Portable C model of a railway grade-crossing warning controller. The project
reacts to train approach and island occupancy, drives prewarning, lowers and
raises the barriers, supervises lamp health, and locks out on gate-motion or
lamp failures until a safe reset is performed.

## Why It Matters

- Shows rail-safety sequencing instead of only generic state machines
- Demonstrates warning timing, barrier feedback handling, and safety lockout
- Maps directly to wayside controller, signalling, and crossing-protection firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=idle state=IDLE cmd=GATE_UP zone=CLEAR gate=UP warn=OFF bell=OFF fault=NONE
phase=approach state=PREWARN cmd=START_WARNING zone=APPROACH gate=UP warn=ON bell=ON fault=NONE
phase=lowering state=LOWERING cmd=LOWER_GATE zone=APPROACH gate=MOVING warn=ON bell=ON fault=NONE
phase=occupied state=PROTECTED cmd=HOLD_DOWN zone=ISLAND gate=DOWN warn=ON bell=OFF fault=NONE
phase=clearing state=RAISING cmd=RAISE_GATE zone=CLEAR gate=MOVING warn=ON bell=OFF fault=NONE
phase=lamp_fault state=FAULT cmd=LATCH_FAULT zone=APPROACH gate=DOWN warn=ON bell=ON fault=LAMP_FAIL
phase=recovered state=IDLE cmd=GATE_UP zone=CLEAR gate=UP warn=OFF bell=OFF fault=NONE
```

## Implementation Notes

- `train_zone.c` derives whether the crossing is clear, on approach, or occupied
- `gate_monitor.c` maps barrier feedback into `UP`, `DOWN`, or `MOVING` and enforces a motion timeout
- `grade_crossing_controller.c` handles idle, prewarn, lowering, protected, raising, and fault recovery flow

## Hardware Path

Natural next step: port the controller to a safety MCU or PLC with track-circuit
or axle-counter inputs, lamp drivers, bell output, barrier motor contactors,
and gate position sensing.
