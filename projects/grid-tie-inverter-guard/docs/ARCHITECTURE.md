# Architecture

## Overview

The project models a grid-tie inverter protection loop that runs once per
control frame. Each step:

1. Reads grid-present status, RMS voltage, grid frequency, ROCOF, heatsink
   temperature, DC-link voltage, and relay feedback
2. Checks whether the grid is healthy enough to begin or maintain synchronization
3. Derates export power when thermal stress rises but safety margins still exist
4. Trips immediately on anti-islanding or hardware-protection conditions
5. Waits through a cooldown window before re-entering sync and export

## Modules

- `inverter_guard_types.*`: enums, labels, and shared I/O structures
- `grid_monitor.*`: sync window and fault-threshold evaluation
- `derating_model.*`: export power limit estimate from thermal stress
- `inverter_guard.*`: wait, sync, export, trip, and cooldown state machine
- `main.c`: deterministic interconnect scenarios for portfolio review

## State Model

- `WAIT_GRID`: relay open, waiting for a healthy grid
- `SYNC`: the controller is measuring a clean sync window
- `EXPORT`: relay closed and inverter export enabled
- `TRIP`: fault detected, relay must open immediately
- `COOLDOWN`: holdoff period before the next sync attempt

## Fault Model

- `grid_loss`: the grid disappeared during sync or export
- `undervoltage` and `overvoltage`: mains amplitude left safe bounds
- `underfrequency` and `overfrequency`: grid frequency left safe bounds
- `rocof`: rate-of-change of frequency suggests islanding or an unstable grid
- `overtemp`: heatsink temperature requires an immediate shutdown
- `dc_link_high`: bus voltage exceeds a safe limit
- `relay_fault`: export state does not match relay feedback
