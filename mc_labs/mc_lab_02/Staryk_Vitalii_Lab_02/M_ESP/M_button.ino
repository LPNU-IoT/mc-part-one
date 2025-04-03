extern const int buttonPin;
extern bool stopRequested;
extern unsigned long stopTime;

void setupButton() {
  pinMode(buttonPin, INPUT);
}

void checkButton() {
  if (digitalRead(buttonPin) == HIGH) {
    stopRequested = true;
    stopTime = millis();
  }
}
