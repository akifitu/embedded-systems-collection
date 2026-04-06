# Implementation Plan

1. Define slot identifiers, verdicts, failure reasons, manifest layout, and
   formatting helpers so demos and tests can inspect deterministic boot decisions.
2. Add standalone SHA-256 and HMAC-SHA256 primitives for image-hash and
   manifest-authentication checks.
3. Build a manifest verifier that enforces product ID, ABI revision,
   anti-rollback counter, hash match, and authenticated metadata.
4. Add slot-selection logic that prefers the primary image, falls back to a
   recovery slot when possible, and rejects both when trust is broken.
5. Add unit tests for golden boot, rollback rejection, tamper detection,
   recovery fallback, and hard-fail cases.
