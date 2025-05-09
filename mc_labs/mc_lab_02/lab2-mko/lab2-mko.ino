const unsigned char BUTTON_0_PIN = 49;
const unsigned char BUTTON_1_PIN = 47;

const unsigned char LED_PINS[] = { 62, 63, 64, 65, 66, 67, 68, 69 };
const unsigned int DELAY = 800;

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  pinMode(BUTTON_0_PIN, INPUT);
  digitalWrite(BUTTON_0_PIN, HIGH);

  pinMode(BUTTON_1_PIN, INPUT);
  digitalWrite(BUTTON_1_PIN, HIGH);

  Serial.begin(9600);
}


unsigned char currentIteration = 0;
unsigned char currentMode = 0;
void bleep_sequential() {
  digitalWrite(LED_PINS[currentIteration], HIGH);
  delay(DELAY);
  digitalWrite(LED_PINS[currentIteration], LOW);
  currentIteration = (currentIteration + 1) % 8;
}

void bleep_two_converge() {
  digitalWrite(LED_PINS[currentIteration], HIGH);
  digitalWrite(LED_PINS[7 - currentIteration], HIGH);
  delay(DELAY);
  digitalWrite(LED_PINS[currentIteration], LOW);
  digitalWrite(LED_PINS[7 - currentIteration], LOW);
  currentIteration = (currentIteration + 1) % 4;
}



void loop() {
  int inByte = -1;
  if (Serial.available())
    inByte = Serial.read();

  if (currentMode == 0) {
    if (inByte == 0xA1)
      currentMode = 1;
    else if (inByte == 0xB1)
      currentMode = 2;
  }

  if (currentMode == 1)
    bleep_sequential();
  else if (currentMode == 2)
    bleep_two_converge();

  if (currentIteration == 0)
    currentMode = 0;


  if (digitalRead(BUTTON_0_PIN) == LOW)
  {
    Serial.write(0xA1);
    delay(1000);
  }
  if (digitalRead(BUTTON_1_PIN) == LOW)
  {
    Serial.write(0xB1);
    delay(1000);
  }
}
