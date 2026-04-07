# Implementation Plan

1. Define crossing states, commands, fault labels, and shared input/output
   structures so demos and tests can inspect deterministic warning behavior.
2. Implement a train-zone helper that distinguishes clear track, approach, and island occupancy.
3. Add a gate monitor that interprets barrier feedback and enforces motion timeout checks.
4. Build a controller that sequences prewarning, barrier lowering, protected hold,
   raising, and fault lockout for lamp or gate failures.
5. Add a deterministic demo and unit tests for idle, approach warning, lowering,
   protected occupancy, lamp fault, gate timeout, and reset recovery.
