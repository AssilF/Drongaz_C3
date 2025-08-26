#pragma once

#include <array>

#include "../ports/i_imu.h"
#include "../ports/i_radio.h"
#include "../ports/i_telemetry_out.h"
#include "estimator.h"
#include "motor.h"

class FlightCore {
public:
  FlightCore(IImu &imu, IRadio &radio, ITelemetryOut &tel,
             const std::array<IEsc *, 4> &escs);

  void init();
  void arm();
  void disarm();
  void update(float dt);

private:
  IImu &imu_;
  IRadio &radio_;
  ITelemetryOut &telemetry_;
  std::array<Motor, 4> motors_;
  Estimator estimator_;
  bool armed_{false};
};
