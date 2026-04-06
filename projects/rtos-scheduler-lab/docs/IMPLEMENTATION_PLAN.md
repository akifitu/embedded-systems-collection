# Implementation Plan

## Goal

Build a compact RTOS-style scheduler model that shows how periodic embedded
tasks are released, selected by priority, monitored for deadline misses, and
guarded by a watchdog on a critical control task.

## Step By Step

1. Define a periodic task model with period, relative deadline, WCET, and fixed priority.
2. Implement a scheduler tick that releases jobs and selects the highest-priority ready task.
3. Track per-job remaining execution and deadline misses.
4. Track time since the control task last ran and trip a watchdog if starvation exceeds a threshold.
5. Build deterministic task sets for:
   - nominal load
   - telemetry burst
   - sensor stall overload
   - recovery
6. Emit a compact summary of idle time, missed deadlines, watchdog state, and last-running task.
7. Lock behavior with unit tests before pushing.

## Embedded Value

- Shows real-time scheduling knowledge beyond generic main loops
- Makes overload behavior explicit and measurable
- Maps directly to FreeRTOS, Zephyr, or custom scheduler work

