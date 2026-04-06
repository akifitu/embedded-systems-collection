# Architecture

## Overview

The project models a sensorless BLDC startup state machine that runs on a fixed
control tick. Each step:

1. Applies rotor alignment at a safe duty cycle
2. Advances a six-step commutation sequence in open loop
3. Watches for back-EMF zero-cross events
4. Switches to closed-loop commutation once lock is credible
5. Latches faults if current rises too high or lock never arrives

## Modules

- `bldc_types.*`: shared enums, fault flags, and formatting helpers
- `commutation_table.*`: six-step sector advance and speed/period conversion
- `startup_controller.*`: alignment, ramp, lock, and fault supervision
- `main.c`: deterministic motor scenarios and demo snapshots

## State Model

- `IDLE`: controller is inactive
- `ALIGN`: rotor is pre-positioned before ramp
- `OPEN_LOOP`: commutation period decreases without back-EMF feedback
- `CLOSED_LOOP`: zero-cross events keep the commutation synchronized
- `FAULT`: drive outputs are disabled until software clears the error

## Fault Model

- `NO_LOCK`: zero-cross lock never arrived during startup
- `OVERCURRENT`: phase current exceeded the safe threshold
- `STALL`: lock was lost after entering closed loop
