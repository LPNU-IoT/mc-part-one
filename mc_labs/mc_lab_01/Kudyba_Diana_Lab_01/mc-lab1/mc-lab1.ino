#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "wifi.h"
#define BUTTON_PIN 14  // Визначаємо кнопку перед підключенням btn.h 
#include "btn.h"
#include "indexHtml.h"

#define LED_PINS {0, 2, 12} 

ESP8266WebServer server(80);
int leds[] = LED_PINS;
int numLeds = sizeof(leds) / sizeof(leds[0]);
bool direction = true;
int ledIndex = 0; 

int ledPatterns[3][2] = {{2, 12}, {2, 0}, {0, 12}};
int patternIndex = 0;

void handleButtonClick() {
    direction = !direction;  // Зміна напрямку
    server.send(200, "text/plain", "Button Clicked");
}

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected!");
    Serial.println(WiFi.localIP());

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    for (int i = 0; i < numLeds; i++) {
        pinMode(leds[i], OUTPUT);
        digitalWrite(leds[i], LOW);
    }

    server.on("/", []() { server.send(200, "text/html", webPage); });
    server.on("/toggle", handleButtonClick);
    server.begin();
}

void loop() {
    server.handleClient();
    
    if (readButton()) {
        direction = !direction;
    }

    // Вимикаємо всі світлодіоди
    for (int i = 0; i < numLeds; i++) {
        digitalWrite(leds[i], LOW);
    }

    digitalWrite(ledPatterns[patternIndex][0], HIGH);
    digitalWrite(ledPatterns[patternIndex][1], HIGH);

    if (direction) {
        patternIndex = (patternIndex + 1) % 3;
    } else {
        patternIndex = (patternIndex - 1 + 3) % 3;
    }

    delay(300);
}
