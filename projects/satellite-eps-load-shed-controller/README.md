# Satellite EPS Load Shed Controller

Portable C model of a small-satellite electrical power system controller. The
project watches eclipse/daylight conditions, battery state-of-charge, bus
voltage, and array power to shed noncritical loads, enter survival mode, and
restore loads after recharge.

## Why It Matters

- Shows spacecraft EPS autonomy instead of only terrestrial control loops
- Demonstrates staged load shedding, eclipse survival, and recovery sequencing
- Maps directly to cubesat avionics, power-board, and fault-management firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=nominal_daylight state=NOMINAL cmd=HOLD_FULL_POWER fault=NONE sun=YES soc=78 bus=7600mV payload=ON heater=ON radio=HIGH adcs=FULL budget=60W
phase=eclipse_shed state=SHED cmd=SHED_NONCRITICAL fault=NONE sun=NO soc=42 bus=7280mV payload=OFF heater=OFF radio=BEACON adcs=FULL budget=16W
phase=deep_eclipse state=SURVIVAL cmd=ENTER_SURVIVAL fault=NONE sun=NO soc=16 bus=6940mV payload=OFF heater=OFF radio=BEACON adcs=SAFE budget=8W
phase=recharge_recovery state=RECOVERY cmd=RESTORE_LOADS fault=NONE sun=YES soc=34 bus=7420mV payload=OFF heater=ON radio=BEACON adcs=FULL budget=24W
phase=nominal_restored state=NOMINAL cmd=HOLD_FULL_POWER fault=NONE sun=YES soc=62 bus=7580mV payload=ON heater=OFF radio=HIGH adcs=FULL budget=58W
phase=undervoltage_fault state=FAULT cmd=LATCH_FAULT fault=BUS_UNDERVOLTAGE sun=NO soc=18 bus=6480mV payload=OFF heater=OFF radio=BEACON adcs=SAFE budget=5W
phase=reset_ready state=NOMINAL cmd=RESET_EPS fault=NONE sun=YES soc=66 bus=7620mV payload=ON heater=OFF radio=HIGH adcs=FULL budget=56W
```

## Implementation Notes

- `battery_guard.c` classifies undervoltage and deep-discharge faults and suggests shed or survival transitions
- `load_plan.c` maps each EPS state into payload, heater, radio, and ADCS power budgets
- `satellite_eps_controller.c` implements nominal, shed, survival, recovery, fault, and reset flow

## Hardware Path

Natural next step: port the controller to STM32, MSP430, or radiation-tolerant
avionics hardware with battery telemetry ADCs, solar regulator measurements,
switched payload rails, heater drivers, and watchdog-tied safe-mode recovery.
