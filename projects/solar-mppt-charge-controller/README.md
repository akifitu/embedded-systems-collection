# Solar MPPT Charge Controller

Portable C model of a solar charge controller with perturb-and-observe MPPT.
The project tracks panel power, transitions through bulk, absorb, and float
charge stages, and locks out on reverse-polarity, battery overvoltage, or
charger overtemperature conditions.

## Why It Matters

- Shows power-electronics control instead of only generic low-power logic
- Demonstrates MPPT duty adjustment, stage-based charging, and safety shutdown
- Maps directly to solar charger, DC power, and battery-management firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=dawn_idle state=IDLE cmd=OPEN_RELAY fault=NONE duty=0 relay=OPEN fan=OFF stage_target=0mV panel=14.0V 0.4A power=5.6W batt=11.8V temp=29C
phase=bulk_track state=BULK cmd=SEEK_MPP fault=NONE duty=52 relay=CLOSED fan=OFF stage_target=14400mV panel=18.0V 4.2A power=75.6W batt=12.5V temp=34C
phase=cloud_retrack state=BULK cmd=SEEK_MPP fault=NONE duty=50 relay=CLOSED fan=OFF stage_target=14400mV panel=16.8V 3.9A power=65.5W batt=13.2V temp=36C
phase=absorb_hold state=ABSORB cmd=HOLD_ABSORB fault=NONE duty=48 relay=CLOSED fan=ON stage_target=14400mV panel=17.2V 3.1A power=53.3W batt=14.4V temp=44C
phase=float_maintain state=FLOAT cmd=HOLD_FLOAT fault=NONE duty=28 relay=CLOSED fan=OFF stage_target=13600mV panel=17.8V 1.4A power=24.9W batt=13.7V temp=35C
phase=thermal_fault state=FAULT cmd=LATCH_FAULT fault=OVER_TEMP duty=0 relay=OPEN fan=ON stage_target=0mV panel=17.0V 2.0A power=34.0W batt=13.9V temp=83C
phase=reset_ready state=IDLE cmd=RESET_CHARGER fault=NONE duty=0 relay=OPEN fan=OFF stage_target=0mV panel=15.0V 0.8A power=12.0W batt=12.9V temp=31C
```

## Implementation Notes

- `power_tracker.c` performs a bounded perturb-and-observe duty update from panel power samples
- `charge_policy.c` maps bulk, absorb, float, and fault states into relay, fan, and target-voltage outputs
- `solar_mppt_controller.c` implements stage progression, MPPT tracking, fault latch, and reset flow

## Hardware Path

Natural next step: port the controller to STM32, AVR, or a solar power board
with synchronous buck PWM, panel voltage/current ADC sampling, battery voltage
feedback, NTC temperature input, and reverse-polarity or MOSFET fault sensing.
