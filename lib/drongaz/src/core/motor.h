#pragma once

#include "../ports/i_esc.h"

class Motor {
public:
  explicit Motor(IEsc &esc) : esc_(esc) {}

  bool arm() { return esc_.arm(); }
  void disarm() { esc_.disarm(); }
  void setThrottle(float duty) { esc_.write(duty); }

private:
  IEsc &esc_;
};
