# Architecture

## Overview

The project models a wind-turbine pitch safety controller that runs once per
service frame. Each step:

1. Reads start intent, grid availability, wind speed, rotor speed, and pitch-system health
2. Detects hydraulic or pitch-sensor faults before power production can continue
3. Chooses between parked, startup, generating, feathering, storm hold, and fault states
4. Maps the chosen state into blade angle, brake, and generator contactor outputs
5. Latches hard pitch-system faults until reset under safe parked conditions

## Modules

- `turbine_types.*`: enums, labels, and shared input/output structures
- `wind_guard.*`: start permissives, storm thresholds, feather decisions, and fault checks
- `pitch_plan.*`: blade-angle, brake, and generator output policy
- `wind_turbine_controller.*`: main turbine state machine and reset handling
- `main.c`: deterministic turbine scenarios for GitHub review

## State Model

- `PARKED`: blades feathered, brake on, generator disconnected
- `STARTUP`: blades released from park and aligned for rotor acceleration
- `GENERATING`: normal power production with pitch trim
- `FEATHERING`: generator removed and blades driven toward feather to slow the rotor
- `STORM_HOLD`: turbine parked during excessive wind until conditions improve
- `FAULT`: hard unsafe condition latched until reset

## Fault Model

- `HYDRAULIC_LOW`: pitch actuator hydraulic pressure is insufficient
- `PITCH_SENSOR_MISMATCH`: pitch position feedback is not trustworthy
