#pragma once
#include <cstddef>

struct IStore {
  virtual ~IStore() = default;
  virtual bool put(const char *key, const void *value, size_t size) = 0;
  virtual bool get(const char *key, void *value, size_t size) = 0;
};
