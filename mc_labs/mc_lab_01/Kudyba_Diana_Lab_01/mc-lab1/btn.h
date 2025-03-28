bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

bool readButton() {
    bool buttonState = digitalRead(BUTTON_PIN);
    if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = millis();
        lastButtonState = buttonState;
        return true;
    }
    lastButtonState = buttonState;
    return false;
}
