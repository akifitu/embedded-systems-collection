# STM32F446 Bring-Up Checklist

## Firmware Setup

- Generate CubeMX startup code for `STM32F446RETx`
- Enable `ADC1` scan conversion with DMA for 7 analog channels
- Configure `CAN1` in normal mode with a board-appropriate bitrate
- Enable GPIO outputs for charge, discharge, balance, and status LED

## First Bench Tests

- Verify divider-scaled cell voltages with a lab supply before enabling outputs
- Inject a known shunt voltage and confirm current scaling math
- Heat one NTC and confirm overtemperature fault entry
- Force an undervoltage condition and confirm discharge is disabled

## Safety Gates Before Live Pack Testing

- Add hardware fuse and current-limited bench supply
- Keep contactor outputs disconnected during ADC calibration
- Log every computed fault and output transition over UART or SWO
- Validate the balance outputs with a resistor load before attaching cells

