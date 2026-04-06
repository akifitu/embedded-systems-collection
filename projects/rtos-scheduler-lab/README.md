# RTOS Scheduler Lab

Host-buildable real-time scheduling simulator for embedded task sets.

Docs:
- [Implementation Plan](docs/IMPLEMENTATION_PLAN.md)
- [Architecture Notes](docs/ARCHITECTURE.md)

## Features

- Fixed-priority preemptive scheduling
- Periodic task release and deadline tracking
- Watchdog starvation detection for critical control tasks
- Deterministic overload and recovery scenarios

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=nominal idle=25 misses=0 watchdog=OK starved=none last=control
phase=radio_burst idle=10 misses=0 watchdog=OK starved=none last=control
phase=sensor_stall idle=0 misses=14 watchdog=TRIPPED starved=control last=sensor
phase=recovery idle=22 misses=0 watchdog=OK starved=none last=control
```

## Future Extensions

- True EDF scheduling comparison
- FreeRTOS trace exporter
- Priority inheritance experiment
- ISR release jitter injection
