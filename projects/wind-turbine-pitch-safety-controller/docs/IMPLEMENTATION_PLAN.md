# Implementation Plan

1. Define turbine states, commands, fault labels, and shared I/O structures so
   demos and tests can inspect deterministic pitch-safety behavior.
2. Implement a wind guard that classifies hydraulic or pitch-sensor faults and
   decides when the turbine should start, feather, or enter storm hold.
3. Add a pitch plan helper that maps controller state to blade angle, brake,
   and generator enable outputs.
4. Build a controller that handles parked startup, generating trim, grid-loss
   feathering, storm hold, latched fault, and safe reset.
5. Add a deterministic demo and unit tests for parked idle, startup,
   generating, feathering, storm hold, fault lockout, and recovery.
