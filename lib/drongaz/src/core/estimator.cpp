#include "estimator.h"
#include <cmath>

void Estimator::reset() { att_ = {}; }

Attitude Estimator::update(const ImuSample &sample, float dt) {
  // Simple complementary filter
  constexpr float alpha = 0.98f;
  float pitchAcc = atan2f(sample.ay, sample.az);
  float rollAcc = atan2f(-sample.ax, sample.az);

  att_.pitch = alpha * (att_.pitch + sample.gx * dt) + (1 - alpha) * pitchAcc;
  att_.roll = alpha * (att_.roll + sample.gy * dt) + (1 - alpha) * rollAcc;
  att_.yaw += sample.gz * dt; // integrate gyro for yaw
  return att_;
}
