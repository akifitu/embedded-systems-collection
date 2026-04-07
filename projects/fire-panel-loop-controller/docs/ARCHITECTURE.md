# Architecture

## Overview

The project models a fire-alarm panel loop controller that runs once per
service frame. Each step:

1. Reads smoke, manual-pull, loop supervision, silence, and reset inputs
2. Classifies active alarm and trouble conditions
3. Verifies smoke before full evacuation but bypasses verification for pull stations
4. Drives NACs, local buzzer, and annunciator LEDs from the current panel state
5. Latches alarm or trouble state until the operator performs a safe reset

## Modules

- `fire_panel_types.*`: enums, labels, and shared input/output structures
- `zone_evaluator.*`: smoke/manual-pull/trouble classification helpers
- `nac_policy.*`: NAC, buzzer, and annunciator output policy
- `fire_panel_controller.*`: verify, alarm, silence, trouble, and reset logic
- `main.c`: deterministic fire-panel scenarios for GitHub review

## State Model

- `IDLE`: normal loop monitoring with no active alarm or trouble
- `VERIFY`: smoke input present and waiting for confirmation
- `ALARM`: confirmed smoke or manual pull, NACs active
- `SILENCED`: evacuation outputs silenced but alarm still latched
- `TROUBLE`: supervised-loop or detector-maintenance issue latched until reset

## Fault Model

- `LOOP_OPEN`: supervised initiating-device loop opened or wiring failed
- `SENSOR_DIRTY`: detector maintenance threshold exceeded and service is required
