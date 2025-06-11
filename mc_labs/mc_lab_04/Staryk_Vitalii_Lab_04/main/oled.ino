#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void showConnectingAnimation() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20);
  display.print("Connected");
  display.setCursor(20, 35);
  for (int i = 0; i < 5; i++) {
    display.print(".");
    display.display();
    delay(1000);
  }
}

void showWaiting() {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(1);
  display.println("Waiting for message...");
  display.display();
}

void showMessage(String message) {
  time_t now = time(nullptr);
  struct tm* t = localtime(&now);
  char timeStr[20];
  sprintf(timeStr, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Message: ");
  display.println(message);
  display.setCursor(0, 20);
  display.print("Received at:");
  display.setCursor(0, 30);
  display.print(timeStr);
  display.display();
}
