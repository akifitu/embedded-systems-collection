# Implementation Plan

1. Define panel states, commands, alarm/trouble labels, and shared I/O
   structures so tests and demos can inspect deterministic life-safety behavior.
2. Implement an input evaluator that distinguishes verified-smoke candidates,
   manual-pull alarms, and loop supervision trouble conditions.
3. Add an NAC/output policy layer that maps panel state to horns, buzzer, and
   annunciator LEDs.
4. Build a controller that performs smoke verification, alarm latching, NAC
   silence, trouble lockout, and safe reset handling.
5. Add a deterministic demo and unit tests for idle monitoring, smoke verify,
   confirmed alarm, silence, loop trouble, manual pull, and reset recovery.
