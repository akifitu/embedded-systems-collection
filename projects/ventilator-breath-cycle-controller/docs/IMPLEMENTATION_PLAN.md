# Implementation Plan

1. Define ventilator states, commands, fault labels, and shared I/O structures
   so demos and tests can inspect deterministic breath-cycle behavior.
2. Implement a patient guard that classifies gas-supply loss, sensor failure,
   high-pressure, disconnect, and apnea-backup conditions.
3. Add a breath-profile helper that maps each state to blower, valve, and
   target-pressure outputs.
4. Build a controller that handles standby, inhale, plateau hold, exhale,
   backup ventilation, hard alarm latch, and safe reset.
5. Add a deterministic demo and unit tests for patient-triggered inspiration,
   exhale, backup breath, alarm conditions, and recovery.
