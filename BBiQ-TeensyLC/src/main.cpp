#include <Arduino.h>
#include "button.hpp"
#include "display.hpp"
#include "event.hpp"
#include "mode.hpp"
#include "probe.hpp"
#include "reset.hpp"
#include "serial.hpp"
#include "ui.hpp"

void setup()
{
  eventSetup();
  serialSetup();
  resetSetup();
  modeSetup();
  buttonSetup();
  probeSetup();
  uiSetup();
  displaySetup();

  reset(RunMode::NORMAL);
}

void loop()
{
  uint32_t ts = millis();
  serialLoop(ts);
  resetLoop(ts);
  modeLoop(ts);
  buttonLoop(ts);
  probeLoop(ts);
  uiLoop(ts);
  displayLoop(ts);
}