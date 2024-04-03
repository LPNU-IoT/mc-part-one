#include <HardwareSerial.h>

#define RX2 16
#define TX2 17
#define LED1 SCK
#define LED2 MISO
#define LED3 MOSI
#define btnGPIO1 SDA
#define btnGPIO2 A10
#define DELAY_BETWEEN_LEDS 400
#define DEBOUNCE_DELAY 80
#define UART_DELAY 50

uint32_t timestamp;
uint32_t timestamp2 = 0;
uint32_t timestamp3 = 0;
uint32_t timestamp4 = 0;
uint32_t count = 0;
uint32_t buttonCounter = 0;
uint32_t prevCount = 0;
uint8_t ledsArray[] = {LED1, LED2, LED3};
uint8_t ledLen = sizeof(ledsArray) / sizeof(ledsArray[0]);
uint8_t currentLed = 0;
uint32_t prevButtonCounter = 0;
uint8_t lastButtonState = LOW;
uint8_t buttonState;

bool algo_active = true;
bool presentState;
bool btnPressed = false;
bool msgSended = false;

void pinsSetup()
{
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(btnGPIO1, INPUT);
  pinMode(btnGPIO2, INPUT);
}

IRAM_ATTR void ISRbtn1LOW()
{
  count++;
}

IRAM_ATTR void ISRbtnChange()
{
  buttonCounter++;
}

void btnChange()
{
  if (buttonCounter > prevButtonCounter)
  {
    if (millis() - timestamp3 >= DEBOUNCE_DELAY)
    {
      timestamp3 = millis();
      prevButtonCounter = buttonCounter;
      presentState = digitalRead(btnGPIO2);
      btnPressed = (presentState == LOW) ? true : false;
      if (buttonCounter > 10000)
      {
        prevButtonCounter = 0;
        buttonCounter = 0;
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinsSetup();
  attachInterrupt(digitalPinToInterrupt(btnGPIO2), ISRbtnChange, CHANGE);
  timestamp = millis();
  Serial2.begin(115200, SERIAL_8N1, RX2, TX2);
}

void do_algo()
{
  digitalWrite(ledsArray[currentLed], HIGH);
  currentLed++;
  if (currentLed >= ledLen)
  {
    currentLed = 0;
  }
}

void checkButton()
{
  int reading = digitalRead(btnGPIO1);
  if (reading != lastButtonState)
  {
    timestamp2 = millis();
  }
  if (millis() - timestamp2 >= DEBOUNCE_DELAY)
  {
    if (reading != buttonState)
    {
      buttonState = reading;
      if (buttonState == HIGH)
      {
        algo_active = !algo_active;
      }
    }
  }
  lastButtonState = reading;
}

void loop()
{
  checkButton();
  if (millis() - timestamp4 >= UART_DELAY)
  {
    timestamp4 = millis();
    if (btnPressed)
    {
      Serial2.write(0xA1);
      msgSended = false;
    }
    else if (!btnPressed && !msgSended)
    {
      Serial2.write('\n');
      msgSended = true;
    }
  }
  if (Serial2.available() > 0)
  {
    byte some = Serial2.read();
    if (some == 0xC1)
      algo_active = false;
    if (some == 0xD1)
      algo_active = true;
  }
  btnChange();
  if (algo_active)
  {
    if (millis() - timestamp >= DELAY_BETWEEN_LEDS)
    {
      timestamp = millis();
      for (int i = 0; i < ledLen; i++)
      {
        digitalWrite(ledsArray[i], LOW);
      }
      do_algo();
    }
  }
}
