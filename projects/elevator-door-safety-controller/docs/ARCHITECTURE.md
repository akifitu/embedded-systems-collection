# Architecture

## Overview

The project models an elevator door controller that runs once per service
frame. Each step:

1. Reads open/close requests, obstruction input, door feedbacks, and lock status
2. Derives the current door position from the feedback sensors
3. Attempts normal close first, then escalates to nudge after repeated obstructions
4. Verifies the lock contact after the doors reach the closed position
5. Latches fault state until a safe reset is performed

## Modules

- `door_types.*`: enums, labels, and shared input/output structures
- `door_monitor.*`: door-position and motion-timeout helpers
- `obstruction_policy.*`: retry counting and nudge decision logic
- `elevator_door_controller.*`: opening, open hold, closing, nudge, fault, and reset logic
- `main.c`: deterministic door-controller scenarios for GitHub review

## State Model

- `CLOSED`: doors closed and holding
- `OPENING`: motor drives open until open feedback is reached
- `OPEN`: doors fully open and waiting to close
- `CLOSING`: normal close attempt with obstruction reopen behavior
- `NUDGE`: buzzer-active low-force close after repeated obstructions
- `FAULT`: latched unsafe condition until reset

## Fault Model

- `LOCK_FAIL`: closed feedback arrived but the lock contact did not engage
- `MOTION_TIMEOUT`: the door failed to complete commanded motion within the allowed frames
