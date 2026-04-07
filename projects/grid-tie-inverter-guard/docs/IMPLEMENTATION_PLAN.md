# Implementation Plan

1. Define guard states, export commands, fault reasons, and structured
   input/output types so deterministic host demos can inspect protection logic.
2. Implement a grid monitor that evaluates voltage, frequency, ROCOF, and
   DC-link safety windows for sync and trip decisions.
3. Add a derating model that reduces export power under rising heatsink
   temperature before a hard thermal trip occurs.
4. Build a guard controller that advances from waiting to sync, enables export,
   trips on anti-islanding faults, and enforces cooldown before reconnection.
5. Add a deterministic demo and unit tests for sync lock, derating, undervoltage
   trip, relay feedback fault, and cooldown recovery.
