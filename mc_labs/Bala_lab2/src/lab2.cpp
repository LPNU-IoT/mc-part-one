#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Pins.h"
#include "Websrv.h"

void handleLeds();
void handleButton();
void changeDelay();
void sendCOM();
void checkSerial();
void changeLed(uint8_t pin1, uint8_t pin2);

uint8_t lastButtonState = 1;
uint32_t lastDebounceTime = 0;
const uint32_t debounceDelay = 50;

enum LedState
{
  LED1_ON,
  LED2_ON,
  LED3_ON
};

LedState ledState = LED1_ON;

uint32_t lastLedToggle = 0;
uint32_t ledDelay = 1000;

SoftwareSerial mySerial(TX, RX, SWSERIAL_7E1);

void setup()
{
  mySerial.begin(57600);
  Serial.begin(115200);
  setupPins();
  setupWebServer();
}

void loop()
{
  handleButton();
  handleLeds();
  checkSerial();
}

void handleButton()
{
  uint32_t tmp = millis();
  int btnVal = digitalRead(BTN_PIN);
  bool dbtn = 0;
  if (btnVal != lastButtonState)
  {
    lastDebounceTime = millis();
    dbtn = true;
  }

  if (dbtn && ((tmp - lastDebounceTime) > debounceDelay))
  {
    dbtn = false;
    tmp = 0;
    lastDebounceTime = 0;
    if (btnVal == LOW && lastButtonState == HIGH)
    {
      changeDelay();
    }
    lastButtonState = btnVal;
  }
}

void sendCOM()
{
  mySerial.write('A');
  Serial.println("Sent: A");
}

void checkSerial()
{
  if (mySerial.available())
  {
    uint8_t receivedChar = mySerial.read();
    Serial.print("Received: ");
    Serial.println((char)receivedChar);

    if (receivedChar == 'A')
    {
      Serial.println("Command Received: A");
      changeDelay();
    }
  }
}



void handleLeds()
{
  unsigned long currentMillis = millis();

  if (currentMillis - lastLedToggle >= ledDelay)
  {
    lastLedToggle = currentMillis;

    switch (ledState)
    {
    case LED1_ON:
      changeLed(LED3_PIN, LED1_PIN);
      ledState = LED2_ON;
      break;

    case LED2_ON:
      changeLed(LED1_PIN, LED2_PIN);
      ledState = LED3_ON;
      break;

    case LED3_ON:
      changeLed(LED2_PIN, LED3_PIN);
      ledState = LED1_ON;
      Serial.println("led > 2");
      break;
    }
  }
}

void changeLed(uint8_t pin1, uint8_t pin2) {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
}

void changeDelay()
{
  if (ledDelay > 100)
  {
    ledDelay -= 100;
  }
  else
  {
    ledDelay = 1000;
  }
  Serial.print("New ledDelay: ");
  Serial.println(ledDelay);
}