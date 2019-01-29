#include <Arduino.h>
#include <stdlib.h>
#include <SPI.h>
#include <U8x8lib.h>
#include "event.hpp"
#include "mode.hpp"
#include "pins.hpp"
#include "probe.hpp"

namespace
{
const uint32_t RESET_SPLASH_TIME = 2500;
U8X8_SSD1306_128X32_UNIVISION_4W_HW_SPI oled(
    (uint8_t)Pin::OLED_CS, (uint8_t)Pin::OLED_DC, (uint8_t)Pin::OLED_RST);

uint32_t lastReset = 0;
bool splashOn = true;
Probe::ID selected = Probe::ID::COUNT;
RunMode currentMode = RunMode::BOOT;

void drawBootDisplay()
{
    oled.clear();
    oled.setFont(u8x8_font_8x13_1x2_r);
    oled.draw1x2String(0, 0, "BBiQ");
}

void drawNoProbeDisplay()
{
    oled.clear();
    oled.setFont(u8x8_font_8x13_1x2_r);
    oled.drawString(4, 0, "No probe");
    oled.drawString(4, 2, "connected");
}

void drawProgramDisplay()
{
    oled.clear();
    oled.setFont(u8x8_font_8x13_1x2_r);
    oled.drawString(0, 0, "ESP8266 USB Prgm");
    oled.drawString(1, 2, "Mode activated");
}

void drawMainDisplay(int cur, int low, int high, const char *name)
{
    oled.clear();
    char nums[8];
    oled.setFont(u8x8_font_8x13_1x2_r);

    __itoa(cur, nums, 10);
    nums[3] = '\0';
    oled.draw1x2String(0, 0, nums);
    oled.drawString(4, 0, name);
    __itoa(low, nums, 10);
    nums[3] = '\0';
    oled.drawString(4, 2, "L:");
    oled.drawString(6, 2, nums);
    __itoa(high, nums, 10);
    nums[3] = '\0';
    oled.drawString(10, 2, "H:");
    oled.drawString(12, 2, nums);
}

void handler(Event *e)
{
    switch (e->type)
    {
    case Event::Type::RESET:
        lastReset = e->ts;
        drawBootDisplay();
        break;
    case Event::Type::MODE:
    {
        ModeEvent *me = (ModeEvent *)e;
        currentMode = me->mode;
        break;
    }
    case Event::Type::PROBE:
    {
        ProbeEvent *pe = (ProbeEvent *)e;
        switch (pe->action)
        {
        case ProbeEvent::Action::SELECT:
            if (pe->probe == NULL)
                selected = Probe::ID::COUNT;
            else
                selected = pe->probe->id;
            //intentional fallthrough
        default:
            if (selected == Probe::ID::COUNT)
                drawNoProbeDisplay();
            else if (selected == pe->probe->id)
                if (!splashOn)
                    drawMainDisplay(pe->probe->temperature, pe->probe->lowAlarm, pe->probe->highAlarm, pe->probe->name);
        }
        break;
    }
    default:;
    }
}
} // namespace

void displaySetup()
{
    registerHandler(Event::Type::RESET, &handler);
    registerHandler(Event::Type::MODE, &handler);
    registerHandler(Event::Type::PROBE, &handler);
    oled.begin();
    oled.setFlipMode(true);
    oled.setContrast(0);
    drawBootDisplay();
}

void displayLoop(uint32_t *ts)
{
    if (splashOn && int32_t(*ts) - int32_t(lastReset) > int32_t(RESET_SPLASH_TIME))
    {
        switch (currentMode)
        {
        case RunMode::BOOT:
            lastReset = *ts;
            break;
        case RunMode::PROGRAM:
            drawProgramDisplay();
            splashOn = false;
            break;
        case RunMode::NORMAL:
            if (selected == Probe::ID::COUNT)
                drawNoProbeDisplay();
            splashOn = false;
            break;
        }
    }
}
