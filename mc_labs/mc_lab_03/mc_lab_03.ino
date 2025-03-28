#include <LiquidCrystal.h>

#define DDR_KEYPAD  DDRF
#define PORT_KEYPAD PORTF
#define PIN_KEYPAD  PINF
#include "keypad4x4.h"

const int buzzerPin = 21;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 22, rw = 23, en = 24, d4 = 26, d5 = 27, d6 = 28, d7 = 29;
LiquidCrystal lcd(rs, rw, en, d4, d5, d6, d7);

const PROGMEM  char sixty[60][3] = {
  {"00"}, {"01"}, {"02"}, {"03"}, {"04"}, {"05"}, {"06"}, {"07"}, {"08"}, {"09"},
  {"10"}, {"11"}, {"12"}, {"13"}, {"14"}, {"15"}, {"16"}, {"17"}, {"18"}, {"19"},
  {"20"}, {"21"}, {"22"}, {"23"}, {"24"}, {"25"}, {"26"}, {"27"}, {"28"}, {"29"},
  {"30"}, {"31"}, {"32"}, {"33"}, {"34"}, {"35"}, {"36"}, {"37"}, {"38"}, {"39"},
  {"40"}, {"41"}, {"42"}, {"43"}, {"44"}, {"45"}, {"46"}, {"47"}, {"48"}, {"49"},
  {"50"}, {"51"}, {"52"}, {"53"}, {"54"}, {"55"}, {"56"}, {"57"}, {"58"}, {"59"}
};

struct Time
{
  unsigned char second, minute, hour;
};

Time TClock = {0, 0, 0};
Time TAlarm = {0, 0, 0};
char isAlarmOn ='f';
char displayTime = 't';
char isButtonCPressed = 'f';
char isButtonAPressed = 'f';
char isButtonBPressed = 'f';
short counter = 0;
char arr[4] = {};

void clearTime()
{
  counter = 0;
  arr[0] = '0';
  arr[1] = '0';
  arr[2] = '0';
  arr[3] = '0';
}

void LCD_WriteStrPROGMEM(char *str, int n)  //вивід масиву символів,
{ //записаних у флеші
  for (int i = 0; i < n; i++)
    lcd.print( (char)pgm_read_byte( &(str[i]) ) );
}

void coutTime(Time* FuncTime)
{
  lcd.setCursor(3, 0);
  LCD_WriteStrPROGMEM(sixty[FuncTime->hour], 2);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[FuncTime->minute], 2);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[FuncTime->second], 2);
}

void countTimeForChange() {
  lcd.write(arr[0]);
  lcd.write(arr[1]);
  lcd.write(':');
  lcd.write(arr[2]);
  lcd.write(arr[3]);
  lcd.write(':');
  LCD_WriteStrPROGMEM(sixty[TClock.second], 2);
}

ISR(TIMER3_COMPA_vect)  // Таймер Т1 по співпадінню А, кожної 1 сек.
{
  if (++TClock.second == 60)
  {
    TClock.second = 0;
    if (++TClock.minute == 60)
    {
      TClock.minute = 0;
      if (++TClock.hour == 24)
        TClock.hour = 0;
    }
  }
  if (displayTime == 't')
  {
    coutTime(&TClock);
  }
  isItAlarm();
}

void setTime(char arr[], Time* FuncTime)
{
  unsigned char hour = (arr[0] - 48) * 10 + (arr[1] - 48);
  unsigned char minute = (arr[2] - 48) * 10 + (arr[3] - 48);
  if (hour < 24 && minute < 60) {
    FuncTime->hour = hour;
    FuncTime->minute = minute;
  }
  else 
  {
    lcd.setCursor(0,1);
    lcd.print("   Incorrect!   ");
    delay(2000);
  }
}

void isItAlarm()
{
  if (isAlarmOn == 't' && TClock.hour == TAlarm.hour && TClock.minute == TAlarm.minute) 
  {
    for (int i = 15; i >= 0; i--)
    {
      delay(2000);
      digitalWrite(buzzerPin, HIGH);
      delay(2000);
      digitalWrite(buzzerPin, LOW);
    }
  }
}

void disableAllButtons()
{
  isButtonCPressed = 'f';
  isButtonAPressed = 'f';
  isButtonBPressed = 'f';
}

void setup() 
{
  noInterrupts();           // disable all interrupts

  // Таймер#1: Скид при співпадінні OCR1A (1sec) + дільник=256
  TCCR3A = 0x00;
  TCCR3B = (1 << WGM32) | (1 << CS32) | (1 << CS30); //CTC mode & Prescaler @ 1024
  TIMSK3 = (1 << OCIE3A); // дозвіл на переривання по співпадінню
  OCR3A = 0x3D08;// compare value = 1 sec (16MHz AVR)
  
  //KeyPad 4x4
  initKeyPad();

  //LCD 16x2
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  interrupts();  // Enable global interrupts

  //Buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
    if ( isButtonPressed() ) 
    {
      digitalWrite(buzzerPin, HIGH);
      lcd.setCursor(0, 1);
      //lcd.write(readKeyFromPad4x4());
      delay(100);
      digitalWrite(buzzerPin, LOW);

      switch(readKeyFromPad4x4()) 
      {
        case 'A':
          disableAllButtons();
          clearTime();
          lcd.clear();
          isButtonAPressed = 't';
          lcd.setCursor(0,1);
          lcd.print("Ins: ");  
          clearTime();
          break;

        case 'B':
          disableAllButtons();
          clearTime();
          lcd.clear();
          isButtonBPressed = 't';
          lcd.setCursor(0,1);
          lcd.print("Ins al: "); 
          clearTime();
          break;

        case 'C':
          disableAllButtons();
          clearTime();
          displayTime = 'f';
          isButtonCPressed = 't';
          lcd.clear();
          coutTime(&TAlarm);
          lcd.setCursor(1,1);
          if (isAlarmOn == 't') lcd.print("On ");
          else if (isAlarmOn == 'f') lcd.print("Off");
          break;

        case 'D':
          if (isButtonCPressed == 't')
          {
            lcd.setCursor(1,1);
            if (isAlarmOn == 'f') 
            { 
            isAlarmOn = 't';
            lcd.print("On ");
            }

            else if (isAlarmOn == 't') 
            {
            isAlarmOn = 'f';
            lcd.print("Off");
            }
          }
          break;
          
        case 'E':
          if (isButtonAPressed == 't')
          {
            setTime(arr, &TClock);
            clearTime();
            lcd.clear();
            disableAllButtons();
          }
          else if (isButtonBPressed == 't')
          {
            setTime(arr, &TAlarm);
            clearTime();
            lcd.clear();
            disableAllButtons();
          }
          break;

        case 'F':
          lcd.clear();
          displayTime = 't';
          clearTime();
          disableAllButtons();
          break;

        default:
          if (isButtonAPressed == 't')
          {
            lcd.setCursor(5, 1);
            if (counter < 4)
            {  
              arr[counter] = readKeyFromPad4x4();
              counter ++;             
            }
            else 
            { 
              counter = 0;
              arr[counter] = readKeyFromPad4x4();
              counter ++;
            }
            countTimeForChange();
          }
          else if (isButtonBPressed == 't')
          {
            lcd.setCursor(8, 1);
            if (counter < 4)
            {  
              arr[counter] = readKeyFromPad4x4();
              counter ++;             
            }
            else 
            { 
              counter = 0;
              arr[counter] = readKeyFromPad4x4();
              counter ++;
            }
            countTimeForChange();
          }
          break;
      }
    }
}