#ifndef BTN_H
#define BTN_H

#include <Arduino.h>
#define HOLD_TIME 2000

extern const uint8_t btnGPIO;
extern unsigned long lastHoldTime;
extern unsigned long lastClickTime;
extern unsigned long currentDelay;

extern bool btnHold;
extern bool isPressBtn;
extern bool algoBlink;
extern bool siteBtnPressed;
extern bool isSiteBtnHold;

void buttonHold()
{
    if (!digitalRead(btnGPIO))
    {
        if (!btnHold)
        {
            lastHoldTime = millis();
            btnHold = true;
            Serial.println("press");
        }

        if (millis() - lastHoldTime >= HOLD_TIME)
        {
            algoBlink = !algoBlink;
            Serial.println("Button is held");
        }
    }
    else
    {
        btnHold = false;
    }
}

void checkSiteBtn()
{
    static bool lastState = false;

    if (siteBtnPressed && !lastState)
    {
        algoBlink = !algoBlink;
        Serial.println(algoBlink ? "Algorithm mode activated" : "Default mode activated");
    }

    lastState = siteBtnPressed;
}

#endif