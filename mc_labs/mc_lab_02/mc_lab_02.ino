#include <LiquidCrystal.h>

const int ledPin =  13;     // the number of the LED pin
const int buttonPin = 22;   // the number of the button pin

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 5, rw = 6, en = 7, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);

unsigned char pwmOCR_A = 0;
unsigned char pwmOCR_B = 0;
bool isCommand = true;
unsigned char command;
bool isFanAStarted = false;
bool isFanBStarted = false;

void setup() {
  // initialize UART0
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  // initialize button pin
  pinMode(buttonPin, INPUT_PULLUP);

  //LCD 16x2
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("Start Client");

  //Timer0  -- fast PWM
  TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1) ;
//  TCCR0B = (0 << CS02) | (1 << CS01) | (1 << CS00); // prescaler=
  TCCR0B = (0 << CS02) | (1 << CS01) | (0 << CS00); // prescaler=
  OCR0A = 0;
  OCR0B = 0;

  //OC_Pins
  DDRB |= 1 << 7;
  DDRG |= 1 << 5;
}


void loop() {
  // read from port 0:
  if (Serial.available()) {
    int inByte = Serial.read();
    if (isCommand) {
      //Start FAN_A
      if (inByte == 0xA1) {
        OCR0A = pwmOCR_A;
        isFanAStarted = true;
        lcd.setCursor(0, 0);
        lcd.print("StartA OCRA=");
        lcd.print(OCR0A);
        lcd.print("   ");
      }
      //Stop FAN_A
      else if (inByte == 0xA2) {
        OCR0A = 0;
        isFanAStarted = false;
        lcd.setCursor(0, 0);
        lcd.print("StopA  OCRA=");
        lcd.print(OCR0A);
        lcd.print("   ");
      }
      //OCR_A command
      else if (inByte == 0xA3) {
        isCommand = false;
        command = inByte;
      }
      //-------------------------------
      //Start FAN_B
      if (inByte == 0xB1) {
        OCR0B = pwmOCR_B;
        isFanBStarted = true;
        lcd.setCursor(0, 1);
        lcd.print("StartB OCRB=");
        lcd.print(OCR0B);
        lcd.print("   ");
      }
      //Stop FAN_B
      else if (inByte == 0xB2) {
        OCR0B = 0;
        isFanBStarted = false;
        lcd.setCursor(0, 1);
        lcd.print("StopB  OCRB=");
        lcd.print(OCR0B);
        lcd.print("   ");
      }
      //OCR_B command
      else if (inByte == 0xB3) {
        isCommand = false;
        command = inByte;
      }
    }
    else {
      if (command == 0xA3) {
        isCommand = true;
        pwmOCR_A = inByte;
        if (isFanAStarted) {
          OCR0A = pwmOCR_A;
          lcd.setCursor(7, 0);
          lcd.print("OCRA=");
          lcd.print(OCR0A);
          lcd.print("  ");
        }
      }
      //---------------------------
      if (command == 0xB3) {
        isCommand = true;
        pwmOCR_B = inByte;
        if (isFanBStarted) {
          OCR0B = pwmOCR_B;
          lcd.setCursor(7, 1);
          lcd.print("OCRB=");
          lcd.print(OCR0B);
          lcd.print("  ");
        }
      }

    }
  }
}
