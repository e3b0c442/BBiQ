#include <Arduino.h>
#include "event.hpp"
#include "mode.hpp"
#include "reset.hpp"
#include "serial.hpp"

void setup()
{
  eventSetup();
  serialSetup();
  resetSetup();
  modeSetup();

  reset(RunMode::NORMAL);
}

void loop()
{
  uint32_t ts = millis();
  serialLoop(&ts);
  modeLoop(&ts);
}