#ifndef LED_LIGHTNING_H
#define LED_LIGHTNING_H
#include "pins.h"

extern void sendLedState();

uint32_t previousMillis = 0;
uint8_t currentLed = 0;

void ledLighting(uint32_t ledInterval) 
{
    uint32_t currentMillis = millis();

    if ((currentMillis - previousMillis) >= ledInterval) 
    {
        previousMillis = currentMillis;
        digitalWrite(leds[currentLed], LOW);
        currentLed = (currentLed + 1) % ledCount;
        digitalWrite(leds[currentLed], HIGH);
        sendLedState();
    }
}

#endif // LED_LIGHTNING_H