# Gas Burner Flame Safeguard Controller

Portable C model of an industrial burner flame-safeguard controller. The
project handles prepurge, trial-for-ignition, proven-flame run, postpurge, and
hard lockout when airflow, limit, or flame conditions become unsafe.

## Why It Matters

- Shows combustion-safety sequencing instead of only generic state machines
- Demonstrates purge timing, ignition-prove windows, and lockout behavior
- Maps directly to boiler, furnace, and burner-management firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=idle state=IDLE cmd=HOLD_OFF fault=NONE fan=OFF igniter=OFF valve=CLOSED flame=NO progress=0
phase=prepurge state=PREPURGE cmd=START_PREPURGE fault=NONE fan=ON igniter=OFF valve=CLOSED flame=NO progress=50
phase=ignition_trial state=IGNITION cmd=TRIAL_IGNITE fault=NONE fan=ON igniter=ON valve=OPEN flame=NO progress=50
phase=flame_proven state=RUNNING cmd=HOLD_FLAME fault=NONE fan=ON igniter=OFF valve=OPEN flame=YES progress=100
phase=postpurge state=POSTPURGE cmd=POSTPURGE_FAN fault=NONE fan=ON igniter=OFF valve=CLOSED flame=NO progress=50
phase=ignition_fail state=LOCKOUT cmd=LATCH_LOCKOUT fault=IGNITION_FAIL fan=OFF igniter=OFF valve=CLOSED flame=NO progress=0
phase=reset_ready state=IDLE cmd=RESET_BURNER fault=NONE fan=OFF igniter=OFF valve=CLOSED flame=NO progress=0
```

## Implementation Notes

- `interlock_guard.c` prioritizes stuck-flame, airflow, and limit faults
- `ignition_window.c` provides deterministic purge and ignition timing helpers
- `gas_burner_controller.c` implements prepurge, ignition, running, postpurge, lockout, and reset flow

## Hardware Path

Natural next step: port the controller to STM32, AVR, or a burner-management
MCU with blower relay drive, flame-rod or UV scanner input, gas valve outputs,
airflow proving switch, high-limit chain, and lockout annunciation.
