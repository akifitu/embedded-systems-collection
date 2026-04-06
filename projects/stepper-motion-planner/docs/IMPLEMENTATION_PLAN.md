# Implementation Plan

1. Define motion states, directions, fault flags, commands, and text helpers so
   demos and tests can inspect deterministic controller output.
2. Implement a trajectory planner that derives triangular or trapezoidal
   profiles from distance, max velocity, and acceleration.
3. Build a top-level stepper controller with homing, move execution, limit
   handling, target timeout, and stall fault logic.
4. Add a deterministic host demo that simulates home-seek, nominal move,
   limit-abort, and rehome recovery.
5. Add unit tests for profile generation, homing zeroing, nominal move
   completion, limit switch faulting, and stall detection.
