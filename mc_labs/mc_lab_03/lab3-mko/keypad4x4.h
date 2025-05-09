#define DDR_KEYPAD DDRK
#define PORT_KEYPAD PORTK
#define PIN_KEYPAD PINK


class Keypad {
public:
  bool pressed = false;
  char d1, d2, d3, d4, dA, dB, dC, dD;
  char columns[4];
  char rows[4];
  Keypad(char d1, char d2, char d3, char d4, char dA, char dB, char dC, char dD)
    : columns({ d1, d2, d3, d4 }),
      rows({ dA, dB, dC, dD }) {
    this->d1 = d1;
    this->d2 = d2;
    this->d3 = d3;
    this->d4 = d4;
    this->dA = dA;
    this->dB = dB;
    this->dC = dC;
    this->dD = dD;
  }

  bool init() {
    for (char row : rows)
      pinMode(row, OUTPUT);
  

    for (char column : columns)
      pinMode(column, INPUT);
  }

  bool isPressed() {
    return pressed;
  }

  char readKey() {
    char* location = getPressedButtonLocation();

    if (!pressed)
      return '?';

    delay(200);
    return getKeyByLocation(location);
  }

private:
  char* getPressedButtonLocation() {
    char result[] = { 0, 0 };
    for (char rowToRead : rows) {
      for (char row : rows) {
        digitalWrite(row, HIGH);
        asm("nop");
      }
      char activeColumn;
      for (char column : columns) {
        pressed = digitalRead(column) == HIGH;
        if (pressed)
          activeColumn = column;
      }


      if (pressed) {
        result[0] = rowToRead;
        result[1] = activeColumn;
      }
    }

    return result;
  }

  char getKeyByLocation(char location[2]) {
    char key = '?';
    if (location[0] == dA && location[2] == d1) key = '7';
    if (location[0] == dB && location[2] == d1) key = '4';
    if (location[0] == dC && location[2] == d1) key = '1';
    if (location[0] == dD && location[2] == d1) key = 'F';
    if (location[0] == dA && location[2] == d2) key = '8';
    if (location[0] == dB && location[2] == d2) key = '5';
    if (location[0] == dC && location[2] == d2) key = '2';
    if (location[0] == dD && location[2] == d2) key = '0';
    if (location[0] == dD && location[2] == d3) key = '9';
    if (location[0] == dA && location[2] == d3) key = '6';
    if (location[0] == dB && location[2] == d3) key = '3';
    if (location[0] == dC && location[2] == d3) key = 'E';
    if (location[0] == dD && location[2] == d4) key = 'A';
    if (location[0] == dA && location[2] == d4) key = 'B';
    if (location[0] == dB && location[2] == d4) key = 'C';
    if (location[0] == dC && location[2] == d4) key = 'D';
    return key;
  }
};

const int Key_1 = B10111110;
const int Key_2 = B10111101;
const int Key_3 = B10111011;
const int Key_A = B11100111;

const int Key_4 = B11011110;
const int Key_5 = B11011101;
const int Key_6 = B11011011;
const int Key_B = B11010111;

const int Key_7 = B11101110;
const int Key_8 = B11101101;
const int Key_9 = B11101011;
const int Key_C = B10110111;

const int Key_F = B01111110;
const int Key_0 = B01111101;
const int Key_E = B01111011;
const int Key_D = B01110111;

unsigned char freePinFromKeyPad = 1;
unsigned char keyFromKeyPad = 1;
char pressedButton = 0;
bool logicPressed = false;

void initKeyPad() {
  DDR_KEYPAD = 0x0F;
  PORT_KEYPAD = 0xF0;
}

char readKeyFromPad4x4() {
  logicPressed = false;
  return pressedButton;
}

bool isButtonPressed() {
  if (freePinFromKeyPad == 1)  //перевірка чи була натиснута кнопка
  {                            //якщо =1, тоді ще не натискалася
    if (PIN_KEYPAD != 0xF0) {
      delay(50);
      freePinFromKeyPad = 0;
      keyFromKeyPad = 1;
      // Визначення натиснутої клавіші
      // почергова подача 0V на рядки клавіатури A,B,C,D
      PORT_KEYPAD = B11111110;  // A-рядок
      asm("nop");
      if (PORT_KEYPAD == PIN_KEYPAD) {
        PORT_KEYPAD = B11111101;  // B-рядок
        asm("nop");
        if (PORT_KEYPAD == PIN_KEYPAD) {
          PORT_KEYPAD = B11111011;  // C-рядок
          asm("nop");
          if (PORT_KEYPAD == PIN_KEYPAD) {
            PORT_KEYPAD = B11110111;  // D-рядок
            asm("nop");
            if (PORT_KEYPAD == PIN_KEYPAD)
              keyFromKeyPad = 0;  // жодна клавіша не натиснута
          }
        }
      }
      if (keyFromKeyPad == 1)  //визначення натиснутої клавіші
      {
        if (PIN_KEYPAD == Key_1) pressedButton = '1';
        else if (PIN_KEYPAD == Key_2) pressedButton = '2';
        else if (PIN_KEYPAD == Key_3) pressedButton = '3';
        else if (PIN_KEYPAD == Key_4) pressedButton = '4';
        else if (PIN_KEYPAD == Key_5) pressedButton = '5';
        else if (PIN_KEYPAD == Key_6) pressedButton = '6';
        else if (PIN_KEYPAD == Key_7) pressedButton = '7';
        else if (PIN_KEYPAD == Key_8) pressedButton = '8';
        else if (PIN_KEYPAD == Key_9) pressedButton = '9';
        else if (PIN_KEYPAD == Key_0) pressedButton = '0';
        else if (PIN_KEYPAD == Key_A) pressedButton = 'A';
        else if (PIN_KEYPAD == Key_B) pressedButton = 'B';
        else if (PIN_KEYPAD == Key_C) pressedButton = 'C';
        else if (PIN_KEYPAD == Key_D) pressedButton = 'D';
        else if (PIN_KEYPAD == Key_E) pressedButton = 'E';
        else if (PIN_KEYPAD == Key_F) pressedButton = 'F';
        logicPressed = true;
      }

      PORT_KEYPAD = 0xF0;  //відновлюємо порт
    }
  } else if (PIN_KEYPAD == 0xF0)  //перевіряємо чи кнопка відпущена
  {
    delay(200);
    freePinFromKeyPad = 1;
  }
  return logicPressed;
}
