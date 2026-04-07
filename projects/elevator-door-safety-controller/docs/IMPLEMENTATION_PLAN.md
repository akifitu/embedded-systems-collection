# Implementation Plan

1. Define door states, commands, fault labels, and shared I/O structures so
   demos and tests can inspect deterministic elevator-door behavior.
2. Implement a door monitor that classifies open, closed, or moving feedback
   and flags excessive motion duration.
3. Add an obstruction policy helper that records retries and decides when to
   switch from normal close to nudge mode.
4. Build a controller that handles closed, opening, open hold, closing, nudge,
   lock-fault, motion-timeout, and reset behavior.
5. Add a deterministic demo and unit tests for normal opening/closing,
   obstruction retry, nudge escalation, lock fault, timeout fault, and recovery.
