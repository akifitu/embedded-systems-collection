# Embedded Systems Portfolio Lab

[![CI](https://github.com/akifitu/embedded-systems-portfolio-lab/actions/workflows/ci.yml/badge.svg)](https://github.com/akifitu/embedded-systems-portfolio-lab/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-0f766e.svg)](LICENSE)
[![Language: C](https://img.shields.io/badge/language-C-1d4ed8.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

This repository is an embedded-systems portfolio designed to look credible on
GitHub before any real board is connected. The code is written in portable C,
builds on the host with a standard compiler, and models the kind of firmware
problems that show up in production teams.

## Portfolio Signal

- Safety and control logic through a battery-management state machine
- Reliability and upgrade strategy through an A/B OTA bootloader model
- Bus communication literacy through a CAN telemetry scheduler
- Embedded diagnostics and fixed-point friendly DSP through a motor monitor
- Power-fail-safe persistence through a wear-aware flash journal
- Low-power duty cycling and energy budgeting through a harvesting node controller
- Industrial protocol handling through a Modbus RTU field device
- Firmware measurement and device identity proof through a secure attestation node
- Fixed-priority real-time scheduling through an RTOS scheduler laboratory
- Repeatability through `make test` and a GitHub Actions CI pipeline

## System Map

```mermaid
flowchart LR
    Host[Host Build and Tests] --> BMS[Smart BMS Firmware]
    Host --> OTA[OTA Bootloader Simulator]
    Host --> CAN[CAN Telemetry Node]
    Host --> MCM[Motor Condition Monitor]
    Host --> RFJ[Resilient Flash Journal]
    Host --> EHN[Energy Harvesting Node]
    Host --> MB[Modbus RTU Field Node]
    Host --> SAN[Secure Attestation Node]
    Host --> RTOS[RTOS Scheduler Lab]
    BMS --> Safety[Fault Detection and SoC]
    OTA --> Reliability[CRC32, Trial Boot, Rollback]
    CAN --> VehicleBus[Periodic and Fault CAN Frames]
    MCM --> Predictive[Window Features and Fault Events]
    RFJ --> Persistence[Recovery Scan and Wear Tracking]
    EHN --> Power[Task Selection and Deep Sleep]
    MB --> Industrial[Register Map and Exception Frames]
    SAN --> Trust[Measurement Check and HMAC Token]
    RTOS --> Timing[Priority Scheduling and Watchdog]
```

## Projects

| Project | Focus | Demo | Deep Dive |
| --- | --- | --- | --- |
| `smart-bms-firmware` | State machine, balancing, faults, SoC | `make run-bms` | [Architecture](projects/smart-bms-firmware/docs/ARCHITECTURE.md) |
| `ota-bootloader-simulator` | A/B staging, CRC32, confirm, rollback | `make run-ota` | [Architecture](projects/ota-bootloader-simulator/docs/ARCHITECTURE.md) |
| `can-telemetry-node` | CAN scheduling, fault priority, frame packing | `make run-can` | [Architecture](projects/can-telemetry-node/docs/ARCHITECTURE.md) |
| `motor-condition-monitor` | Windowed vibration analysis, fault classification, event log | `make run-motor` | [Architecture](projects/motor-condition-monitor/docs/ARCHITECTURE.md) |
| `resilient-flash-journal` | Crash-safe event persistence, replay, wear tracking | `make run-journal` | [Architecture](projects/resilient-flash-journal/docs/ARCHITECTURE.md) |
| `energy-harvesting-node` | Energy budget, task gating, brownout-safe duty cycling | `make run-power` | [Architecture](projects/energy-harvesting-node/docs/ARCHITECTURE.md) |
| `modbus-rtu-field-node` | Register map, CRC, Modbus function handling | `make run-modbus` | [Architecture](projects/modbus-rtu-field-node/docs/ARCHITECTURE.md) |
| `secure-attestation-node` | SHA-256 measurement, HMAC challenge-response, replay guard | `make run-attest` | [Architecture](projects/secure-attestation-node/docs/ARCHITECTURE.md) |
| `rtos-scheduler-lab` | Fixed-priority scheduling, deadline miss, watchdog | `make run-rtos` | [Architecture](projects/rtos-scheduler-lab/docs/ARCHITECTURE.md) |

## Recorded Demo Snapshots

### Smart BMS Firmware

```text
step=0 state=IDLE soc=72.00 charge=1 discharge=1 faults=none balancing=[0 0 0 0]
step=1 state=CHARGING soc=72.02 charge=1 discharge=0 faults=none balancing=[0 0 0 1]
step=2 state=CHARGING soc=72.03 charge=1 discharge=0 faults=none balancing=[0 0 0 1]
step=3 state=DISCHARGING soc=72.00 charge=0 discharge=1 faults=none balancing=[0 0 0 0]
step=4 state=FAULT soc=72.00 charge=0 discharge=0 faults= overtemp balancing=[0 0 0 0]
```

### OTA Bootloader Simulator

```text
factory: v1.0.0 crc=19A140E2 size=16 confirmed=1
after test upgrade reboot: v1.1.0 crc=4CE93DFC size=22 confirmed=0
reboot without confirm: v1.0.0 crc=19A140E2 size=16 confirmed=1
after permanent upgrade reboot: v1.2.0 crc=0E7C2932 size=27 confirmed=1
final reboot: v1.2.0 crc=0E7C2932 size=27 confirmed=1
```

### CAN Telemetry Node

```text
tick=2 emitted=3
  vcan0 080 [2] 07 00 00 00 00 00 00 00
  vcan0 180 [6] 60 04 1E 00 D4 02 00 00
  vcan0 280 [4] 16 03 07 00 00 00 00 00
tick=4 emitted=2
  vcan0 080 [2] 00 00 00 00 00 00 00 00
  vcan0 180 [6] C0 12 28 00 CD 02 00 00
```

### Motor Condition Monitor

```text
phase=imbalance fault=IMBALANCE sev=WARNING rms=636 p2p=1800 jerk=217 current=5200 temp=38.0C events=1
phase=bearing fault=BEARING_WEAR sev=WARNING rms=740 p2p=2300 jerk=1262 current=5600 temp=41.0C events=2
phase=stall fault=STALL sev=CRITICAL rms=39 p2p=120 jerk=18 current=9800 temp=44.0C events=3
```

### Resilient Flash Journal

```text
boot_a valid=3 latest_seq=2 latest_type=OVERCURRENT erase=[0 0 0]
boot_b valid=9 latest_seq=11 latest_type=CONFIG_CHANGE erase=[1 0 0]
tail seq=9 type=WATCHDOG_RESET sev=WARNING value=1
tail seq=10 type=POWER_FAIL sev=CRITICAL value=1
tail seq=11 type=CONFIG_CHANGE sev=INFO value=7
```

### Energy Harvesting Node

```text
phase=dawn mode=BALANCED action=SAMPLE battery=5042mWh backlog=1 reserve=NO wake=15s
phase=sunny mode=PERFORMANCE action=TX battery=5861mWh backlog=0 reserve=NO wake=5s
phase=cloud mode=BALANCED action=SLEEP battery=5691mWh backlog=3 reserve=NO wake=20s
phase=brownout mode=SURVIVAL action=DEEP_SLEEP battery=1816mWh backlog=4 reserve=YES wake=60s
phase=night-recovery mode=BALANCED action=TX battery=4171mWh backlog=3 reserve=NO wake=5s
```

### Modbus RTU Field Node

```text
req1 11 03 00 00 00 04 46 99
rsp1 11 03 08 09 29 00 18 00 3C 00 02 D8 70
req2 11 06 00 11 00 19 1A 95
rsp2 11 06 00 11 00 19 1A 95
req3 11 03 00 10 00 02 C7 5E
rsp3 11 03 04 00 41 00 19 7A 2C
rsp4 11 93 01 8C F5
```

### Secure Attestation Node

```text
phase=golden boot=YES status=OK counter=41 token=f90d4a7232d03ec661a92da43581e5f8
phase=replay_guard boot=YES status=STALE_CHALLENGE counter=41 token=BLOCKED
phase=trusted_update boot=YES status=OK counter=42 token=4fc3bda9798b53048f171e8650cfbc19
phase=tampered boot=NO status=MEASUREMENT_MISMATCH counter=42 token=BLOCKED
```

### RTOS Scheduler Lab

```text
phase=nominal idle=25 misses=0 watchdog=OK starved=none last=control
phase=radio_burst idle=10 misses=0 watchdog=OK starved=none last=control
phase=sensor_stall idle=0 misses=14 watchdog=TRIPPED starved=control last=sensor
phase=recovery idle=22 misses=0 watchdog=OK starved=none last=control
```

## Build

Build and test everything:

```sh
make all
make test
```

Run project demos:

```sh
make run-bms
make run-ota
make run-can
make run-motor
make run-journal
make run-power
make run-modbus
make run-attest
make run-rtos
```

## Why This Set Works on GitHub

- It covers control, reliability, and communications instead of only toy sensor code.
- Each project produces deterministic output that reviewers can inspect quickly.
- The repository is split into standalone subprojects that can later become separate repos.

## Suggested Next Hardware Targets

- Port the BMS project to STM32 or ESP32 with ADC, GPIO, and contactor control
- Port the OTA simulator to Zephyr or MCUboot integration tests
- Bridge the CAN node to Linux `vcan` or a real MCP2515 transceiver
- Port the motor monitor to an accelerometer + DMA ADC capture chain on STM32
- Port the flash journal to real NOR/QSPI flash with brownout-triggered flush
- Port the harvesting node to a solar charger + ADC coulomb counter board
- Port the Modbus field node to RS-485 transceivers with UART DMA
- Port the secure attestation node to a secure element or TrustZone-backed key store
- Port the RTOS lab to FreeRTOS or Zephyr task traces on real hardware

## References

- Zephyr native host execution:
  https://docs.zephyrproject.org/3.7.0/boards/native/native_posix/doc/index.html
- SocketCAN overview and `vcan` virtual interfaces:
  https://docs.kernel.org/networking/can.html
- MCUboot image signing and upgrade concepts:
  https://docs.mcuboot.com/signed_images.html
  https://docs.mcuboot.com/imgtool.html
