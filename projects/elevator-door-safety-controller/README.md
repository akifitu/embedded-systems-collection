# Elevator Door Safety Controller

Portable C model of an elevator door controller. The project handles car-door
opening at floor, passenger dwell, closing retries after obstruction, buzzered
nudge mode, and latched faults when the door fails to lock or complete motion
in time.

## Why It Matters

- Shows vertical-transport safety logic instead of only generic MCU state machines
- Demonstrates obstruction recovery, nudge mode, and door-lock supervision
- Maps directly to elevator controller, door-operator, and building-safety firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=idle_closed state=CLOSED cmd=HOLD_CLOSED fault=NONE door=CLOSED motor=STOP lock=LOCKED buzzer=OFF retry=0
phase=arrival_opening state=OPENING cmd=DRIVE_OPEN fault=NONE door=MOVING motor=OPEN lock=UNLOCKED buzzer=OFF retry=0
phase=boarding_open state=OPEN cmd=HOLD_OPEN fault=NONE door=OPEN motor=STOP lock=UNLOCKED buzzer=OFF retry=0
phase=close_attempt state=CLOSING cmd=DRIVE_CLOSE fault=NONE door=MOVING motor=CLOSE lock=UNLOCKED buzzer=OFF retry=0
phase=obstruction_retry state=OPENING cmd=DRIVE_OPEN fault=NONE door=MOVING motor=OPEN lock=UNLOCKED buzzer=OFF retry=1
phase=nudge_close state=NUDGE cmd=NUDGE_CLOSE fault=NONE door=MOVING motor=CLOSE lock=UNLOCKED buzzer=ON retry=2
phase=lock_fault state=FAULT cmd=LATCH_FAULT fault=LOCK_FAIL door=CLOSED motor=STOP lock=UNLOCKED buzzer=ON retry=0
phase=reset_ready state=CLOSED cmd=RESET_DOOR fault=NONE door=CLOSED motor=STOP lock=LOCKED buzzer=OFF retry=0
```

## Implementation Notes

- `door_monitor.c` derives open/closed/moving position from door feedbacks and enforces motion timeouts
- `obstruction_policy.c` counts retries and decides when the controller should escalate to nudge mode
- `elevator_door_controller.c` implements opening, hold-open, closing, obstruction reopen, nudge, fault latch, and reset flow

## Hardware Path

Natural next step: port the controller to STM32, AVR, or a lift-controller MCU
with door motor inverter control, safety-edge input, door position sensors,
lock-contact monitoring, buzzer output, and car-at-floor interlocks.
