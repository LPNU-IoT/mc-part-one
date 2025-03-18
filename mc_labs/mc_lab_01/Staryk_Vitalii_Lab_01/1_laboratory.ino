#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define BUTTON_PIN 12

bool isBlinking = true;
unsigned long prevMillis = 0;
const int blinkInterval = 500;
unsigned long timerStart = 0;
bool timerActive = false;
int ledState = 0;
int pauseState = LOW;
int ledCount = 3;
int timer = 15000;

const char* ssid = "611VVA";
const char* password = "123qwerty9";

ESP8266WebServer server(80);

void updateLEDs();
void web();
void webToggle();
void initLEDs();

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  initLEDs();

  server.on("/", web);
  server.on("/toggle", webToggle);
  server.begin();
}

void loop() {
  server.handleClient();
  updateLEDs();
}
