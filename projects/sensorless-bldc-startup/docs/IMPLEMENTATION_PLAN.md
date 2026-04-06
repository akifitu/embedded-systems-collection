# Implementation Plan

1. Define BLDC states, commutation sectors, lock status, fault flags, and text
   helpers so demos and tests can inspect deterministic controller output.
2. Implement a commutation helper that advances the six-step table and converts
   between commutation period and estimated mechanical speed.
3. Build a startup controller with rotor alignment, open-loop ramp, zero-cross
   lock acquisition, closed-loop tracking, and fault handling.
4. Add a deterministic host demo for nominal startup, load response,
   overcurrent, and no-lock failure.
5. Add unit tests for state transitions, lock acquisition, no-lock timeout,
   overcurrent shutdown, and stall detection after lock.
