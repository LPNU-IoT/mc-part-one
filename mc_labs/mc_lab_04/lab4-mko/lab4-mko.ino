

// include the library code:
#include "keypad4x4.h"
#include <LiquidCrystal.h>

#define MINUTES 1
#define HOURS 2

enum ClockMode {
  Time,
  Alarm
};


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const char segments[] = { 53, 52, 51, 50, 10, 11, 12, 13 };
const char numbers[] = { 22, 23, 24, 25, 26, 27 };
const char timeButton = 18;
const char alarmButton = 70;
const char setButton = 71;
const char toggleButton = 72;
const char buzzerPin = 35;


long time = 0;
long alarm = 0;
bool alarmActive = false;
bool alarmSetModeActive = false;
int inputData = 0;

SegmentDisplay seg(segments, numbers, 6);





ISR(TIMER2_OVF_vect) {
  time = (time + 1) % ((long)60 * 60 * 24);
}

ISR(TIMER1_COMPA_vect) {
  displayTime(alarmSetModeActive ? alarm : time);
  seg.update();
}





void handleButtonPress() {
  byte port_val = PIND;

  if(!(~(port_val >> 3) & 0b1111)) return;

  if(!((port_val >> 3) & 1)) {
      getTimeFromInputs(HOURS, alarmSetModeActive ? alarm : time);
  } 
  if(!((port_val >> 4) & 1)) {
    getTimeFromInputs(MINUTES, alarmSetModeActive ? alarm : time);
  }
  if(!((port_val >> 5) & 1)) 
    alarmSetModeActive = !alarmSetModeActive;
  if(!((port_val >> 6) & 1)) alarmActive = !alarmActive;

  delay(200);
}


void handleCurrentMode() {

}



void displayTime(long seconds) {
  byte segment[] = { 0, 0, 0, 0, 0, 0 };
  getSegmentFromTime(segment, seconds, alarmActive);
  seg.setValue(segment);
}



void getTimeFromInputs(char buttonMode, long& time) {
  static long hours = 0;
  static long minutes = 0;

  if(buttonMode == HOURS) 
    time = time + 3600;
  if(buttonMode == MINUTES)
    time = time +  60;    
}

void checkAlarm() {
  digitalWrite(buzzerPin, (alarmActive && ((time - alarm) % 2 == 0) && ((time - alarm) < 30) && ((time - alarm) > 0)) ? HIGH : LOW);
}



void setup() {
  Serial.begin(9600);


  DDRD &= ~0b01111000;
  PORTD |= 0b01111000;

  pinMode(buzzerPin, OUTPUT);

  noInterrupts();
  //Asynchronous Operation of Timer/Counter2
  //The CPU main clock frequency must be more than four times the oscillator frequency
  //a. Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2
  TIMSK2 = 0;

  //b. Select clock source by setting AS2 as appropriate
  //When AS2 is written to one, Timer/Counter2 is clocked from a crystal oscillator
  //connected to the timer oscillator 1 (TOSC1) pin
  ASSR |= (1 << AS2);

  //c. Write new values to TCNT2, OCR2x, and TCCR2x
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;

  //d. To switch to asynchronous operation: Wait for TCN2xUB, OCR2xUB, and TCR2xUB
  while (ASSR & 0x1F) {}

  //no need to change TCCR2A, normal counting mode
  //prescaler set to 128
  TCCR2B |= (1 << CS22) | (1 << CS20);

  //e. Clear the Timer/Counter2 interrupt flags
  TIFR2 = 0x07;

  //f. Enable interrupts, if needed
  TIMSK2 |= (1 << TOIE2);

  // dynamic indication set up with timer 2
  TCCR1A = 0x00;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);  //CTC mode & Prescaler @ 1024
  TIMSK1 = (1 << OCIE1A);                             // allow compare match mode
  OCR1A = 15624 / 512;                                     // compare value = 1/1024 sec (8MHz AVR)

  interrupts();

  seg.init();
}

void loop() {
  handleButtonPress();
  checkAlarm();
}