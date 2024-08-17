
const unsigned char BUTTON_PIN = 47;

const unsigned char LED_PINS[] = { 62, 63, 64, 65, 66, 67, 68, 69 };
const unsigned int DELAY = 800;

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}


unsigned char currentIteration = 0;
void bleep() {
  int pinToChange =  currentIteration % 2 == 0 ? currentIteration / 2 : (7 - currentIteration / 2);
  digitalWrite(LED_PINS[pinToChange], HIGH);
  delay(DELAY);
  digitalWrite(LED_PINS[pinToChange], LOW);
  currentIteration = (currentIteration + 1) % 8;
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned char temp = !digitalRead(BUTTON_PIN);

  if (!currentIteration && !temp)
    return;

  bleep();
}
