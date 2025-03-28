//#define DDR_KEYPAD  DDRF
//#define PORT_KEYPAD PORTF
//#define PIN_KEYPAD  PINF

const int	Key_1 = B11101110;
const int	Key_2 =	B11011110;
const int	Key_3 =	B10111110;
const int	Key_A =	B01111110;

const int	Key_4 =	B11101101;
const int	Key_5 =	B11011101;
const int	Key_6 =	B10111101;
const int	Key_B =	B01111101;

const int	Key_7 =	B11101011;
const int	Key_8 =	B11011011;
const int	Key_9 =	B10111011;
const int	Key_C =	B01111011;

const int	Key_F =	B11100111;
const int	Key_0 =	B11010111;
const int	Key_E =	B10110111;
const int	Key_D =	B01110111;

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
  if (freePinFromKeyPad == 1) //перевірка чи була натиснута кнопка
  { //якщо =1, тоді ще не натискалася
    if (PIN_KEYPAD != 0xF0)
    {
      delay(50);
      freePinFromKeyPad = 0;
      keyFromKeyPad = 1;
      // Визначення натиснутої клавіші
      // почергова подача 0V на рядки клавіатури A,B,C,D
      PORT_KEYPAD = B11111110;   // A-рядок
      asm("nop");
      if (PORT_KEYPAD == PIN_KEYPAD)
      {
        PORT_KEYPAD = B11111101; // B-рядок
        asm("nop");
        if (PORT_KEYPAD == PIN_KEYPAD)
        {
          PORT_KEYPAD = B11111011; // C-рядок
          asm("nop");
          if (PORT_KEYPAD == PIN_KEYPAD)
          {
            PORT_KEYPAD = B11110111; // D-рядок
            asm("nop");
            if (PORT_KEYPAD == PIN_KEYPAD)
              keyFromKeyPad = 0; // жодна клавіша не натиснута
          }
        }
      }
      if (keyFromKeyPad == 1) //визначення натиснутої клавіші
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

      PORT_KEYPAD = 0xF0;   //відновлюємо порт
    }
  }
  else if (PIN_KEYPAD == 0xF0)  //перевіряємо чи кнопка відпущена
  {
    delay(200);
    freePinFromKeyPad = 1;
  }
  return logicPressed;
}