#pragma once

#include "../ports/i_imu.h"

struct Attitude {
  float pitch{0};
  float roll{0};
  float yaw{0};
};

class Estimator {
public:
  void reset();
  Attitude update(const ImuSample &sample, float dt);
  const Attitude &attitude() const { return att_; }

private:
  Attitude att_;
};
