# Architecture

## Overview

The project models a small UDS application layer running above a transport such
as CAN ISO-TP. Each request:

1. Parses the service ID and subfunction or DID
2. Checks whether the current session and security state allow the operation
3. Produces a positive response or a `0x7F` negative response code
4. Updates node state such as DTC storage or security lock

## Modules

- `uds_types.*`: enums, DTC structures, and formatting helpers
- `did_table.*`: static data identifier lookup for VIN and software version
- `uds_node.*`: service dispatcher and diagnostic state machine
- `main.c`: deterministic request/response demo flow

## Services

- `0x10`: Diagnostic Session Control
- `0x22`: Read Data By Identifier
- `0x27`: Security Access seed/key
- `0x19`: Read DTC Information
- `0x14`: Clear Diagnostic Information

## Security Model

- Default session starts locked
- Extended session allows seed request
- Correct key unlocks restricted services
- Invalid key returns NRC `0x35` and relocks the node
