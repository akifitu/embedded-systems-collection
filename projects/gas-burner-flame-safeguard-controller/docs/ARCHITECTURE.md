# Architecture

## Overview

The project models a burner flame-safeguard controller that runs once per
service frame. Each step:

1. Reads the heat call, airflow proof, flame sensor, and limit-chain status
2. Validates safety interlocks before fuel can be admitted
3. Executes a prepurge before opening the gas valve
4. Enforces a bounded trial-for-ignition window to prove flame
5. Performs postpurge after heat demand ends or latches a lockout on unsafe events

## Modules

- `burner_types.*`: enums, labels, and shared input/output structures
- `interlock_guard.*`: stuck-flame, airflow, and limit-chain fault helpers
- `ignition_window.*`: prepurge, ignition, and postpurge timing helpers
- `gas_burner_controller.*`: main burner state machine and lockout handling
- `main.c`: deterministic burner scenarios for GitHub review

## State Model

- `IDLE`: outputs off and waiting for a safe heat request
- `PREPURGE`: blower active to clear the combustion chamber
- `IGNITION`: gas valve and igniter active while proving flame
- `RUNNING`: flame proven and burner firing normally
- `POSTPURGE`: blower continues after fuel shutoff
- `LOCKOUT`: unsafe condition latched until manual reset

## Fault Model

- `AIRFLOW_FAIL`: airflow proof disappeared during an active burner phase
- `IGNITION_FAIL`: flame was not proven before the ignition window expired
- `INTERLOCK_OPEN`: a safety limit opened during startup or run
- `FLAME_STUCK`: flame signal was present before a start sequence
