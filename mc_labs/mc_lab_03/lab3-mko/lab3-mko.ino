

// include the library code:
#include "keypad4x4.h"
#include <LiquidCrystal.h>

#define RESET -1
#define RETRIEVE -2

enum ClockMode {
  Time,
  Alarm,
  AlarmSetup,
  TimeSetup
};

const PROGMEM char sixty[60][2] = {
  "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
  "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
  "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
  "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
  "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
  "50", "51", "52", "53", "54", "55", "56", "57", "58", "59"
};


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int padPins[] = { 62, 63, 64, 65, 66, 67, 98, 69 };
const int rs = 22, rw = PJ6, en = 23;
const int ds[] = { 49, 48, 47, 46, 45, 44, 43, 42 };
const int buzzerPin = 24;

long time = 0;
long alarm = 0;
bool alarmActive = false;
bool previousAlarmSetFailed = false;
char mode = ClockMode::Time;
int inputData = 0;

LiquidCrystal lcd(rs, en, ds[0], ds[1], ds[2], ds[3], ds[4], ds[5], ds[6], ds[7]);

ISR(TIMER1_COMPA_vect) {
  time++;
}

String makeStr(String str) {
  while(str.length() < 16) {
    str += ' ';
  }
  return str;
}

String secondsToTime(long time) {
  String timeFormat = "00:00:00";

  long hoursMinutesSeconds[] = { (time / (60 * 60)) % 24, (time / 60) % 60, time % 60 };

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      timeFormat[i * 3 + j] = (char)pgm_read_byte(&(sixty[hoursMinutesSeconds[i]][j]));
    }
  }

  return timeFormat;
}

void handleButtonPress() {
  if (isButtonPressed()) {
    char value = readKeyFromPad4x4();
    switch (value) {
      case 'A':
        mode = ClockMode::TimeSetup;
        getTime(RESET);
        break;
      case 'B':
        mode = ClockMode::AlarmSetup;
        getTime(RESET);
        break;
      case 'C':
        mode = ClockMode::Alarm;
        break;
      case 'D':
        if( mode == ClockMode::Alarm)
        alarmActive = !alarmActive;
        break;
      case 'E':
        if(mode == ClockMode::TimeSetup) {
          mode = ClockMode::Time;
          time = getTime(RETRIEVE);  
        } else if(mode == ClockMode::AlarmSetup) {
          mode = ClockMode::Alarm;
          alarm = getTime(RETRIEVE);  
        }    
        break;
      case 'F':
        mode = ClockMode::Time;
        break;
      default:
        if(mode != ClockMode::TimeSetup &&
           mode != ClockMode::AlarmSetup) break;
        
        getTime(int(value - '0'));
        break;
    }
  }
}

void displayTime(long seconds) {
  lcd.setCursor(0, 1);

  // print the number of seconds since reset:
  lcd.print(makeStr(secondsToTime(seconds)));
}


long getTime(int inputNumber) {
  static int currentValue[] = { 0, 0, 0, 0, 0, 0 };
  static int currentSymbol = 0;

  if(inputNumber == RESET) {
    currentSymbol = 0;
    for(int i = 0; i < 6; i++)
      currentValue[i] = 0;
    
    displayTime(0);
    return 0;
  } 
  
  int previousNumber = currentValue[currentSymbol];
  if(inputNumber != RETRIEVE) 
    currentValue[currentSymbol] = inputNumber;
  

  long hours = (currentValue[0] * 10 + currentValue[1]);
  long minutes = (currentValue[2] * 10 + currentValue[3]);
  long seconds = (currentValue[4] * 10 + currentValue[5]);

  if(hours >= 24 || minutes >= 60 || seconds >= 60) {
    currentValue[currentSymbol] = previousNumber;   
    return -1;
  }

  long currentTime = (hours * 3600) + (minutes * 60) + seconds;

  if(inputNumber != RETRIEVE) {
    displayTime(currentTime);
    currentSymbol = (++currentSymbol) % 6;
  }

  return currentTime;
}

void checkAlarm() {
  digitalWrite(buzzerPin, (alarmActive && ((time - alarm) % 2 == 0) && ((time - alarm) < 30) && ((time - alarm) > 0)) ? HIGH : LOW);
}

void handleCurrentMode() {
  lcd.setCursor(0, 0);
  switch (mode) {
    case ClockMode::TimeSetup:
      lcd.print(makeStr("set time:"));
      break;
    case ClockMode::AlarmSetup:
      lcd.print(makeStr("set alarm:"));
      break;
    case ClockMode::Alarm:
      lcd.print(makeStr("alarm" + String(alarmActive ? " on" : " off") + ":"));
      displayTime(alarm);
      break;
    case ClockMode::Time:
      lcd.print(makeStr("time" + String(alarmActive ? ", alarm on" : "") + ":"));
      displayTime(time);
      break;
  }
}


void setup() {

  initKeyPad();

  noInterrupts();

  // Таймер#1: Скид при співпадінні OCR1A (1sec)
  TCCR1A = 0x00;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);  //CTC mode & Prescaler @ 1024
  TIMSK1 = (1 << OCIE1A);                             // дозвіл на переривання по співпадінню
  OCR1A = 0x3D08;                                     // compare value = 1 sec (16MHz AVR)

  interrupts();

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  checkAlarm();
  handleButtonPress();
  handleCurrentMode();
}