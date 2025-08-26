# ARCHITECTURE.md — Drongaz Software Architecture

## Philosophy
Hexagonal Architecture (Ports & Adapters). Keep **Core** pure and deterministic. All IO behind **Ports** with **HAL Adapters** at the edges. FreeRTOS used for scheduling and isolation of timing-sensitive work.

## Layers
- **Core (blue)**: `FlightCore`, `Controllers(AttitudeCtl, AltitudeCtl)`, `Mixer`, `Estimator`, `TelemetryFormat`
- **Ports (gray)**: `Clock`, `Imu`, `Esc`, `Radio`, `TelemetryOut`, `Store`
- **HAL Adapters (green)**: `LedcEsc`, `Mpu6050Imu`, `EspNowRadio`, `SerialTcpTelemetry`, `NvsStorage/EepromStorage`
- **Middleware/RTOS (orange)**: `Scheduler(FreeRTOS tasks)`, `PairingManager`
- **External (purple)**: ESP32/ESP32-C3, MPU6050, 4× ESC+Motors, Ground Station

## Directory Map
```
/include/        # public headers
/src/core/       # core logic (no IO)
/src/ports/      # pure abstract interfaces
/src/hal/        # adapters implementing ports
/src/mw/         # RTOS tasks, pairing, schedulers
/src/app/        # binaries/main entrypoints
/telemetry/      # protocol schemas + host tools
/tests/          # unit & HIL tests
/docs/           # architecture docs, diagrams
/.github/workflows/ # CI
```

## Dependency Rules
1. Core depends only on Ports.
2. HAL depends on Ports (implements them). HAL never imports Core.
3. Middleware wires Core+Ports+HAL at runtime; no business logic.
4. No global singletons for hardware. Prefer explicit dependency injection.
5. Control loops are allocation-free in hot paths; bounded timing.

## Ports: Snapshot
(See `/src/ports` — this snapshot may lag behind the code.)

- `IEsc`: arm/disarm/write/read (rpm/current)  
- `IImu`: begin/read (ax..gz,timestamp)  
- `IRadio`: read RC frame (thrust, biasFB, biasLR, yaw)  
- `ITelemetryOut`: write TelemetryFrame (attitude, battV, etc.)  
- `IStore`: put/get blobs

## Timing & Tasks
- **Scheduler tick:** configurable, aim for jitter < 1 ms.
- **Priority:** Control > Radio > Telemetry > Background.
- **Watchdog/Failsafe:** Radio link-loss immediately dampens mixer and disarms if needed.

## Testing
- Unit tests for Core & math. HIL scripts for HAL.
- CI: build matrix (esp32/esp32c3), clang-tidy/cppcheck, size reports.

## Diagrams
Mermaid is acceptable in GitHub; PNG exports recommended for docs portability in releases.
