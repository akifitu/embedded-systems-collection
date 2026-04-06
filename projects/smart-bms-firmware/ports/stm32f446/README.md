# STM32F446 BMS Port Scaffold

This branch adds a first hardware-port track for the host-side BMS project. The
target is an STM32F446 device such as the Nucleo-F446RE or a custom F446-based
power board.

## Intent

- Reuse the existing BMS control logic on a real MCU
- Sample cell voltages, current, and temperatures through ADC
- Drive charge and discharge outputs through GPIO
- Prepare a path toward CAN telemetry on `CAN1`

## Directory Map

- `Core/Inc`: board-facing headers and configuration
- `Core/Src`: STM32 entry point, app loop, and HAL bridge
- `docs`: block diagram and bring-up checklist

## Proposed Pin Map

| Signal | STM32F446 Pin | Peripheral |
| --- | --- | --- |
| Cell 0 sense | `PA0` | `ADC1_IN0` |
| Cell 1 sense | `PA1` | `ADC1_IN1` |
| Cell 2 sense | `PA4` | `ADC1_IN4` |
| Cell 3 sense | `PB0` | `ADC1_IN8` |
| Pack current shunt | `PB1` | `ADC1_IN9` |
| NTC 0 | `PC0` | `ADC1_IN10` |
| NTC 1 | `PC1` | `ADC1_IN11` |
| Charge enable | `PA8` | GPIO output |
| Discharge enable | `PA9` | GPIO output |
| Balance 0 | `PA10` | GPIO output |
| Balance 1 | `PA11` | GPIO output |
| Balance 2 | `PA12` | GPIO output |
| Balance 3 | `PB4` | GPIO output |
| CAN RX | `PB8` | `CAN1_RX` |
| CAN TX | `PB9` | `CAN1_TX` |
| Status LED | `PC13` | GPIO output |

## Next Build Step

Generate the STM32CubeMX project with GPIO, ADC1 scan + DMA, CAN1, and a
1 kHz system tick, then wire the generated handles into `bms_port_hal.c`.

## Supporting Docs

- [Block Diagram](docs/BLOCK_DIAGRAM.md)
- [Bring-Up Checklist](docs/BRINGUP_CHECKLIST.md)

