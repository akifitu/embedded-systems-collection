# Implementation Plan

1. Define charger states, commands, fault labels, and shared I/O structures so
   demos and tests can inspect deterministic MPPT and charge-stage behavior.
2. Implement a power tracker that adjusts PWM duty from panel power changes
   using a bounded perturb-and-observe rule.
3. Add a charge-policy helper that maps bulk, absorb, float, and fault states
   to relay, fan, and target-voltage outputs.
4. Build a controller that handles idle, bulk MPPT, absorb, float, hard fault,
   and safe reset.
5. Add a deterministic demo and unit tests for dawn idle, bulk tracking,
   absorb transition, float maintenance, thermal fault, and recovery.
