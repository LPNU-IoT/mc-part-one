#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#define SSID "bodyaFrayer"
#define PASSWORD "12345678"
#define BLINK_INTERVAL 500
#define HOLD_INTERVAL 500
#define DEBOUNCE_TIME 50
#define DOUBLE_CLICK_TIME 300

enum class Color { RED, YELLOW, GREEN };

typedef struct led_s {
  const uint8_t pin;
  bool state;
  led_s *next;
  led_s *prev;
  Color color;
} led_t;

typedef struct button_s {
  uint8_t pin;
  bool state;
  bool wasPressed;
  uint32_t pressStartTime;
  uint32_t lastClickTime;
  uint32_t lastDebounce;
  uint8_t clickCount;
  bool hardIsHeld;
  bool webIsHeld;
  bool serialIsHeld;
  bool isPressed;
  bool lastState;
} button_t;

led_t redLED = {13, LOW, nullptr, nullptr, Color::RED};
led_t yellowLED = {12, LOW, nullptr, nullptr, Color::YELLOW};
led_t greenLED = {14, LOW, nullptr, nullptr, Color::GREEN};
led_t *currentLED = &redLED;
button_t button = {16, LOW, false, 0, 0, 0, 0, false, false, false, false, HIGH};

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

bool reverseDirection = false;
bool isRunning = false;
uint32_t previousBlinkTime = 0;

enum class Command { NONE, START, STOP };
Command webCommand = Command::NONE;

void setupLEDOrder() {
  redLED.next = &yellowLED;
  redLED.prev = &greenLED;

  yellowLED.next = &greenLED;
  yellowLED.prev = &redLED;

  greenLED.next = &redLED;
  greenLED.prev = &yellowLED;
}

void pinSetup() {
  pinMode(redLED.pin, OUTPUT);
  pinMode(yellowLED.pin, OUTPUT);
  pinMode(greenLED.pin, OUTPUT);
  pinMode(button.pin, INPUT_PULLUP);
}

void lightLEDs() {
  digitalWrite(redLED.pin, redLED.state);
  digitalWrite(yellowLED.pin, yellowLED.state);
  digitalWrite(greenLED.pin, greenLED.state);
}

void lightNextLED() {
  uint32_t now = millis();
  if (now - previousBlinkTime >= BLINK_INTERVAL) {
    previousBlinkTime = now;
    currentLED->state = LOW;

    currentLED = reverseDirection ? currentLED->prev : currentLED->next;
    currentLED->state = HIGH;

    lightLEDs();
    switch (currentLED->color) {
      case Color::RED: ws.textAll("red"); break;
      case Color::YELLOW: ws.textAll("yellow"); break;
      case Color::GREEN: ws.textAll("green"); break;
    }
  }
}

void turnOffLEDs() {
  redLED.state = LOW;
  yellowLED.state = LOW;
  greenLED.state = LOW;
  lightLEDs();
}

// --- Web control handlers ---
void handleHold(AsyncWebServerRequest *request) {
  button.webIsHeld = true;
  request->send_P(200, "text/html", "ok");
}

void handleReleased(AsyncWebServerRequest *request) {
  button.webIsHeld = false;
  request->send_P(200, "text/html", "ok");
}

void sendStartSignal(AsyncWebServerRequest *request) {
  webCommand = Command::START;
  request->send_P(200, "text/html", "ok");
}

void sendStopSignal(AsyncWebServerRequest *request) {
  webCommand = Command::STOP;
  request->send_P(200, "text/html", "ok");
}

void serverSetup() {
  WiFi.softAP(SSID, PASSWORD);
  LittleFS.begin();
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
  server.on("/hold", HTTP_GET, handleHold);
  server.on("/release", HTTP_GET, handleReleased);
  server.on("/start", HTTP_GET, sendStartSignal);
  server.on("/stop", HTTP_GET, sendStopSignal);
  server.addHandler(&ws);
  server.begin();
}

void handleButtonInput(uint32_t now) {
  bool state = digitalRead(button.pin) == LOW;
  if (state != button.lastState) {
    button.lastDebounce = now;
    button.lastState = state;
  }

  if ((now - button.lastDebounce) > DEBOUNCE_TIME) {
    if (state && !button.isPressed) {
      button.isPressed = true;
      if (now - button.lastClickTime < DOUBLE_CLICK_TIME) {
        button.clickCount++;
      } else {
        button.clickCount = 1;
      }
      button.lastClickTime = now;
    } else if (!state && button.isPressed) {
      button.isPressed = false;
    }
  }

  if (button.clickCount == 2) {
    isRunning = !isRunning;
    if (isRunning) {
      reverseDirection = !reverseDirection;
      Serial.println("Direction changed (Button)");
    }
    button.clickCount = 0;
  }
}

void checkWebCommand() {
  if (webCommand == Command::START)
  {
    isRunning = true;
    reverseDirection = !reverseDirection;
    Serial.println("Direction changed (Web)");
    webCommand = Command::NONE;
  }
   else if (webCommand == Command::STOP)
    {
    isRunning = false;
    webCommand = Command::NONE;
  }
}

void setup() {
  Serial.begin(115200);
  setupLEDOrder();
  pinSetup();
  serverSetup();
}

void loop() {
  static uint32_t now = millis();

  handleButtonInput(now);
  checkWebCommand();

  if (isRunning || button.webIsHeld || button.serialIsHeld || button.hardIsHeld)
  {
    lightNextLED();
  }
  else
  {
    turnOffLEDs();
  }
}