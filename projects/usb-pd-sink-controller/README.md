# USB PD Sink Controller

Portable C model of a USB Power Delivery sink policy engine. The project
parses fixed PDO capabilities, selects the most suitable contract for the load,
derates under thermal stress, handles request retries, and falls back to a
safe 5 V contract on brownout or negotiation failure.

## Why It Matters

- Shows embedded power-policy logic beyond simple sensors or GPIO
- Demonstrates decision-making under cable, thermal, and source constraints
- Maps cleanly to real USB-C hardware such as FUSB302 or STUSB4500 frontends

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=attach_request state=REQUESTING req=20.0V/1.90A active=none power=38.0W retries=0 faults=none
phase=ready state=READY req=20.0V/1.90A active=20.0V/1.90A power=38.0W retries=0 faults=none
phase=thermal_derate state=DERATED req=9.0V/2.00A active=9.0V/2.00A power=18.0W retries=0 faults=none
phase=thin_cable state=READY req=20.0V/1.50A active=20.0V/1.50A power=30.0W retries=0 faults=none
phase=brownout state=FAULT req=5.0V/0.50A active=5.0V/0.50A power=2.5W retries=0 faults=brownout
phase=reject_loop state=FAULT req=5.0V/0.50A active=5.0V/0.50A power=2.5W retries=3 faults=retry_exhausted
```

## Implementation Notes

- `pdo_selector.c` ranks fixed PDOs against requested power, preferred voltage,
  and cable current limits
- `sink_policy.c` models request, accept, reject, derate, and fault fallback
- `main.c` drives deterministic scenarios that resemble laptop or SBC sinks

## Hardware Path

Natural next step: pair an STM32 or RP2040 with an FUSB302, STUSB4500, or
similar Type-C PD frontend and integrate ADC-based input current telemetry.
