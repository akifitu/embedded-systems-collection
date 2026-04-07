# Architecture

## Overview

The project models a pressure-targeted ventilator controller that runs once per
service frame. Each step:

1. Reads therapy enable, patient trigger, gas supply, pressure sensor, airway pressure, and flow
2. Detects hard alarm conditions before a breath can proceed
3. Advances through inhale, plateau hold, and exhale phases
4. Starts an automatic backup breath if no patient trigger arrives in time
5. Latches alarms until reset under safe conditions

## Modules

- `ventilator_types.*`: enums, labels, and shared input/output structures
- `patient_guard.*`: alarm classification and apnea-backup helper
- `breath_profile.*`: blower, valve, and target-pressure policy
- `ventilator_controller.*`: breath cycle, backup breath, alarm, and reset logic
- `main.c`: deterministic ventilator scenarios for GitHub review

## State Model

- `STANDBY`: therapy armed but no active inspiratory delivery
- `INHALE`: inspiratory valve open and pressure rising toward target
- `HOLD`: short plateau hold after inspiration
- `EXHALE`: expiratory valve open for passive exhalation
- `BACKUP`: automatic mandatory breath after apnea timeout
- `ALARM`: hard unsafe condition latched until reset

## Fault Model

- `HIGH_PRESSURE`: airway pressure exceeded the safety limit
- `DISCONNECT`: circuit appears disconnected during exhalation
- `GAS_SUPPLY_LOSS`: inspiratory gas source unavailable
- `SENSOR_FAULT`: pressure sensing is unavailable or invalid
