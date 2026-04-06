# Implementation Plan

1. Define key masks, event enums, moisture state, and formatting helpers so
   demos and tests can inspect deterministic touch behavior.
2. Implement a baseline tracker that adapts slowly when keys are idle and dry.
3. Build a keypad controller that debounces presses, emits tap and hold events,
   recognizes a combo gesture, and blocks touch decisions under moisture.
4. Add a deterministic host demo for idle, tap, hold, combo, moisture fault,
   and recovery.
5. Add unit tests for baseline tracking, tap detection, hold detection, combo
   detection, and moisture suppression.
