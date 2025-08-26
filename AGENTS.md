# AGENTS.md — Drongaz Codex

**Purpose:** Define a lightweight, human+AI agent system for the Drongaz codebase so we can plan, build, review, and ship consistently.
**Philosophy:** Hexagonal architecture (Ports & Adapters). Core stays pure. All IO behind ports. Repeatable workflows. Strong guardrails.

---

## 0) TL;DR

- **Conductor** (A-00) routes work.
- **Hex Guardian** (A-01) enforces architecture rules.
- **Core / Control** (A-10) owns FlightCore, Controllers, Mixer, Estimator.
- **HAL Builder** (A-20) adds/maintains adapters (e.g., `LedcEsc`, `Mpu6050Imu`, `EspNowRadio`).
- **Radio+Link** (A-21) manages ESP-NOW/IP links and pairing.
- **Telemetry** (A-22) tracks serial/TCP telemetry + ground tooling.
- **Tests & QA** (A-30) owns unit/bench tests + HIL scripts.
- **CI & Release** (A-40) builds, analyzes, versions, and tags.
- **Docs** (A-50) keeps READMEs, API docs, diagrams fresh.
- **Safety & Power** (A-60) checks power/thermal/current margins.
- **Configurator Bridge** (A-70) exports compatibility metadata for the ASCE shop/configurator.

Each agent has: **Inputs → Responsibilities → Outputs → Guardrails → Triggers**.

---

## 1) Codebase Ground Rules

- **Language/Tooling:** C++17 via **PlatformIO**; ESP32/ESP32-C3 targets; FreeRTOS; no Arduino-IDE-only hacks.
- **Layering:**  
  - **Core (blue):** `FlightCore`, `Controllers(AttitudeCtl, AltitudeCtl)`, `Mixer`, `Estimator`, `TelemetryFormat`
  - **Ports (gray):** `Clock`, `Imu`, `Esc`, `Radio`, `TelemetryOut`, `Store`
  - **HAL Adapters (green):** `LedcEsc` → `Esc`, `Mpu6050Imu` → `Imu`, `EspNowRadio` → `Radio`, `SerialTcpTelemetry` → `TelemetryOut`, `NvsStorage/EepromStorage` → `Store`
  - **Middleware/RTOS (orange):** `Scheduler(FreeRTOS tasks)`, `PairingManager`
  - **External (purple):** ESP32/ESP32-C3 MCU, MPU6050, 4× ESC+Motors, Ground Station
- **Directory Map (canonical):**
  ```
  /include/        # public headers
  /src/core/       # core logic (no IO)
  /src/ports/      # pure abstract interfaces
  /src/hal/        # adapters implementing ports
  /src/mw/         # RTOS tasks, pairing, schedulers
  /src/app/        # binaries/main
  /telemetry/      # protocol schemas + host tools (Processing/Java, Python)
  /tests/          # unit & HIL tests
  /docs/           # architecture docs, diagrams
  /.github/workflows/ # CI
  ```
- **Naming:** Ports `I*` (e.g., `IEsc`), adapters `<Impl><Port>` (e.g., `LedcEsc`), tasks `Task<Name>`.
- **Core NEVER depends on HAL.** Only via ports.
- **Formatting & Static Analysis:** clang-format, clang-tidy, cppcheck.
- **Commits:** Conventional Commits (`feat:`, `fix:`, `docs:`, `refactor:`, `test:`, `build:`).
- **Issues/PR Labels:** `agent:A-xx`, `layer:core|ports|hal|mw`, `risk:power`, `telemetry`, `radio`, `estimator`, `control`, `mixer`, `docs`, `tests`.

---

## 2) Shared Message Schema (Issues/Tasks)

Tasks exchanged between humans and AI agents should carry this minimal YAML:

```yaml
task:
  id: DRZ-####            # e.g., GitHub issue number or short slug
  intent: ""              # short goal, e.g., "Add Mpu6050Imu adapter"
  context:                # links to code, specs, logs
    - path: src/ports/i_imu.h
    - path: src/hal/Mpu6050Imu.cpp
  acceptance_criteria:
    - "Unit tests green"
    - "No core↔HAL boundary violations"
  constraints:
    - "C++17"
    - "PlatformIO env: esp32dev, esp32c3"
    - "Timing jitter < 1ms on scheduler tick"
  artifacts_expected:
    - "Adapter .h/.cpp"
    - "Config in platformio.ini"
    - "Docs in docs/hal/imu.md"
  agent_owner: A-20
  reviewers:
    - A-01
    - A-30
```

---

## 3) Agents

### A-00 — Conductor (Orchestrator)
**Inputs:** New issues, roadmap, failing CI, user priorities.  
**Responsibilities:** Route tasks to agents; slice epics; ensure cross-agent handoffs.  
**Outputs:** Labeled issues, milestone plan, weekly plan in `/docs/plan/weekly.md`.  
**Guardrails:** No code changes; only planning.  
**Triggers:** Any new feature/bug; weekly cadence.

---

### A-01 — Hex Guardian (Architecture)
**Inputs:** PRs touching `/src/core`, `/src/ports`, `/src/hal`, `/src/mw`; diagrams.  
**Responsibilities:**
- Enforce hexagonal boundaries; review dependencies and includes.
- Maintain `ARCHITECTURE.md` and mermaid/PNG diagrams.
- Maintain Port contracts (`/src/ports/`) and change logs.
**Outputs:** Review notes; updated diagrams in `/docs/architecture/`.  
**Guardrails:** Must reject any direct Core↔HAL coupling; no hidden singletons.  
**Triggers:** `layer:*` labels, any port signature change.

---

### A-10 — Core / Control Agent
**Inputs:** Controller specs; estimator math; tuning notes.  
**Responsibilities:**
- Implement/maintain `FlightCore`, `Controllers(Attitude/Altitude)`, `Mixer`, `Estimator`.
- Keep control math deterministic and allocation-free in hot paths.
**Outputs:** Core code + unit tests (`/tests/core_*`).  
**Guardrails:** No hardware calls; pure logic; fixed-point or bounded heap.  
**Triggers:** New aircraft type, new control mode, regression in stability.

---

### A-20 — HAL Builder
**Inputs:** Port contracts, datasheets, board pin maps.  
**Responsibilities:**
- Add/maintain adapters: `LedcEsc`, `Mpu6050Imu`, `EspNowRadio`, `SerialTcpTelemetry`, `NvsStorage`.
- Configure `platformio.ini` envs (ESP32/ESP32-C3).
- Ensure ISR/timing safety and FreeRTOS awareness.
**Outputs:** Adapter code, pin tables in `/docs/hal/`, example `app` wiring.  
**Guardrails:** No control logic leakage; respect port granularity; debounce & bounds.  
**Triggers:** New sensor/actuator/radio; platform swap.

---

### A-21 — Radio & Link Agent
**Inputs:** Link budgets, ESP-NOW pairing flows, TCP bridge needs.  
**Responsibilities:** Pairing manager, coexistence of **ESP-NOW** control and **TCP/Serial** telemetry; link-loss failsafes.  
**Outputs:** `src/mw/PairingManager.*`, radio tests, throughput/latency logs.  
**Guardrails:** Never block control loop; watchdog integration.  
**Triggers:** Connectivity bugs; dual-link changes.

---

### A-22 — Telemetry & Ground Tools
**Inputs:** Telemetry schema; Processing/Python tools.  
**Responsibilities:** Maintain `TelemetryOut` port + `SerialTcpTelemetry` adapter; ground tools (Processing app, FFT views, toggleable graphs, PID/status ingestion).  
**Outputs:** `/telemetry/{schema.md, processing_app/, py_tools/}`; sample captures.  
**Guardrails:** Back-pressure handling; bounded frame sizes; versioned schema.  
**Triggers:** New signals; dashboard features; schema bumps.

---

### A-30 — Tests & QA
**Inputs:** Specs from A-01/A-10/A-20, bug reports, flight logs.  
**Responsibilities:** Unit tests (doctest/Catch2/Unity), bench tests, HIL scripts; hardware test matrices; CI test orchestration.  
**Outputs:** `/tests/` + CI job definitions; coverage reports.  
**Guardrails:** Deterministic seeds; timeouts; flaky-test quarantine.  
**Triggers:** Any PR with code, release candidates.

---

### A-40 — CI & Release
**Inputs:** `platformio.ini`, board matrix, semver plan.  
**Responsibilities:** GitHub Actions (build matrix, clang-tidy/cppcheck, size, unit tests); artifact upload; semantic releases and changelogs.  
**Outputs:** Badges, release notes, tagged binaries in `releases/`.  
**Guardrails:** Fails closed; no manual hotfix bypass.  
**Triggers:** Every PR; tag events.

---

### A-50 — Docs Agent
**Inputs:** API changes, diagrams, decisions.  
**Responsibilities:** Keep READMEs, `ARCHITECTURE.md`, Port docs, HAL quickstarts; convert mermaid → PNG for portability; keep examples compiling.  
**Outputs:** `/docs/**`, updated diagrams, snippets tested.  
**Guardrails:** No divergence from code; doc CI check.  
**Triggers:** API/port changes; new adapters; milestones.

---

### A-60 — Safety & Power Agent
**Inputs:** Motor/ESC/battery specs, power logs, temperature/rpm observations.  
**Responsibilities:** Power budget spreadsheets, current limits, ESC PWM bounds, brownout risk notes; “safe defaults” for mixers and arming rules.  
**Outputs:** `/docs/safety/power.md`, checklists before test flights.  
**Guardrails:** Conservative margins; refuse unsafe configs.  
**Triggers:** Prop/motor/battery changes; reports of sag/overheat.

---

### A-70 — Configurator Bridge (ASCE)
**Inputs:** Inventory of components/adapters, platform profiles, compatibility rules.  
**Responsibilities:** Export machine-readable **compatibility graph** (ports, adapters, power/IO footprints) for the ASCE shop configurator.  
**Outputs:** `/docs/asce/compatibility.json` (see schema below), adapter rules, auto-added adapter hints.  
**Guardrails:** Core-agnostic; no shop logic leaking into firmware.  
**Triggers:** New platform or adapter; attribute updates.

**Example `compatibility.json` (minimal):**
```json
{
  "version": "1.0.0",
  "platforms": [
    {
      "id": "drongaz",
      "ports": ["Imu","Esc","Radio","TelemetryOut","Store","Clock"],
      "power_tiers": ["11V","16V"],
      "notes": "Hex-arch flight controller"
    }
  ],
  "components": [
    {"id":"mpu6050","type":"sensor","port":"Imu"},
    {"id":"ledc_esc","type":"actuator","port":"Esc"},
    {"id":"espnow","type":"radio","port":"Radio"},
    {"id":"serial_tcp","type":"telemetry","port":"TelemetryOut"}
  ],
  "adapters": [
    {"from":"mpu6050","to_port":"Imu","impl":"Mpu6050Imu"},
    {"from":"ledc_esc","to_port":"Esc","impl":"LedcEsc"},
    {"from":"espnow","to_port":"Radio","impl":"EspNowRadio"},
    {"from":"serial_tcp","to_port":"TelemetryOut","impl":"SerialTcpTelemetry"}
  ],
  "auto_rules": [
    {"if":["platform:drongaz","component:mpu6050"],"then_add":["adapter:Mpu6050Imu"]},
    {"if":["platform:drongaz","component:ledc_esc"],"then_add":["adapter:LedcEsc"]}
  ]
}
```

---

## 4) Core Contracts (Ports Snapshot)

> **Source of truth is `/src/ports/`.** This snapshot helps reviewers spot drift.

```cpp
// /src/ports/i_esc.h
struct IEsc {
  virtual ~IEsc() = default;
  virtual bool arm() = 0;
  virtual void disarm() = 0;
  virtual void write(float duty) = 0;
};

// /src/ports/i_imu.h
struct ImuSample {
  float ax, ay, az;
  float gx, gy, gz;
};
struct IImu {
  virtual ~IImu() = default;
  virtual void begin() = 0;
  virtual ImuSample read() = 0;
};

// /src/ports/i_radio.h
struct RcFrame {
  float thrust;
  float biasFb;
  float biasLr;
  float yaw;
};
struct IRadio {
  virtual ~IRadio() = default;
  virtual RcFrame read() = 0;
};

// /src/ports/i_telemetry_out.h
struct TelemetryFrame {
  float pitch;
  float roll;
  float yaw;
  float battV;
};
struct ITelemetryOut {
  virtual ~ITelemetryOut() = default;
  virtual void write(const TelemetryFrame &frame) = 0;
};

// /src/ports/i_store.h
struct IStore {
  virtual ~IStore() = default;
  virtual bool put(const char *key, const void *data, size_t len) = 0;
  virtual bool get(const char *key, void *data, size_t len) = 0;
};
```
---

## 5) Standard Workflows (Agent Runbooks)

### W-01: Add a New HAL Adapter (A-20 owner, A-01 review)
1. Create issue with schema → `agent:A-20`, `layer:hal`.
2. Confirm port signature in `/src/ports`.
3. Implement `src/hal/<Impl><Port>.{h,cpp}`; wire pins in `platformio.ini`.
4. Add example wiring in `/src/app/example_*`.
5. Unit/bench tests in `/tests/hal_*`.
6. Update `/docs/hal/<port>.md` + diagram.
7. CI must pass; A-01 reviews dependencies; A-30 approves tests.

### W-02: New Control Feature in Core (A-10 owner)
1. RFC in `/docs/rfcs/RFC-<feature>.md` (math, timing, interfaces).
2. Implement in `/src/core/`; no IO.
3. Extend unit tests; run latency checks.
4. Update telemetry schema if needed (A-22).
5. Review: A-01 (arch), A-30 (tests).

### W-03: Dual-Link Telemetry/Control Change (A-21/A-22)
1. Define link contract + priority (control > telemetry).
2. Update `PairingManager` + `SerialTcpTelemetry`.
3. HIL test with link loss + recovery.
4. Document operator flows `/docs/radio/ops.md`.

### W-04: Export Compatibility for Configurator (A-70)
1. Update `/docs/asce/compatibility.json` when adding component/adapter.
2. Validate with JSON schema; CI job `validate-compat`.
3. Provide example “auto-added adapter” rules.
4. Notify web team via label `asce:compat`.

---

## 6) Telemetry Schema (Operational Snapshot)

- **Transport:** TCP or Serial (selectable).  
- **Frame:** line-delimited JSON (bounded < 512B).  
- **Versioning:** `schema_rev` increments on breaking change.  

```json
{
  "schema_rev": 1,
  "t_us": 123456789,
  "att": {"pitch":0.01,"roll":-0.03,"yaw":1.57},
  "imu": {"ax":0.0,"ay":0.0,"az":9.8,"gx":0.1,"gy":-0.1,"gz":0.0},
  "rc": {"thrust":0.6,"biasFB":0.0,"biasLR":0.1,"yaw":0.0},
  "battV": 15.8,
  "flags": {"armed":true,"failsafe":false}
}
```

Ground tools (Processing/Python) must tolerate missing fields and advertise the `schema_rev` they support.

---

## 7) Checklists

### Pre-Flight (A-60 + A-21 + A-22)
- [ ] Battery IR and voltage in spec; ESC temp baseline.
- [ ] Link RSSI stable; failsafe armed.
- [ ] Telemetry plotting attitude + RC; packet loss < 2%.

### PR Gate (all)
- [ ] CI green: build matrix, tidy, cppcheck, unit tests.
- [ ] No Core↔HAL violations (A-01 sign-off).
- [ ] Docs updated (A-50).
- [ ] If adapters or platforms changed → update `compatibility.json` (A-70).

---

## 8) Labels & Ownership Map

| Label            | Default Owner |
|------------------|---------------|
| `agent:A-00`     | Conductor     |
| `agent:A-01`     | Hex Guardian  |
| `agent:A-10`     | Core/Control  |
| `agent:A-20`     | HAL Builder   |
| `agent:A-21`     | Radio & Link  |
| `agent:A-22`     | Telemetry     |
| `agent:A-30`     | Tests & QA    |
| `agent:A-40`     | CI & Release  |
| `agent:A-50`     | Docs          |
| `agent:A-60`     | Safety/Power  |
| `agent:A-70`     | Config Bridge |

Multiple agents may co-review; **A-01** can veto on architecture breaches.

---

## 9) Glossary

- **Hex Architecture:** Separation of **Core**, **Ports**, **Adapters**.
- **Port:** Pure interface describing a capability (e.g., `IImu`).
- **Adapter:** Concrete implementation (e.g., `Mpu6050Imu`) bound at edges.
- **HIL:** Hardware-in-the-loop.
- **ASCE:** Broader ecosystem + shop/configurator that consumes compatibility metadata.

---

## 10) Roadmap Seeds (suggested backlog)

- `feat(core)` Rate-limiting + anti-windup for AttitudeCtl.
- `feat(hal)` Add `Clock` port + high-res timer adapter.
- `feat(radio)` Dual-path ESP-NOW control + TCP telemetry arbitration.
- `feat(telemetry)` PID tuning round-trip via command frames.
- `feat(config)` Populate `compatibility.json` for Bulky, Thegill, Mechiane.
- `test(hil)` Spin-stand RPM vs PWM curve capture; auto-fit mixer limits.
- `safety` ESC brownout detector + staged arming.

---

*This document is living. Changes to ports or layering MUST update this file and `ARCHITECTURE.md`.*
