#define LED1_PIN 0 
#define LED2_PIN 14
#define LED3_PIN 13

#define BUTTON_PIN 12

extern bool isBlinking;
extern unsigned long prevMillis;
extern const int blinkInterval;
extern unsigned long timerStart;
extern bool timerActive;
extern int ledState;  
extern int pauseState;
extern int ledCount;
extern int timer;

int stepState = 0; 

void initLEDs() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
}

void updateLEDs() {
  if (digitalRead(BUTTON_PIN) == LOW && isBlinking) {
    isBlinking = false;
    timerActive = true;
    timerStart = millis();
    pauseState = getActiveLed(); 
    Serial.println("Button pressed, pausing LEDs for 15 sec");
  }

  if (timerActive && millis() - timerStart >= timer) {
    isBlinking = true;
    timerActive = false;
    Serial.println("15 seconds passed, resuming blinking");
  }

  if (isBlinking) {
    unsigned long currentMillis = millis();
    if (currentMillis - prevMillis >= blinkInterval) {
      prevMillis = currentMillis;

      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, LOW);
      digitalWrite(LED3_PIN, LOW);

      int currentLed = getCurrentLed();
      switch (currentLed) {
        case 0: digitalWrite(LED1_PIN, HIGH); break;
        case 1: digitalWrite(LED2_PIN, HIGH); break;
        case 2: digitalWrite(LED3_PIN, HIGH); break;
      }

      stepState++;
      if (stepState > 2) {
        stepState = 0;
        ledState = (ledState + 1) % ledCount;
      }
    }
  } else {
    digitalWrite(LED1_PIN, pauseState == 0 ? HIGH : LOW);
    digitalWrite(LED2_PIN, pauseState == 1 ? HIGH : LOW);
    digitalWrite(LED3_PIN, pauseState == 2 ? HIGH : LOW);
  }
}

int getCurrentLed() {
  if (stepState == 0) {
    return ledState;
  } else if (stepState == 1) {
    if (ledState == 0) return 2; 
    if (ledState == 1) return 0; 
    if (ledState == 2) return 1; 
  } else if (stepState == 2) {
    return ledState; 
  }
  return ledState;
}

int getActiveLed() {
  return getCurrentLed();
}
