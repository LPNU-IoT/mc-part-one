#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "btn.h"
#include "wifi.h"
#include "indexHTML.h"

#define LED1 14  // D5 (Red LED)
#define LED2 2   // D4 (Yellow LED)
#define LED3 13  // D7 (Green LED)

ESP8266WebServer server(80);
bool running = false;  
int lastLED = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  turnOffLEDs();
  connectToWiFi();
  
  // Define Web Routes
  server.on("/toggle", HTTP_GET, []() {
    running = !running;
    server.send(200, "text/plain", running ? "ON" : "OFF");
  });

  server.on("/status", HTTP_GET, []() {
    server.send(200, "text/plain", running ? "ON" : "OFF");
  });

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", htmlPage);
  });

  server.begin();
}

void loop() {
  server.handleClient();

  if (detectDoubleClick()) {
    running = !running;
  }

  if (running) {
    runLEDSequence();
  }
}

void runLEDSequence() {
  static unsigned long lastMillis = 0;
  const int interval = 500;
  int ledPins[] = {LED1, LED3, LED2};

  if (millis() - lastMillis > interval) {
    lastMillis = millis();
    digitalWrite(ledPins[lastLED], LOW);
    lastLED = (lastLED + 1) % 3;
    digitalWrite(ledPins[lastLED], HIGH);
  }
}

void turnOffLEDs() {
  int ledPins[] = {LED1, LED3, LED2};
  for (int i = 0; i < 3; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}
