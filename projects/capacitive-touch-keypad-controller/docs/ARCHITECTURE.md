# Architecture

## Overview

The project models a four-key capacitive touch keypad running on a fixed scan
loop. Each scan:

1. Reads raw key counts and a shield or flood indicator
2. Updates per-key baselines when the panel is idle and dry
3. Computes touch deltas against thresholds
4. Debounces candidate touches and emits tap, hold, or combo events
5. Suppresses touches if moisture or flooding is detected

## Modules

- `touch_types.*`: key masks, events, moisture state, and formatting helpers
- `baseline_tracker.*`: baseline maintenance and delta extraction
- `keypad_controller.*`: debounce, hold, combo, and moisture guard logic
- `main.c`: deterministic keypad scenarios

## Event Model

- Single-key release after a short stable press produces `TAP_*`
- Sustained stable press produces `HOLD_*`
- Simultaneous `UP + OK` produces `COMBO_UP_OK`

## Fault Model

- `moisture`: flood or shield event blocks touch decisions until the panel dries
