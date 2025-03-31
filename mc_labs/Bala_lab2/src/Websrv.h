#ifndef WEBSRV_H
#define WEBSRV_H

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "Pins.h"

extern uint32_t ledDelay;

const char* ssid = "Mi Phone2";
const char* password = "12345678";

AsyncWebServer server(80);

void changeDelay();
void sendCOM();
extern void handleButton();

void handleLEDStatus(AsyncWebServerRequest *request, int pin) {
    request->send(200, "text/plain", String(digitalRead(pin)).c_str());
}

void setupWebServer() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed");
        return;
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.on("/change_delay", HTTP_GET, [](AsyncWebServerRequest *request) {
        changeDelay();
        request->send(200, "text/plain", String(ledDelay).c_str());
    });

    server.on("/send_com", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendCOM();
        request->send(200, "text/plain", String(ledDelay).c_str());
    });

    server.on("/get_delay", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ledDelay).c_str());
    });

    server.on("/status_led_1", HTTP_GET, [](AsyncWebServerRequest *request) { handleLEDStatus(request, LED1_PIN); });
    server.on("/status_led_2", HTTP_GET, [](AsyncWebServerRequest *request) { handleLEDStatus(request, LED2_PIN); });
    server.on("/status_led_3", HTTP_GET, [](AsyncWebServerRequest *request) { handleLEDStatus(request, LED3_PIN); });   

    server.begin();
}

#endif