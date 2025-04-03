extern const int leds[];
extern const int numLeds;
extern int currentLed;
extern bool stopRequested;
extern unsigned long stopTime;

void setupLeds() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
}

void blinkLeds() {
  digitalWrite(leds[currentLed], LOW);
  currentLed = (currentLed + 1) % numLeds;
  digitalWrite(leds[currentLed], HIGH);
  delay(500);
}

bool shouldStopLeds() {
  if (stopRequested && millis() - stopTime < ledBlinkDuration) {
    return true;
  }
  stopRequested = false;
  return false;
}
