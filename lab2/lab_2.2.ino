#include <Arduino.h>
#include "pins.h"
#include "web_server.h"
#include "led_lightning.h"

uint32_t interval = 100;

bool lastButtonState = HIGH;
uint32_t lastDebounceTime = 0;
uint32_t debounceDelay = 50;

void increaseInterval() 
{
    interval += 100;
    Serial.println("Interval increased: " + String(interval) + " ms");
}

void initPins() 
{
    pinMode(buttonPin, INPUT);
        for (int i = 0; i < ledCount; i++) 
        {
            pinMode(leds[i], OUTPUT);
            digitalWrite(leds[i], LOW);
        }
}

void IRAM_ATTR handleButtonPress() 
{
    uint32_t currentMillis = millis();
    if ((currentMillis - lastDebounceTime) > debounceDelay) 
    {
        increaseInterval();
        lastDebounceTime = currentMillis;
    }
}

void readSerialMonitor() 
{
  if (Serial2.available() > 0) 
  {
    char command = Serial2.read();
    if (command == 'С') 
    {
      increaseInterval();
      Serial2.println('R');
      Serial.println("send response");
    } 
    else if (command == 'R') 
    {
      increaseInterval();
      Serial.println("Set interval");
    } 
    else 
    {
      Serial.print("Unknow command: ");
      Serial.println(command);
    }
  }
}

void setup() 
{
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8E2, 16, 17);
    initPins();
    initWebServer();
    attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING);
}

void loop() 
{
    server.handleClient();
    readSerialMonitor();
    ledLighting(interval);
}

