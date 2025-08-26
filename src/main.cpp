#include "core/flight_core.h"
#include <Arduino.h>
#include <array>

struct DummyEsc : IEsc {
  bool arm() override { return true; }
  void disarm() override {}
  void write(float) override {}
};

struct DummyImu : IImu {
  void begin() override {}
  ImuSample read() override { return {}; }
};

struct DummyRadio : IRadio {
  RcFrame read() override { return {}; }
};

struct DummyTelemetry : ITelemetryOut {
  void write(const TelemetryFrame &) override {}
};

DummyEsc esc1, esc2, esc3, esc4;
std::array<IEsc *, 4> escs{&esc1, &esc2, &esc3, &esc4};
DummyImu imu;
DummyRadio radio;
DummyTelemetry tele;

FlightCore flightCore(imu, radio, tele, escs);

void setup() {
  flightCore.init();
  flightCore.arm();
}

void loop() {
  flightCore.update(0.01f);
  delay(10);
}
