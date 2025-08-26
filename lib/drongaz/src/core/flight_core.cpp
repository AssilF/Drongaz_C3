#include "flight_core.h"

FlightCore::FlightCore(IImu &imu, IRadio &radio, ITelemetryOut &tel,
                       const std::array<IEsc *, 4> &escs)
    : imu_(imu), radio_(radio), telemetry_(tel),
      motors_{Motor(*escs[0]), Motor(*escs[1]), Motor(*escs[2]),
              Motor(*escs[3])} {}

void FlightCore::init() {
  imu_.begin();
  estimator_.reset();
}

void FlightCore::arm() {
  bool ok = true;
  for (auto &m : motors_) {
    ok &= m.arm();
  }
  armed_ = ok;
}

void FlightCore::disarm() {
  for (auto &m : motors_) {
    m.disarm();
  }
  armed_ = false;
}

void FlightCore::update(float dt) {
  ImuSample sample = imu_.read();
  Attitude att = estimator_.update(sample, dt);

  TelemetryFrame frame{};
  frame.pitch = att.pitch;
  frame.roll = att.roll;
  frame.yaw = att.yaw;
  telemetry_.write(frame);

  RcFrame rc = radio_.read();
  if (armed_) {
    for (auto &m : motors_) {
      m.setThrottle(rc.thrust);
    }
  }
}
