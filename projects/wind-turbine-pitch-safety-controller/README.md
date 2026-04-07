# Wind Turbine Pitch Safety Controller

Portable C model of a wind-turbine pitch and shutdown controller. The project
handles parked startup, power-producing pitch trim, grid-loss feathering,
storm-hold parking, and hard fault lockout when hydraulic or pitch-sensor
conditions become unsafe.

## Why It Matters

- Shows renewable-energy safety logic instead of only generic control loops
- Demonstrates feathering, storm hold, and latched pitch-system faults
- Maps directly to turbine controller, pitch-drive, and nacelle safety firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=parked state=PARKED cmd=HOLD_PARK fault=NONE wind=4.0mps rpm=0 pitch=90deg gen=OFF brake=ON
phase=startup state=STARTUP cmd=STARTUP_RELEASE fault=NONE wind=9.0mps rpm=220 pitch=18deg gen=OFF brake=OFF
phase=generating state=GENERATING cmd=TRACK_POWER fault=NONE wind=12.0mps rpm=1480 pitch=14deg gen=ON brake=OFF
phase=grid_trip_feather state=FEATHERING cmd=FEATHER_BLADES fault=NONE wind=13.0mps rpm=1320 pitch=82deg gen=OFF brake=OFF
phase=storm_hold state=STORM_HOLD cmd=HOLD_STORM fault=NONE wind=28.0mps rpm=140 pitch=88deg gen=OFF brake=ON
phase=hydraulic_fault state=FAULT cmd=LATCH_FAULT fault=HYDRAULIC_LOW wind=11.0mps rpm=1300 pitch=90deg gen=OFF brake=ON
phase=reset_ready state=PARKED cmd=RESET_TURBINE fault=NONE wind=7.0mps rpm=0 pitch=90deg gen=OFF brake=ON
```

## Implementation Notes

- `wind_guard.c` decides start permissives, storm-hold thresholds, feather conditions, and hard pitch-system faults
- `pitch_plan.c` maps each state into deterministic blade angle, brake, and generator outputs
- `wind_turbine_controller.c` implements parked, startup, generating, feathering, storm-hold, fault, and reset flow

## Hardware Path

Natural next step: port the controller to an STM32, TI C2000, or industrial
pitch controller with rotor-speed capture, hydraulic-pressure input, grid
contact feedback, pitch encoder channels, brake relay drive, and feather relay
outputs.
