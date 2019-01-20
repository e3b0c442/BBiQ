#include <Arduino.h>
#include <SPI.h>
#include <U8x8lib.h>
#include "pins.h"

static U8X8_SSD1306_128X32_UNIVISION_4W_HW_SPI oled(OLED_CS, OLED_DC, OLED_RESET);

void displaySetup()
{
    oled.begin();
    oled.setFlipMode(true);
    oled.setContrast(0);
    oled.setFont(u8x8_font_8x13_1x2_r);
    oled.draw1x2String(0, 0, "888");
    oled.drawString(4, 0, "Probe name");
    oled.drawString(4, 2, "L:000 H:000");
}

void displayLoop() {}