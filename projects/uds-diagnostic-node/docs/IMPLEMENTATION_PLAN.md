# Implementation Plan

1. Define session, security, negative-response, DTC, and frame helper types so
   demos and tests can inspect deterministic state transitions.
2. Implement a DID table with VIN and software version examples.
3. Build a UDS node that handles `0x10`, `0x22`, `0x27`, `0x19`, and `0x14`
   with session/security gating and standards-style response formatting.
4. Add a host demo that walks through extended session, seed/key unlock, DTC
   read and clear, plus a failed security access case.
5. Add unit tests for session switching, DID reads, unlock success, bad-key
   negative response, and DTC clearing.
