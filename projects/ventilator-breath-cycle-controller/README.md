# Ventilator Breath Cycle Controller

Portable C model of a pressure-targeted ventilator controller. The project
handles patient-triggered inspiration, plateau hold, exhalation, apnea backup
breaths, and hard alarms on high pressure, circuit disconnect, gas loss, or
sensor failure.

## Why It Matters

- Shows medical respiratory control instead of only generic state machines
- Demonstrates breath-cycle timing, backup ventilation, and patient-safety alarms
- Maps directly to ventilator, anesthesia, and airway-support firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=standby state=STANDBY cmd=HOLD_STANDBY fault=NONE mode=SUPPORT blower=0 in_valve=CLOSED ex_valve=OPEN target=0 pressure=5 flow=12
phase=patient_trigger state=INHALE cmd=DELIVER_INHALE fault=NONE mode=SUPPORT blower=68 in_valve=OPEN ex_valve=CLOSED target=18 pressure=12 flow=36
phase=plateau state=HOLD cmd=HOLD_PLATEAU fault=NONE mode=SUPPORT blower=28 in_valve=CLOSED ex_valve=CLOSED target=18 pressure=18 flow=4
phase=exhale state=EXHALE cmd=VENT_EXHALE fault=NONE mode=SUPPORT blower=0 in_valve=CLOSED ex_valve=OPEN target=5 pressure=8 flow=24
phase=backup_breath state=BACKUP cmd=START_BACKUP fault=NONE mode=BACKUP blower=72 in_valve=OPEN ex_valve=CLOSED target=20 pressure=10 flow=32
phase=disconnect_alarm state=ALARM cmd=LATCH_ALARM fault=DISCONNECT mode=SUPPORT blower=0 in_valve=CLOSED ex_valve=OPEN target=0 pressure=3 flow=0
phase=reset_ready state=STANDBY cmd=RESET_VENT fault=NONE mode=SUPPORT blower=0 in_valve=CLOSED ex_valve=OPEN target=0 pressure=5 flow=10
```

## Implementation Notes

- `patient_guard.c` prioritizes gas loss, sensor failure, high-pressure, and disconnect alarms
- `breath_profile.c` maps each breath state into blower, inspiratory valve, expiratory valve, and target pressure outputs
- `ventilator_controller.c` implements standby, inhale, hold, exhale, backup breath, alarm latch, and reset flow

## Hardware Path

Natural next step: port the controller to STM32, NXP, or medical-control
hardware with blower PWM drive, inspiratory and expiratory valve outputs,
airway pressure sensing, flow sensor inputs, and oxygen or gas-supply
supervision.
