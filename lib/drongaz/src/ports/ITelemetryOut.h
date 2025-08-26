#pragma once

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
