#pragma once

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
