#include <Arduino.h>
#include "button.hpp"
#include "display.hpp"
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
  buttonSetup();
  displaySetup();
  reset(RunMode::NORMAL);
}

void loop()
{
  uint32_t ts = millis();
  serialLoop(&ts);
  resetLoop(&ts);
  modeLoop(&ts);
  buttonLoop(&ts);
  displayLoop(&ts);
}