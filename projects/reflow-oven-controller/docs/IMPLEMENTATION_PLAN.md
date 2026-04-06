# Implementation Plan

1. Define oven stages, fault flags, controller outputs, and string helpers so
   demos and tests can inspect deterministic state.
2. Implement a profile manager that advances through `PREHEAT`, `SOAK`,
   `REFLOW`, `COOL`, and `COMPLETE` based on measured temperature and hold
   timing.
3. Implement a PID-style heater controller with stage-aware clamps to reduce
   overshoot near soak and reflow targets.
4. Add a top-level reflow controller that combines profile state, PID demand,
   cooling fan policy, and sticky fault handling.
5. Build a deterministic host demo with a simple thermal plant model and named
   snapshots for GitHub readers.
6. Add unit tests for profile progression, heater clamping, sensor fault
   shutdown, and heater-runaway detection.
