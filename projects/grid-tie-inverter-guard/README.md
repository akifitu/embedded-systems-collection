# Grid-Tie Inverter Guard

Portable C model of a grid-interconnect protection controller. The project
tracks mains stability, synchronizes export enable, trips on anti-islanding
events, applies thermal derating, and enforces a cooldown window before
reconnecting.

## Why It Matters

- Shows power-electronics protection logic instead of only generic MCU peripherals
- Demonstrates interconnect safety behavior such as sync windows, relay opens, and anti-islanding trips
- Maps directly to solar inverter, UPS, ESS, and microgrid controller firmware

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=boot_wait state=WAIT_GRID cmd=OPEN_RELAY reason=NONE limit=0 sync=0 v=0 f=0.000Hz relay=OPEN quality=SEARCH
phase=syncing state=SYNC cmd=TRACK_GRID reason=NONE limit=0 sync=3 v=230 f=50.000Hz relay=OPEN quality=LOCKING
phase=export state=EXPORT cmd=EXPORT_POWER reason=NONE limit=100 sync=5 v=230 f=50.000Hz relay=CLOSED quality=LOCKED
phase=thermal_derate state=EXPORT cmd=EXPORT_POWER reason=NONE limit=68 sync=5 v=230 f=50.000Hz relay=CLOSED quality=DERATED
phase=sag_trip state=TRIP cmd=OPEN_RELAY reason=UNDERVOLTAGE limit=0 sync=0 v=187 f=50.000Hz relay=OPEN quality=FAULT
phase=cooldown state=COOLDOWN cmd=HOLD_OFF reason=COOLDOWN limit=0 sync=0 v=230 f=50.000Hz relay=OPEN quality=RECOVERING
phase=relock state=EXPORT cmd=EXPORT_POWER reason=NONE limit=100 sync=5 v=230 f=50.000Hz relay=CLOSED quality=LOCKED
```

## Implementation Notes

- `grid_monitor.c` checks voltage, frequency, ROCOF, and DC-link safety
- `derating_model.c` converts heatsink stress into export power limits
- `inverter_guard.c` manages wait, sync, export, trip, cooldown, and relock behavior

## Hardware Path

Natural next step: port the controller to an STM32, dsPIC, or C2000 board with
ADC sampling, zero-cross timing capture, relay feedback GPIOs, and gate-driver
fault telemetry.
