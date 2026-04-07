# Implementation Plan

1. Define EPS states, commands, fault labels, and shared input/output
   structures so tests and demos can inspect deterministic power decisions.
2. Implement a battery guard that flags bus undervoltage or deep discharge and
   suggests when the controller should shed loads or enter survival mode.
3. Add a load-plan helper that maps each EPS state to payload, heater, radio,
   and ADCS availability.
4. Build a controller that handles nominal operation, shed mode, survival,
   recovery, latched faults, and safe reset.
5. Add a deterministic demo and unit tests for daylight nominal power, eclipse
   shedding, deep-eclipse survival, recharge recovery, undervoltage fault, and
   fault reset.
