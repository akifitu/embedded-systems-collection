# Stepper Motion Planner

Portable C model of a stepper-axis controller with homing, trapezoidal move
planning, limit-switch safety, and stall detection. The project resembles the
firmware logic behind CNC feeders, lab automation stages, and pick-and-place
subsystems before any real motor driver is attached.

## Why It Matters

- Shows motion-control thinking instead of only sensing or protocol handling
- Demonstrates homing, acceleration limits, and fault latching
- Maps directly to common embedded hardware such as STM32 or RP2040 plus TMC drivers

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=home_seek state=HOMING pos=1240 target=0 rate=1200 dir=REV progress=50 faults=none
phase=home_latched state=READY pos=0 target=0 rate=0 dir=STOP progress=100 faults=none
phase=move_profile state=MOVING pos=1574 target=12000 rate=4800 dir=FWD progress=13 faults=none
phase=move_complete state=READY pos=12000 target=12000 rate=0 dir=STOP progress=100 faults=none
phase=limit_abort state=FAULT pos=15014 target=16000 rate=0 dir=STOP progress=75 faults=limit_hit
phase=rehome_complete state=READY pos=0 target=0 rate=0 dir=STOP progress=100 faults=none
```

## Implementation Notes

- `trajectory_planner.c` computes triangular or trapezoidal profiles from move distance
- `stepper_controller.c` handles homing, move execution, limit switches, and stall faults
- `main.c` runs deterministic axis scenarios suitable for GitHub screenshots

## Hardware Path

Natural next step: pair an STM32 or RP2040 with a TMC2209 or A4988 driver,
step/dir timers, endstop GPIOs, and optional encoder feedback for closed-loop
verification.
