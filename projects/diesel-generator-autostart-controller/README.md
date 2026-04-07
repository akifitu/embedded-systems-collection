# Diesel Generator Autostart Controller

Portable C model of a standby-generator autostart controller. The project
detects utility loss, performs preheat and crank sequencing, validates engine
start via RPM and oil pressure, closes the generator contactor after warmup,
and handles cooldown or fault shutdown paths.

## Why It Matters

- Shows real embedded backup-power sequencing instead of only passive monitoring
- Demonstrates crank retry, warmup, low-oil fault, and cooldown stop behavior
- Maps directly to ATS, genset controller, and industrial power-management firmware

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=utility_ok state=IDLE cmd=OPEN_CONTACTOR utility=ON rpm=0 batt=25.4V tries=0 contactor=OPEN fault=NONE
phase=outage_detected state=START_DELAY cmd=PREHEAT utility=OFF rpm=0 batt=25.1V tries=0 contactor=OPEN fault=NONE
phase=cranking state=CRANKING cmd=CRANK_START utility=OFF rpm=280 batt=24.6V tries=1 contactor=OPEN fault=NONE
phase=warmup state=WARMUP cmd=OPEN_CONTACTOR utility=OFF rpm=1450 batt=24.8V tries=1 contactor=OPEN fault=NONE
phase=running state=RUNNING cmd=CLOSE_CONTACTOR utility=OFF rpm=1500 batt=25.0V tries=1 contactor=CLOSED fault=NONE
phase=utility_restore state=COOLDOWN cmd=COOL_ENGINE utility=ON rpm=1490 batt=25.1V tries=1 contactor=OPEN fault=NONE
phase=low_oil_fault state=FAULT cmd=STOP_ENGINE utility=OFF rpm=1460 batt=24.9V tries=1 contactor=OPEN fault=LOW_OIL
phase=reset_ready state=IDLE cmd=OPEN_CONTACTOR utility=ON rpm=0 batt=25.3V tries=0 contactor=OPEN fault=NONE
```

## Implementation Notes

- `engine_guard.c` evaluates start success, low battery, and oil-pressure safety
- `crank_manager.c` limits crank duration and start attempts
- `generator_controller.c` manages idle, start delay, cranking, warmup, running, cooldown, and fault states

## Hardware Path

Natural next step: port the controller to an STM32, AVR, or PLC-adjacent MCU
with mains-loss sensing, starter relay drive, oil-pressure input, battery
measurement, and ATS interlock feedback.
