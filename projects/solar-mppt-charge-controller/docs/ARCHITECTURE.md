# Architecture

## Overview

The project models a solar MPPT charge controller that runs once per service
frame. Each step:

1. Reads panel voltage and current, battery voltage, temperature, and charge-enable intent
2. Detects reverse-polarity, battery overvoltage, and charger overtemperature faults
3. Updates the MPPT duty command from the latest panel power sample
4. Chooses between idle, bulk, absorb, float, or fault states
5. Maps the chosen state into relay, fan, and target-voltage outputs

## Modules

- `solar_types.*`: enums, labels, and shared input/output structures
- `power_tracker.*`: perturb-and-observe duty update helper
- `charge_policy.*`: relay, fan, and voltage-target policy
- `solar_mppt_controller.*`: bulk, absorb, float, fault, and reset logic
- `main.c`: deterministic solar-charger scenarios for GitHub review

## State Model

- `IDLE`: charger disabled or panel power too low to charge
- `BULK`: MPPT tracking active while battery rises toward absorb voltage
- `ABSORB`: charger holds absorb target before tapering into float
- `FLOAT`: maintenance charging at a lower voltage target
- `FAULT`: unsafe electrical or thermal condition latched until reset

## Fault Model

- `BATTERY_OVERVOLT`: battery exceeded the safe charging ceiling
- `PANEL_REVERSE`: reverse polarity or panel protection fault detected
- `OVER_TEMP`: charger thermal limit exceeded
