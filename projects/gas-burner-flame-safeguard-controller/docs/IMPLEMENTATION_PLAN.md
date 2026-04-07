# Implementation Plan

1. Define burner states, commands, fault labels, and shared I/O structures so
   tests and demos can inspect deterministic combustion-safety behavior.
2. Implement an interlock guard that classifies stuck flame, airflow loss, and
   open limit-chain conditions.
3. Add ignition-window helpers that track prepurge, trial-for-ignition, and
   postpurge timing with simple progress outputs.
4. Build a controller that handles idle, prepurge, ignition, running,
   postpurge, hard lockout, and safe reset.
5. Add a deterministic demo and unit tests for normal startup, flame prove,
   heat-satisfied shutdown, ignition failure, interlock lockout, and recovery.
