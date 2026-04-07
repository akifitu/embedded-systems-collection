# Architecture

## Overview

The project models a satellite electrical power system controller that runs once
per service frame. Each step:

1. Reads sunlight state, battery SoC, bus voltage, solar input, and payload demand
2. Detects undervoltage or deep-discharge faults
3. Decides between nominal, shed, survival, and recovery modes
4. Maps the chosen mode into enabled rails and subsystem budgets
5. Latches hard faults until reset under safe electrical conditions

## Modules

- `eps_types.*`: enums, labels, and shared input/output structures
- `battery_guard.*`: fault thresholds and shed or survival decision helpers
- `load_plan.*`: payload, heater, radio, and ADCS power policy
- `satellite_eps_controller.*`: nominal, shed, survival, recovery, fault, and reset logic
- `main.c`: deterministic spacecraft EPS scenarios for GitHub review

## State Model

- `NOMINAL`: all planned loads available
- `SHED`: payload and noncritical loads removed to preserve bus margin
- `SURVIVAL`: only safe-mode avionics and beacon-class communications remain
- `RECOVERY`: sunlight returned and the controller is rebuilding margin before full restore
- `FAULT`: electrical condition unsafe until explicit reset

## Fault Model

- `BUS_UNDERVOLTAGE`: main power bus dropped below safe operating range
- `BATTERY_DEEP_DISCHARGE`: battery SoC fell to a damaging reserve level
