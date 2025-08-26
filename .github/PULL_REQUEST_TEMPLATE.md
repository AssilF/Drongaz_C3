## PR Checklist

- [ ] CI green (build matrix, unit tests, clang-tidy, cppcheck)
- [ ] No Core ↔ HAL violations (imports reviewed)
- [ ] Docs updated (`AGENTS.md`, `ARCHITECTURE.md`, or port docs if needed)
- [ ] Tests: unit/bench/HIL as appropriate
- [ ] Telemetry schema bumped or compatible (if affected)
- [ ] If adapters/platforms changed → `docs/asce/compatibility.json` updated

### Summary
Describe what changed and why.

### Risk & Mitigation
- Timing, memory, safety notes.

### Linked Issues
Fixes #____
