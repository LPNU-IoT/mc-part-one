#include <time.h>

void showMessage(String message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Message:");
  display.println(message);
  display.display();
}
