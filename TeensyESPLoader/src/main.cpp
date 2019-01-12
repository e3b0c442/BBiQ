#include <Arduino.h>

#define ESP_BOOT_MODE 6
#define ESP_RESET_BUTTON 7
#define ESP_RST 3
#define ESP_IO0 2

#define ESP_BOOTROM_BITRATE 74880
#define ESP_NORMAL_BITRATE 115200
#define TEENSY_BITRATE 115200

typedef enum _SERIAL_MODE
{
  SERIAL_BOOTROM = ESP_BOOTROM_BITRATE,
  SERIAL_NORMAL = ESP_NORMAL_BITRATE
} SERIAL_MODE;

//ESP (re)boot control
unsigned long bootDelay = 100;
unsigned long serialDelay = 70;
unsigned long bootTriggered = 0;
SERIAL_MODE serialMode = SERIAL_BOOTROM;
uint8_t resetState = LOW;
unsigned long lastPing = 1000;

void setup()
{
  //Set up the pins
  pinMode(ESP_RST, OUTPUT);
  pinMode(ESP_IO0, OUTPUT);
  pinMode(ESP_RESET_BUTTON, INPUT_PULLUP);
  pinMode(ESP_BOOT_MODE, INPUT_PULLUP);
  digitalWrite(ESP_RST, resetState);
  digitalWrite(ESP_IO0, digitalRead(ESP_BOOT_MODE));

  //Start the Teensy USB serial
  Serial.begin(TEENSY_BITRATE);

  //Start the Teensy<->ESP serial
  Serial1.begin(serialMode);

  while (!Serial)
    ;
  Serial.print("Welcome to the Teensy ESP loader. Teensy<->ESP bitrate set to ");
  Serial.print(ESP_NORMAL_BITRATE);
  Serial.print("\n");
  bootTriggered = millis();
}

void loop()
{
  unsigned long loopStart = millis();
  if (resetState == LOW && loopStart - bootTriggered > bootDelay)
  {
    uint8_t bootMode = digitalRead(ESP_BOOT_MODE);
    Serial.print("Booting ESP in");
    if (bootMode == HIGH)
    {
      Serial.print(" normal ");
    }
    else
    {
      Serial.print(" programming ");
    }
    Serial.println("mode...");
    resetState = HIGH;
    serialMode = SERIAL_BOOTROM;
    Serial1.end();
    Serial1.begin(serialMode);
    digitalWrite(ESP_RST, resetState);
    digitalWrite(ESP_IO0, bootMode);
    while (!Serial1)
      ;
  }
  if (resetState == HIGH && serialMode == SERIAL_BOOTROM && loopStart - bootTriggered > bootDelay + serialDelay)
  {
    serialMode = SERIAL_NORMAL;
    Serial1.end();
    Serial1.begin(serialMode);
    while (!Serial1)
      ;
    Serial.print("Changed bitrate to ");
    Serial.print(serialMode);
    Serial.println("");
  }
  if (digitalRead(ESP_RESET_BUTTON) == LOW)
  {
    bootTriggered = loopStart;
    resetState = LOW;
    digitalWrite(ESP_RST, resetState);
  }
  /*if (loopStart - lastPing > 10000 && serialMode == SERIAL_NORMAL)
  {
    lastPing = loopStart;
    Serial1.println("{\"id\": \"14916\", \"method\": \"Dummy\"}");
  }*/

  if (Serial1.available())
  {
    Serial.write(Serial1.read());
  }

  if (Serial.available())
  {
    Serial1.write(Serial.read());
  }
}