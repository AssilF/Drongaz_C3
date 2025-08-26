#pragma once

struct IEsc {
  virtual ~IEsc() = default;
  virtual bool arm() = 0;
  virtual void disarm() = 0;
  virtual void write(float duty) = 0;
};
