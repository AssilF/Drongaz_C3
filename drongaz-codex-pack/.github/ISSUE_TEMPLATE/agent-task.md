---
name: Agent Task
about: Create a structured task for Drongaz agents
title: "[DRZ]: "
labels: ["triage","agent:A-00"]
assignees: ""
---

## Intent
Short goal (e.g., "Add Mpu6050Imu adapter").

## Layer & Owner
- Layer: core / ports / hal / mw / telemetry / tests / docs
- Owner: A-10 / A-20 / A-21 / A-22 / A-30 / A-40 / A-50 / A-60 / A-70

## Context
Paths, logs, references:
- src/...

## Acceptance Criteria
- [ ] Unit tests green
- [ ] No core↔HAL boundary violations
- [ ] Docs updated

## Task Schema (paste/edit)
```yaml
task:
  id: DRZ-####
  intent: ""
  context: []
  acceptance_criteria: []
  constraints:
    - "C++17"
    - "PlatformIO env: esp32dev, esp32c3"
  artifacts_expected: []
  agent_owner: A-20
  reviewers: [A-01, A-30]
```
