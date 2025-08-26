#include "core/FlightCore.h"
#include <Arduino.h>

FlightCore flightCore;

void setup() { flightCore.init(); }

void loop() { flightCore.update(); }
