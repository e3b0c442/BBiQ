#include <Arduino.h>
#include <U8x8lib.h>
#include "event.hpp"
#include "pins.hpp"
#include "probe.hpp"
#include "ui.hpp"

namespace
{
U8X8_SSD1306_128X32_UNIVISION_4W_HW_SPI oled(
    (uint8_t)Pin::OLED_CS, (uint8_t)Pin::OLED_DC, (uint8_t)Pin::OLED_RST);

const uint8_t splash[] = {
    //col 0
    0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x00, 0x00, 0xFF, 0xFF, 0xC1, 0xC1, 0xC1, 0xC1,
    0x00, 0x00, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    //col 1
    0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC1, 0xC1, 0xE3, 0xBF, 0x1E, 0x00, 0x00, 0x00,
    0xC0, 0xC0, 0xE1, 0x61, 0x3F, 0x3E, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    //col 2
    0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x00, 0x00, 0xFF, 0xFF, 0xC1, 0xC1, 0xC1, 0xC1,
    0x00, 0x00, 0xFF, 0xFF, 0xC0, 0xC0, 0xC0, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    //col 3
    0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC1, 0xC1, 0xE3, 0xBF, 0x1E, 0x00, 0x00, 0x00,
    0xC0, 0xC0, 0xE1, 0x61, 0x3F, 0x3E, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    //col 4
    0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xE1, 0xE1, 0x00, 0x00, 0x00, 0xE0,
    0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    //col 5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
    0xF8, 0x1E, 0x06, 0x03, 0x03, 0x01, 0x01, 0x01,
    0x0F, 0x3C, 0x30, 0x60, 0x60, 0xC0, 0xC0, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    //col 6
    0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x03, 0x03, 0x06, 0x1E, 0xF8,
    0xC0, 0xC0, 0xC0, 0xE0, 0xE0, 0xF0, 0xDC, 0xCF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    //col 7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC3, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    //col 8
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0xC0, 0x40, 0x00, 0x00,
    0xC0, 0xC0, 0xC0, 0xC1, 0xCB, 0xCE, 0xC4, 0xC0,
    0x00, 0x00, 0x44, 0x4A, 0x53, 0x72, 0x00, 0x00,

    //col 9
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xE0, 0xF0, 0x10, 0x00, 0x00, 0x00, 0x80,
    0xC0, 0xC0, 0xC9, 0xCF, 0xC6, 0xC0, 0xC0, 0xC1,
    0x7E, 0x38, 0x60, 0x18, 0x7E, 0x00, 0x38, 0x44,

    //col 10
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x40, 0x00, 0x00, 0x00, 0x7C, 0xFE, 0x82,
    0xCB, 0xCE, 0xC4, 0xC0, 0xC0, 0xC0, 0xC8, 0xCF,
    0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x7E, 0x18,

    //col 11
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0xC0, 0x40, 0x00, 0x00,
    0xC7, 0xC0, 0xC0, 0xC1, 0xCB, 0xCE, 0xC4, 0xC0,
    0x24, 0x42, 0x00, 0x7E, 0x52, 0x52, 0x00, 0x00,

    //col 12
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xE0, 0xF0, 0x10, 0x00, 0x00, 0x00, 0x80,
    0xC0, 0xC0, 0xC9, 0xCF, 0xC6, 0xC0, 0xC0, 0xC1,
    0x00, 0x38, 0x44, 0x42, 0x42, 0x42, 0x3C, 0x00,

    //col 13
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xCB, 0xCE, 0xC4, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0,
    0x00, 0x7E, 0x08, 0x30, 0x7E, 0x00, 0x40, 0x00,

    //col 14
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0,
    0x40, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,

    //col 15
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void drawSplash()
{
    oled.clear();
    for (int x = 0; x < 16; x++)
        for (int y = 0; y < 4; y++)
            oled.drawTile(x, y, 1, ((uint8_t *)splash) + (x * 32 + y * 8));
}

void drawProgram()
{
    oled.clear();
    oled.setFont(u8x8_font_8x13_1x2_r);
    oled.drawString(0, 0, "ESP8266 USB Prgm");
    oled.drawString(1, 2, "Mode activated");
}

void drawNoProbe()
{
    oled.clear();
    oled.setFont(u8x8_font_8x13_1x2_r);
    oled.drawString(0, 0, "No probe ");
    oled.drawString(0, 2, "connected");
}

void drawProbe(Probe *probe)
{
    oled.clear();
    char tmp[8];
    oled.setFont(u8x8_font_8x13_1x2_r);

    __itoa((int)probe->temperature, tmp, 10);
    tmp[3] = '\0';
    oled.draw1x2String(0, 0, tmp);
    oled.drawString(4, 0, probe->name);
    __itoa((int)probe->lowAlarm, tmp, 10);
    tmp[3] = '\0';
    oled.drawString(4, 2, "L:");
    oled.drawString(6, 2, tmp);
    __itoa((int)probe->highAlarm, tmp, 10);
    tmp[3] = '\0';
    oled.drawString(10, 2, "H:");
    oled.drawString(12, 2, tmp);
}

void handler(Event *e)
{
    Serial.println("---DEBUG--- got display event");
    UIEvent *ue = (UIEvent *)e;
    if (ue->powerSave)
    {
        oled.setPowerSave(true);
        return;
    }

    Serial.println("---DEBUG--- should ok");
    oled.setPowerSave(false);
    switch (ue->screen)
    {
    case Screen::SPLASH:
        drawSplash();
        break;
    case Screen::PROGRAM:
        drawProgram();
        break;
    case Screen::PROBE:
        if (ue->probe == NULL)
            drawNoProbe();
        else
            drawProbe(ue->probe);
        break;
    default:;
    };
}
} // namespace

void displaySetup()
{
    registerHandler(Event::Type::UI, &handler);
    oled.begin();
    oled.setFlipMode(true);
    oled.setContrast(0);
}

void displayLoop(uint32_t ts)
{
}