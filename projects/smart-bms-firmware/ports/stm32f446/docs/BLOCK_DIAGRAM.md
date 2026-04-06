# STM32F446 Port Block Diagram

```mermaid
flowchart LR
    Cells[Cell Divider Network] --> ADC[ADC1 + DMA]
    Shunt[Current Sense] --> ADC
    NTC[Temperature Sensors] --> ADC
    ADC --> HAL[BMS HAL Bridge]
    HAL --> Core[BMS Controller]
    Core --> GPIO[Charge, Discharge, Balance GPIO]
    Core --> CAN[CAN1 Telemetry]
    Tick[SysTick] --> HAL
```

## Porting Notes

- Keep the controller logic platform-agnostic and let the HAL bridge own scaling.
- Use DMA-backed ADC sampling so the control step reads a coherent snapshot.
- Publish critical faults over CAN before toggling contactor outputs.

