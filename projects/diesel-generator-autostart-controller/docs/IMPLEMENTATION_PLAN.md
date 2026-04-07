# Implementation Plan

1. Define autostart states, commands, fault labels, and shared input/output
   structures so host demos and tests can inspect deterministic behavior.
2. Implement engine guards for battery health, oil-pressure validity, and
   successful engine-start detection.
3. Add a crank manager that limits crank-window duration and retry attempts.
4. Build a controller that sequences idle, start delay, cranking, warmup,
   running, cooldown, and fault reset flow.
5. Add a deterministic demo and unit tests for utility loss, successful start,
   running transition, cooldown on utility return, low-oil trip, and reset.
