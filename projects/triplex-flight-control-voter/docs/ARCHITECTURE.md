# Architecture

## Overview

The project models a triplex voter in front of a flight-control actuator. Three
command lanes are compared every cycle. If all three agree within tolerance,
the actuator receives the voted command. If one lane drifts or times out, it is
isolated and the controller continues in degraded mode with the remaining two
lanes. If the remaining lanes disagree or too many lanes are lost, the voter
holds the last good command and latches a fault.

## Modules

- `triplex_flight_control_voter.c`: top-level state machine and fault handling
- `lane_health.c`: active-mask, timeout-mask, lane counting, and spread helpers
- `vote_math.c`: majority-vote and isolation logic
- `main.c`: deterministic scenario runner for portfolio snapshots
- `tests/test_triplex_voter.c`: regression coverage for nominal and degraded paths

## State Flow

1. `IDLE`: no actuator authority, waiting for enable.
2. `SYNC`: all three lanes healthy and inside vote tolerance.
3. `DEGRADED`: one lane isolated, authority maintained from a matching pair.
4. `FAULT`: last good command held internally, actuator output disabled until reset.

## Safety Decisions

- Single-lane drift triggers isolation rather than immediate system loss.
- Lane timeouts can degrade safely only if two healthy lanes remain.
- Dual disagreement after degradation becomes a latched fault.
- Reset is only allowed with control disabled, preventing unsafe in-flight re-entry.
