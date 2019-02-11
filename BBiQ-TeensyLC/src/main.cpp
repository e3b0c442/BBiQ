#include <Arduino.h>
#include "button.hpp"
#include "buzzer.hpp"
#include "display.hpp"
#include "event.hpp"
#include "mode.hpp"
#include "probe.hpp"
#include "reset.hpp"
#include "rpc.hpp"
#include "serial.hpp"
#include "ui.hpp"

void setup()
{
  eventSetup();
  serialSetup();
  rpcSetup();
  resetSetup();
  modeSetup();
  buttonSetup();
  probeSetup();
  uiSetup();
  displaySetup();
  buzzerSetup();

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
  buzzerLoop(ts);
}