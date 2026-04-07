# Architecture

## Overview

The project models a generator autostart controller that runs once per service
frame. Each step:

1. Reads utility presence, battery voltage, engine RPM, oil pressure, fuel, and
   reset/emergency inputs
2. Decides whether a start sequence is required
3. Advances through preheat, crank, warmup, and running stages
4. Opens the contactor and cools down when utility returns
5. Trips to fault on low battery, start failure, low oil, or emergency stop

## Modules

- `generator_types.*`: enums, labels, and shared I/O structures
- `engine_guard.*`: battery, oil, and engine-start validation helpers
- `crank_manager.*`: crank-window and retry-limit helpers
- `generator_controller.*`: autostart state machine
- `main.c`: deterministic backup-power scenarios for portfolio review

## State Model

- `IDLE`: utility healthy, generator stopped
- `START_DELAY`: preheat and start-request staging
- `CRANKING`: starter motor engaged
- `WARMUP`: engine running but generator contactor still open
- `RUNNING`: generator online and contactor closed
- `COOLDOWN`: utility returned, engine cooling with contactor open
- `FAULT`: latched failure until reset

## Fault Model

- `LOW_BATTERY`: battery too weak for a safe crank attempt
- `START_FAIL`: crank retries exhausted without a valid start
- `LOW_OIL`: oil pressure unsafe while engine is running
- `E_STOP`: emergency stop input asserted
