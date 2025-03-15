#ifndef WIFI_H
#define WIFI_H

#include <ESPAsyncWebServer.h>
AsyncWebServer server(80); // Порт 80 для HTTP-з'єднання

#ifndef ESP_WIFI_MODE
#define ESP_WIFI_MODE 1
#endif

#ifndef SSID
#define SSID "admin"
#endif

#ifndef PASSWORD
#define PASSWORD "domestos1216"
#endif

#define HOST_NAME "lvivske1715"

extern bool siteBtnPressed;
void notFound(AsyncWebServerRequest *request);

uint8_t initWiFi()
{
    if (ESP_WIFI_MODE == 1)
    {
        WiFi.mode(WIFI_STA);
        // Connect to Wi-Fi network with SSID and password
        WiFi.begin(SSID, PASSWORD);

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            Serial.println("Connecting to WiFi..");
        }
        Serial.println();
        Serial.print("ESP IP Address: http://");
        Serial.println(WiFi.localIP());

        Serial.print("RRSI: ");
        Serial.println(WiFi.RSSI());
    }
    else if (ESP_WIFI_MODE == 2)
    {
        WiFi.mode(WIFI_AP);
        Serial.println("Setting AP (Access Point)…");
        // Remove the password parameter, if you want the AP (Access Point) to be open
        WiFi.softAP(SSID, NULL);

        IPAddress IP = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(IP);
    }
    else
    {
        WiFi.mode(WIFI_OFF);
        Serial.println("Wifi of");
        return -1;
    }
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html); });

    server.on("/on_alg1", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    siteBtnPressed = true;
    Serial.println("site bnt true");
    request->send(200, "text/plain", "ok"); });

    server.on("/off_alg1", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    siteBtnPressed = false;

    Serial.println("site bnt false");

    request->send(200, "text/plain", "ok"); });

    server.onNotFound(notFound);
    server.begin();

    return 0;
}

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void InitMDNS()
{
    if (!MDNS.begin(HOST_NAME))
    {
        Serial.println("Error starting mDNS");
    }
    Serial.println("mDNS started");
}

#endif