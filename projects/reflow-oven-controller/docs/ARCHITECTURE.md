# Architecture

## Overview

The project models a small appliance firmware loop that runs once per second.
Each step:

1. Reads the thermocouple sample and interlock inputs
2. Updates the active reflow stage from the process profile
3. Computes heater demand with a PID-style controller
4. Latches any safety fault and forces safe outputs
5. Exposes a compact state snapshot for the host demo or tests

## Modules

- `reflow_types.*`: shared enums, fault flags, and text helpers
- `profile_manager.*`: stage transitions and progress tracking
- `pid_controller.*`: heater demand control with anti-windup
- `reflow_controller.*`: fault supervision and top-level output synthesis
- `main.c`: deterministic thermal plant simulation and demo snapshots

## Control Strategy

- `PREHEAT`: rise quickly toward 150 C
- `SOAK`: stabilize near 180 C to reduce thermal shock
- `REFLOW`: climb toward 235 C and hold briefly above liquidus
- `COOL`: disable the heater and run the fan to exit the process safely

## Safety Model

- `SENSOR_OPEN`: thermocouple is invalid
- `OVERTEMP`: chamber exceeds the hard safety ceiling
- `RUNAWAY`: strong heater demand persists without enough temperature rise
- `DOOR_OPEN`: active cycle interrupted by an interlock event

Any latched fault drives the controller to `FAULT`, disables the heater, and
applies a conservative fan policy until temperature falls.
