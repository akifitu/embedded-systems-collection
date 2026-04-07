# Wheel-Slip ABS Controller

Portable C model of an anti-lock braking controller. The project estimates wheel
slip from vehicle and wheel speeds, modulates hydraulic valves per wheel,
enables the pump during ABS intervention, and latches a fault on implausible
wheel-sensor behavior.

## Why It Matters

- Shows closed-loop chassis control instead of only communication or monitoring logic
- Demonstrates real embedded decision flow around slip thresholds and valve actions
- Maps directly to brake ECU, traction-control, and vehicle-dynamics firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=cruise state=STANDBY pump=OFF brake=0 veh=88.0 slip=0/0/0/0 valves=HOLD/HOLD/HOLD/HOLD fault=NONE
phase=threshold state=BRAKING pump=OFF brake=34 veh=72.0 slip=6/6/5/6 valves=APPLY/APPLY/APPLY/APPLY fault=NONE
phase=split_mu state=ABS_ACTIVE pump=ON brake=76 veh=58.0 slip=24/15/10/10 valves=RELEASE/HOLD/APPLY/APPLY fault=NONE
phase=peak_control state=ABS_ACTIVE pump=ON brake=82 veh=42.0 slip=18/16/15/15 valves=HOLD/HOLD/HOLD/HOLD fault=NONE
phase=sensor_fault state=FAULT pump=OFF brake=68 veh=52.0 slip=0/0/0/0 valves=HOLD/HOLD/HOLD/HOLD fault=WHEEL_SENSOR
phase=recovery state=STANDBY pump=OFF brake=0 veh=0.0 slip=0/0/0/0 valves=HOLD/HOLD/HOLD/HOLD fault=NONE
```

## Implementation Notes

- `slip_estimator.c` derives per-wheel slip and checks for implausible sensor readings
- `pressure_modulator.c` maps slip levels into `APPLY`, `HOLD`, and `RELEASE`
- `abs_controller.c` handles standby, normal braking, ABS intervention, and fault recovery

## Hardware Path

Natural next step: port the controller to an automotive MCU with timer capture
for wheel-speed sensors, low-side valve drivers, pump current monitoring, and
hydraulic pressure feedback.
