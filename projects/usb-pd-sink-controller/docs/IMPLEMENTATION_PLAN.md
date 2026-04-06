# Implementation Plan

1. Define PD states, source capability types, contracts, request replies, and
   string helpers for deterministic demos.
2. Implement a PDO selector that ranks fixed supplies against desired power,
   preferred voltage, and cable current constraints.
3. Add a sink policy engine that handles attach, request, accept, reject,
   thermal derating, source mismatch, brownout, and safe fallback behavior.
4. Build a host demo with multiple source scenarios so GitHub readers can see
   negotiation outcomes without PD hardware.
5. Add unit tests for contract selection, derating, brownout fallback, retry
   exhaustion, and mismatch handling.
