extern const int buttonPin;
extern bool stopRequested;
extern unsigned long stopTime;

void setupButton() {
  pinMode(buttonPin, INPUT_PULLUP);
}

void checkButton() {
  if (digitalRead(buttonPin) == LOW) {
    stopRequested = true;
    stopTime = millis();
  }
}
