# Architecture

## Overview

The project models one stepper axis running on a fixed control tick. The
controller can:

1. Seek a home switch and latch position zero
2. Plan a bounded-acceleration move to a target position
3. Generate a step rate that respects acceleration and deceleration limits
4. Watch limit switches and stall feedback
5. Latch faults until a deliberate recovery sequence clears them

## Modules

- `motion_types.*`: shared enums, fault flags, and formatting helpers
- `trajectory_planner.*`: triangular or trapezoidal move planning
- `stepper_controller.*`: homing state machine, move execution, and fault supervision
- `main.c`: deterministic axis scenarios and snapshots

## State Model

- `IDLE`: controller has not been homed yet
- `HOMING`: axis moves toward the home switch at a fixed safe rate
- `READY`: axis is homed and holding position
- `MOVING`: axis follows a profile to the commanded target
- `FAULT`: limit, stall, or timeout condition latched

## Fault Model

- `HOME_TIMEOUT`: home switch never arrived inside the allowed window
- `LIMIT_HIT`: travel limit activated unexpectedly during a move
- `STALL`: motion feedback reported repeated missed motion
- `TARGET_TIMEOUT`: move took too long for the planned profile
