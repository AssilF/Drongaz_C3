# Drongaz Core Library

Reusable hexagonal flight controller primitives (core logic, ports, adapters, middleware)
extracted from the main Drongaz project for integration into other PlatformIO projects.

## Modules
- **FlightCore**: orchestrates IMU, radio, motors and telemetry.
- **Motor**: wraps an `IEsc` port for arming/disarming and throttle control.
- **Estimator**: lightweight complementary filter producing pitch/roll/yaw from IMU samples.
- **Ports**: abstract interfaces for ESCs, IMU, radio, storage and telemetry output.

These pieces form the foundation for building ASCE-aligned flight stacks on ESP32/ESP32-C3 hardware.

