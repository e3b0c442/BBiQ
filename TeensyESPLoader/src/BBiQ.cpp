#include <Arduino.h>
#include "pins.h"

#define ESP_BOOTROM_BITRATE 74480
#define ESP_NORMAL_BITRATE 115200
#define RESET_DEBOUNCE 50
#define BOOTROM_TIME 70

namespace
{
enum BBIQ_MODE
{
    BBIQ_MODE_BOOT,
    BBIQ_MODE_PROGRAM,
    BBIQ_MODE_NORMAL
};

enum BBIQ_MODE bbiqMode = BBIQ_MODE_BOOT;
uint8_t resetState = HIGH;
uint32_t resetLastChange = 0;
bool resetting = false;

uint32_t resetStart = 0;
uint32_t bootStart = 0;

uint8_t modeState = LOW;
} // namespace

void reset()
{
    //initial output settings
    modeState = digitalRead(MODE_SWITCH);
    digitalWrite(ESP_IO2, HIGH);
    digitalWrite(ESP_IO0, modeState);

    //start the Teensy<->ESP serial
    Serial1.begin(ESP_BOOTROM_BITRATE);
    while (!Serial1)
        ;

    //logo because why not
    Serial.println("   (   (      (");
    Serial.println(" ( )\\( )\\(  ( )\\");
    Serial.println(" )((_)((_)\\ )((_)");
    Serial.println("((_)((_)((_|(_)_");
    Serial.println(" | _ ) _ |_)/ _ \\");
    Serial.println(" | _ \\ _ \\ | (_) |");
    Serial.println(" |___/___/_|\\__\\_\\");
    Serial.println("BBiQ v0.1 - 2019-01-13");

    //make sure we've had 50ms since dropping
    while (millis() - resetStart < RESET_DEBOUNCE)
        ;

    //bring ESP reset high
    digitalWrite(ESP_RST, HIGH);

    //set boot mode
    bbiqMode = BBIQ_MODE_BOOT;
    bootStart = millis();
}

void setup()
{
    //start the Teensy<->USB serial
    Serial.begin(115200);
    while (!Serial && millis() < 2000)
        ;

    //initial pin modes
    pinMode(ESP_RST, OUTPUT);
    pinMode(ESP_IO0, OUTPUT);
    pinMode(ESP_UART1_TXD, OUTPUT);

    pinMode(MODE_SWITCH, INPUT_PULLUP);
    pinMode(RESET_BUTTON, INPUT_PULLUP);

    //make sure reset is low
    digitalWrite(ESP_RST, LOW);
    resetStart = millis();

    //run the reset sequence
    reset();
}

void loop()
{
    uint32_t loopStart = millis();

    uint8_t rst = digitalRead(RESET_BUTTON);
    if (rst != resetState)
    {
        resetState = rst;
        resetLastChange = loopStart;
    }
    if (!resetting && resetState == LOW && resetLastChange - loopStart > RESET_DEBOUNCE)
    {
        resetting = true;
        digitalWrite(ESP_RST, LOW);
        Serial1.end();
        Serial2.end();
        while (resetting)
        {
            resetStart = millis();
            rst = digitalRead(RESET_BUTTON);
            if (rst != resetState)
            {
                resetState = rst;
                resetLastChange = loopStart;
            }
            if (resetState == HIGH && resetLastChange - resetStart > RESET_DEBOUNCE)
            {
                resetting = false;
                reset();
            }
        }
        return;
    }

    switch (bbiqMode)
    {
    case BBIQ_MODE_BOOT:
        while (Serial.available())
            Serial1.write(Serial.read());

        while (Serial1.available())
            Serial.write(Serial1.read());

        if (loopStart - bootStart > BOOTROM_TIME)
        {
            Serial1.end();
            Serial1.begin(ESP_NORMAL_BITRATE);
            while (!Serial1)
                ;

            bbiqMode = modeState == HIGH ? BBIQ_MODE_NORMAL : BBIQ_MODE_PROGRAM;
            if (bbiqMode == BBIQ_MODE_NORMAL)
            {
                Serial2.begin(ESP_NORMAL_BITRATE);
                while (!Serial2)
                    ;
            }
        }
        break;
    case BBIQ_MODE_PROGRAM:
        while (Serial.available())
            Serial1.write(Serial.read());

        while (Serial1.available())
            Serial.write(Serial1.read());
        break;
    case BBIQ_MODE_NORMAL:
        while (Serial.available())
            Serial1.write(Serial.read());

        while (Serial1.available())
            Serial.write(Serial1.read());

        while (Serial2.available())
            Serial.write(Serial2.read());
        break;
    }
}