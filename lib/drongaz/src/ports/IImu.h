#pragma once

struct ImuSample {
  float ax, ay, az;
  float gx, gy, gz;
};

struct IImu {
  virtual ~IImu() = default;
  virtual void begin() = 0;
  virtual ImuSample read() = 0;
};
