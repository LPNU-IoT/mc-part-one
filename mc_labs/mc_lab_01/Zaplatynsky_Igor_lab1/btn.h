#ifndef BTN_H
#define BTN_H

#define BUTTON_PIN 12  
const int doubleClickTime = 500;

bool detectDoubleClick() {
  static unsigned long firstClickTime = 0;
  static bool buttonReleased = true;

  if (digitalRead(BUTTON_PIN) == LOW && buttonReleased) {
    buttonReleased = false;
    if (millis() - firstClickTime < doubleClickTime) {
      firstClickTime = 0;
      return true;
    } else {
      firstClickTime = millis();
    }
  } else if (digitalRead(BUTTON_PIN) == HIGH) {
    buttonReleased = true;
  }
  return false;
}

#endif
