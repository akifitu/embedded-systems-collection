# Triplex Flight Control Voter

Portable C model of a triplex flight-control voter that compares three command
lanes, isolates a drifting or timed-out lane, continues in degraded mode with
two healthy lanes, and latches a fault on split-vote or multi-lane loss.

## Why It Matters

- Shows safety-critical redundancy handling instead of simple single-loop control
- Demonstrates majority voting, lane isolation, timeout handling, and fault latch logic
- Maps directly to aerospace, rail, and industrial safety-controller firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=standby state=IDLE cmd=HOLD_LAST_GOOD fault=NONE active=NONE isolated=NONE voted=0.0% spread=0.0% servo=OFF
phase=triplex_sync state=SYNC cmd=VOTE_OUTPUT fault=NONE active=ABC isolated=NONE voted=42.0% spread=0.3% servo=ON
phase=lane_c_isolated state=DEGRADED cmd=ISOLATE_LANE fault=LANE_C_DRIFT active=AB isolated=C voted=43.2% spread=6.0% servo=ON
phase=degraded_hold state=DEGRADED cmd=VOTE_OUTPUT fault=NONE active=AB isolated=C voted=43.5% spread=0.1% servo=ON
phase=split_vote_fault state=FAULT cmd=LATCH_FAULT fault=MULTI_LANE_FAIL active=AB isolated=C voted=43.5% spread=6.0% servo=OFF
phase=timeout_fault state=DEGRADED cmd=ISOLATE_LANE fault=LANE_TIMEOUT active=AC isolated=B voted=41.1% spread=0.2% servo=ON
phase=reset_ready state=IDLE cmd=RESET_VOTER fault=NONE active=NONE isolated=NONE voted=0.0% spread=0.0% servo=OFF
```

## Implementation Notes

- `lane_health.c` builds active and timeout lane masks and computes lane spread
- `vote_math.c` performs majority-vote math and determines isolation candidates
- `triplex_flight_control_voter.c` implements triplex, degraded, timeout, fault, and reset flow

## Hardware Path

Natural next step: port the controller to STM32, NXP, or a safety MCU with
triple redundant ADC or CAN lane inputs, watchdog-linked lane supervision,
actuator command outputs, and discrete health or maintenance annunciation.
