# Architecture

## Overview

The project models the firmware logic that can sit above a USB-C PD frontend.
It does not implement the physical layer. Instead, it assumes the frontend has
already decoded Source Capabilities and reports the result as a list of fixed
PDOs.

Each control step:

1. Receives source capabilities and board telemetry
2. Builds a power request from load demand, cable current, and thermal limits
3. Selects the best PDO candidate
4. Advances the sink policy state machine
5. Emits a compact snapshot for logs or tests

## Modules

- `pd_types.*`: shared enums, contracts, and formatting helpers
- `pdo_selector.*`: PDO ranking and safe 5 V fallback selection
- `sink_policy.*`: state machine for attach, request, accept, reject, and fault handling
- `main.c`: deterministic demo scenarios

## State Model

- `DETACHED`: no attached source
- `REQUESTING`: sink has a pending PDO request
- `READY`: negotiated contract is active
- `DERATED`: active contract reduced by thermal policy
- `FAULT`: sink forces a safe fallback contract

## Fault Model

- `SOURCE_MISMATCH`: no suitable PDO satisfies the sink request
- `OVERTEMP`: board temperature exceeds the allowed threshold
- `BROWNOUT`: measured bus voltage sags too far below the active contract
- `RETRY_EXHAUSTED`: source rejected too many contract requests
