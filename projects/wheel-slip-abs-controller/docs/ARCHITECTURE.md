# Architecture

## Overview

The project models an ABS supervisor that runs once per braking frame. Each step:

1. Reads brake demand, estimated vehicle speed, and four wheel-speed channels
2. Computes per-wheel slip ratios relative to vehicle speed
3. Detects implausible wheel-sensor behavior before any control action
4. Chooses hydraulic valve commands for each wheel based on slip bands
5. Drives the controller through standby, normal braking, ABS intervention, and fault recovery

## Modules

- `abs_types.*`: enums, names, and shared input/output structures
- `slip_estimator.*`: wheel-slip calculation and sensor plausibility checks
- `pressure_modulator.*`: apply/hold/release decision policy per wheel
- `abs_controller.*`: ABS state machine and pump-control logic
- `main.c`: deterministic braking scenarios for portfolio review

## State Model

- `STANDBY`: no significant brake demand or low vehicle speed
- `BRAKING`: base braking without ABS cycling
- `ABS_ACTIVE`: slip exceeded the intervention threshold
- `FAULT`: wheel-sensor input is implausible until the driver releases the brake

## Fault Model

- `wheel_sensor`: a wheel speed is implausibly higher than vehicle speed or otherwise invalid
