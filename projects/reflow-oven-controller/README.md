# Reflow Oven Controller

Portable C firmware model of a lead-free reflow oven controller. The project
tracks a staged temperature profile, drives a heater duty cycle with a
PID-style loop, and applies safety interlocks for thermocouple loss, overtemp,
door-open aborts, and heater runaway.

## Why It Matters

- Shows closed-loop thermal control instead of only digital protocols
- Demonstrates staged process control with deterministic timing behavior
- Includes safe-output fallbacks that a real appliance firmware would need

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=preheat stage=PREHEAT target=150.0 temp=110.5 heater=100 fan=0 progress=7 faults=none
phase=soak stage=SOAK target=180.0 temp=175.7 heater=81 fan=0 progress=42 faults=none
phase=reflow stage=REFLOW target=235.0 temp=181.5 heater=100 fan=0 progress=65 faults=none
phase=cooldown stage=COOL target=90.0 temp=148.4 heater=0 fan=100 progress=90 faults=none
phase=complete stage=COMPLETE target=40.0 temp=79.8 heater=0 fan=25 progress=100 faults=none
phase=sensor_fault stage=FAULT target=0.0 temp=178.0 heater=0 fan=100 progress=0 faults=sensor_open
```

## Implementation Notes

- `profile_manager.c` advances through `PREHEAT`, `SOAK`, `REFLOW`, `COOL`,
  and `COMPLETE`
- `pid_controller.c` computes heater demand with stage-aware clamping
- `reflow_controller.c` latches safety faults and forces safe outputs

## Hardware Path

Natural next step: STM32 or RP2040 target with a MAX31855 or MAX6675
thermocouple frontend, an SSR heater stage, a convection fan MOSFET, and a
door switch GPIO.
