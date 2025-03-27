#include "Pins.h"
#include "Websrv.h"
#include <ESPAsyncWebServer.h>


uint8_t lastButtonState = 1;

uint32_t lastDebounceTime = 0;
const uint32_t debounceDelay = 50;

uint32_t lastLedToggle = 0;
uint32_t ledDelay = 1000;
uint8_t ledState = 0;


void setup() {
  pinsSetup();
  setupWebServer();
}

void loop() {
  handleButton();
  handleLeds();
}

void handleButton() {
  uint32_t tmp = millis();
  int btnVal = digitalRead(BTN_PIN);
  bool dbtn=0;
  if (btnVal != lastButtonState) {
    Serial.println(btnVal);
      lastDebounceTime = millis();
      Serial.println(lastDebounceTime);
      dbtn=true;

  }

  if (dbtn && ((tmp - lastDebounceTime) > debounceDelay)) {
    dbtn=false;
    tmp = 0;
    lastDebounceTime = 0;
     Serial.println("deb time past");
      if (btnVal == LOW && lastButtonState == HIGH) {  
          Serial.println("btn pressed!");
          changeDelay();
      }
      lastButtonState = btnVal;
  }
}


void handleLeds() {
    unsigned long currentMillis = millis();
   
    if (currentMillis - lastLedToggle >= ledDelay) {
        Serial.println("leds");
        lastLedToggle = currentMillis;

        if (ledState == 0) {
            Serial.println("led1");
            digitalWrite(LED1_PIN, HIGH);
            digitalWrite(LED3_PIN, LOW);
        } else if (ledState == 1) {
            Serial.println("led2");
            digitalWrite(LED1_PIN, LOW);
            digitalWrite(LED2_PIN, HIGH);
        } else if (ledState == 2) {
            Serial.println("led3");
            digitalWrite(LED2_PIN, LOW);
            digitalWrite(LED3_PIN, HIGH);
        }

        ledState++;
        if (ledState > 2){
          ledState = 0;
          Serial.println("led > 2");
        }
    }
}

void changeDelay() {
  if (ledDelay > 100) {
        Serial.println("led change");
      ledDelay -= 100;
  } else {
      ledDelay = 1000;
  }
  Serial.print("New ledDelay: ");
  Serial.println(ledDelay);
}