#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const int CMD_STOP_L = 65;
const int CMD_STOP_M = 66;

const char* ssid = "iPhone de Vitaliy";
const char* password = "iphone14";

ESP8266WebServer server(80);

const int leds[] = {2, 13, 14};
const int numLeds = sizeof(leds) / sizeof(leds[0]);
const int buttonPin = 16; 
const int ledBlinkDuration = 15000;

int currentLed = 0;
bool stopRequested = false;
unsigned long stopTime = 0;

extern void setupLeds();
extern void blinkLeds();
extern bool shouldStopLeds();

extern void setupWiFi();
extern void setupServer();
extern void handleWebRequests();
extern void checkRX();

extern void setupButton();
extern void checkButton();

void setup() {
  Serial.begin(115200);
  setupWiFi();
  setupLeds();
  setupButton();
  setupServer();
}

void loop() {
  handleWebRequests();
  checkRX();
  checkButton();

  if (!shouldStopLeds()) {
    blinkLeds();
  }
}
