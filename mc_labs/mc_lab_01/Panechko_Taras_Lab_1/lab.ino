#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Galaxy S20 FE 5G";
const char* password = "0985211834";

ESP8266WebServer server(80);

const int ledPins[] = {13, 0, 2}; // LEDs on pins 13, 0, 2
const int buttonPin = 15; // Button on pin 15

const int ledInterval = 500; // Time between LED changes
const int debounceTime = 50; // Button debounce time
const int doubleClickTime = 300; // Time for double click

int currentLedIndex = 0;
bool reverseDirection = false;
bool isRunning = false;
bool buttonIsPressed = false;

unsigned long lastDebounce = 0;
unsigned long lastClick = 0;
unsigned long lastWebClick = 0;

int clickCount = 0;
int webClickCount = 0;

void setup() {
  WiFi.begin(ssid, password);
  Serial.begin(115200);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nIP Address: " + WiFi.localIP().toString());

  pinMode(buttonPin, INPUT_PULLUP);
  for (int ledPin : ledPins) {
    pinMode(ledPin, OUTPUT);
  }

  server.on("/", handleWebPage);
  server.on("/click", handleButtonClick);
  server.begin();
}

void loop() {
  server.handleClient();
  handleButtonInput();
  handleDoubleClick();
  updateLeds();
}

void handleWebPage() {
  String html = "<html><head><style>body {
   display: flex;
   flex-direction: column;
    align-items: center;
    font-family: sans-serif; }</style>
    </head><body><h1>LED Control</h1>
    <button onclick=\"fetch('/click')\">Click</button>
    </body></html>";
  server.send(200, "text/html", html);
}

void handleButtonClick() {
  unsigned long currentTime = millis();

  if (currentTime - lastWebClick < doubleClickTime) {
    webClickCount++;
  } else {
    webClickCount = 1;
  }
  lastWebClick = currentTime;

  if (webClickCount == 2) {
    isRunning = !isRunning;
    if (isRunning) {
      reverseDirection = !reverseDirection;
      Serial.println("Direction changed (Web)");
    }
    webClickCount = 0;
  }
  server.send(200, "text/plain", "OK");
}

void handleButtonInput() {
  unsigned long currentTime = millis();
  int buttonState = digitalRead(buttonPin);
  static bool lastButtonState = HIGH;

  if (buttonState != lastButtonState) {
    lastDebounce = currentTime;
    lastButtonState = buttonState;
  }

  if ((currentTime - lastDebounce) > debounceTime) {
    if (buttonState == LOW && !buttonIsPressed) {
      buttonIsPressed = true;
      if (currentTime - lastClick < doubleClickTime) {
        clickCount++;
      } else {
        clickCount = 1;
      }
      lastClick = currentTime;
      lastButtonState = HIGH;
    } else if (buttonState == HIGH && buttonIsPressed) {
      buttonIsPressed = false;
    }
  }
}

void handleDoubleClick() {
  if (clickCount == 2) {
    isRunning = !isRunning;
    clickCount = 0;
    if (isRunning) {
      reverseDirection = !reverseDirection;
      Serial.println("Direction changed (Button)");
    }
  }
}

void updateLeds() {
  if (!isRunning) {
    for (int ledPin : ledPins) {
      digitalWrite(ledPin, LOW);
    }
    return;
  }

  unsigned long currentTime = millis();
  static unsigned long lastLedChange = 0;

  if (currentTime - lastLedChange > ledInterval) {
    lastLedChange = currentTime;

    digitalWrite(ledPins[currentLedIndex], LOW);

    if (reverseDirection) {
      currentLedIndex = (currentLedIndex - 1 + 3) % 3;
    } else {
      currentLedIndex = (currentLedIndex + 1) % 3;
    }

    digitalWrite(ledPins[currentLedIndex], HIGH);
  }
}