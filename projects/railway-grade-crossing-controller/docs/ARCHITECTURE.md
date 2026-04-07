# Architecture

## Overview

The project models a railway grade-crossing controller that runs once per
service frame. Each step:

1. Reads train approach, island occupancy, gate feedback, lamp health, and reset input
2. Derives the active train zone and current barrier position
3. Starts prewarning before lowering the gate
4. Holds the crossing protected while the train occupies the crossing
5. Raises the gate after the route clears or trips to fault on unsafe conditions

## Modules

- `crossing_types.*`: enums, labels, and shared I/O structures
- `train_zone.*`: clear/approach/island zone derivation
- `gate_monitor.*`: gate position and motion-timeout helpers
- `grade_crossing_controller.*`: prewarn, lowering, protected, raising, and fault logic
- `main.c`: deterministic rail-crossing scenarios for portfolio review

## State Model

- `IDLE`: no train activity, gate up
- `PREWARN`: flashers and bell active before gate lowering
- `LOWERING`: gate commanded down and waiting for down feedback
- `PROTECTED`: gate down while the train is approaching or on the island
- `RAISING`: route clear and gate commanded back up
- `FAULT`: latched unsafe condition until reset

## Fault Model

- `LAMP_FAIL`: warning lamp supervision failed while the crossing should be active
- `GATE_TIMEOUT`: the barrier did not complete a commanded move in time
