# Capacitive Touch Keypad Controller

Portable C model of a capacitive touch keypad controller. The project tracks
per-key baselines, debounces touch decisions, emits tap/hold/combo events, and
suppresses false touches during moisture or flood conditions.

## Why It Matters

- Shows embedded HMI signal conditioning instead of only comms and control loops
- Demonstrates baseline drift handling, debounce, and moisture rejection
- Maps directly to STM32 TSC, Microchip PTC, ESP32 touch, and similar peripherals

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=idle active=NONE event=NONE moisture=DRY faults=none baselines=1000/1004/998/1002
phase=tap_ok active=NONE event=TAP_OK moisture=DRY faults=none baselines=1000/1004/998/1002
phase=hold_back active=BACK event=HOLD_BACK moisture=DRY faults=none baselines=1000/1004/998/1002
phase=combo active=NONE event=COMBO_UP_OK moisture=DRY faults=none baselines=1000/1004/998/1002
phase=moisture active=NONE event=NONE moisture=WET faults=moisture baselines=1000/1004/998/1002
phase=recovery active=NONE event=TAP_DOWN moisture=DRY faults=none baselines=1000/1004/998/1002
```

## Implementation Notes

- `baseline_tracker.c` updates per-key baselines while the keypad is idle
- `keypad_controller.c` handles debounce, hold/combo events, and moisture guard
- `main.c` drives deterministic touch scenarios that are easy to inspect on GitHub

## Hardware Path

Natural next step: bind the controller to a real touch peripheral and electrode
layout, then tune thresholds against panel material and moisture exposure.
