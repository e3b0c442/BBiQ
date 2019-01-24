#include <Arduino.h>
#include <stdlib.h>
#include <SPI.h>
#include <U8x8lib.h>
#include "event.hpp"
#include "pins.hpp"

namespace
{
/* placeholder code */
uint32_t lastTick = 0;
int tick = 0;
/* end placeholder code */

const uint32_t RESET_SPLASH_TIME = 2500;
U8X8_SSD1306_128X32_UNIVISION_4W_HW_SPI oled(
    (uint8_t)Pin::OLED_CS, (uint8_t)Pin::OLED_DC, (uint8_t)Pin::OLED_RST);

uint32_t lastReset = 0;
bool splashOn = true;

void drawBootDisplay()
{
    oled.clear();
    oled.setFont(u8x8_font_8x13_1x2_r);
    oled.draw1x2String(6, 0, "BBiQ");
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
        splashOn = true;
        break;
    default:;
    }
}
} // namespace

void displaySetup()
{
    registerHandler(Event::Type::RESET, &handler);
    registerHandler(Event::Type::MODE, &handler);
    oled.begin();
    oled.setFlipMode(true);
    oled.setContrast(0);
}

void displayLoop(uint32_t *ts)
{
    if (splashOn &&
        int32_t(*ts) - int32_t(lastReset) > int32_t(RESET_SPLASH_TIME))
    {
        splashOn = false;
    }

    /* placeholder code */
    if (!splashOn && int32_t(*ts) - int32_t(lastTick) > 1000)
    {
        tick++;
        drawMainDisplay(tick, 000, 999, "Probe 1");
        lastTick = *ts;
    }
    /* end placeholder code */
}
