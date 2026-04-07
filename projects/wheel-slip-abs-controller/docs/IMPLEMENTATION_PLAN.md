# Implementation Plan

1. Define controller states, valve actions, fault labels, and shared
   input/output structures so demos and tests can inspect deterministic ABS behavior.
2. Implement a slip estimator that compares vehicle speed and wheel speeds while
   rejecting implausible wheel-sensor readings.
3. Add a pressure-modulation policy that maps slip bands into apply, hold, and
   release decisions for each wheel.
4. Build an ABS controller that switches between standby, base braking, active
   ABS intervention, and a latched sensor-fault state with recovery.
5. Add a deterministic demo and unit tests for cruise, threshold braking,
   split-mu ABS activation, sensor-fault trip, and fault-clear recovery.
