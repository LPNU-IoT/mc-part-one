/**
   @addtogroup LPNU IOT
   @paragraph Lab1
   @author VVA
   @brief  0 var
   @todo
   Кнопка	Світлодіоди
     PD0	  Port-1
   @attention
    Лінійка з 8-ми одноколірних світлодіодів. При натисканні
    кнопки світлодіоди почергово блимають від 0-виводу порту до 7.
    P0→P1→P2→P3→P4→P5→P6→P7
   @date 2024-02-28
*/

//-------------------------------------------------------
//     LSB  P0  P1  P2   P3   P4   P5,  P6,  P7  MSB
//-------------------------------------------------------
// PORTA = {22, 23, 24,  25,  26,  27,  28,  29};
// PORTF = {A0, A1, A2,  A3,  A4,  A5,  A6,  A7};
// PORTK = {A8, A9, A10, A11, A12, A13, A14, A15};
// PORTC = {37, 36, 35,  34,  33,  32,  31,  30};
// PORTL = {49, 48, 47,  46,  45,  44,  43,  42};
// PORTB = {53, 52, 51,  50,  10,  11,  12,  13};
// PORTG = {41, 40, 39,  xx,  xx,  4,   xx,  xx};
// PORTD = {21, 20, 19,  18,  xx,  xx,  xx,  38};
// PORTE = {0,  1,  xx,  5,   2,   3,   xx,  xx};
// PORTH = {17, 16, xx,  6,   7,   8,   9,   xx};
// PORTJ = {15, 14, xx,  xx,  xx,  xx,  xx,  xx};
//-------------------------------------------------------
#include <Arduino.h>

#ifndef __AVR_ATmega2560__
#define __AVR_ATmega2560__
#endif

#define MY_PORT PORTA
#define BUTTON_PIN 21 //PD0
#define TIMER_LEDS 400

// #define PORT_PIN_0   (1 << 0) // 0b00000001 // B 0000 0001
// #define PORT_PIN_1   (1 << 1) // 0b00000010 // B 0000 0010
// #define PORT_PIN_2   (1 << 2) // 0b00000100 // B 0000 0100
// #define PORT_PIN_3   (1 << 3) // 0b00001000 // B 0000 1000
// #define PORT_PIN_4   (1 << 4) // 0b00010000 // B 0001 0000
// #define PORT_PIN_5   (1 << 5) // 0b00100000 // B 0010 0000
// #define PORT_PIN_6   (1 << 6) // 0b01000000 // B 0100 0000
// #define PORT_PIN_7   (1 << 7) // 0b10000000 // B 1000 0000

bool btnPressedFlag;

void setupSerial()
{
  Serial.begin(9600);
  delay(200);
  Serial.println("");
  Serial.println("Setup begin");

}

void setupLedsPort()
{
  Serial.println("setupLedsPort begin");

  // for (uint8_t i = 0; i <= 7; i++)
  // {
  //     Serial.print("Setting bit ");
  //     Serial.println(DDRA, BIN);
  //     bitSet(DDRA, i);
  // } 
  //   OR JUST
  DDRA = 0b11111111;

  Serial.print("Led bits: ");
  Serial.println(DDRA, BIN);
  Serial.println("setupLedsPort end");
}

void setupBtnPort()
{
  Serial.println("setupBtnPort begin");

  uint8_t printPort = digitalPinToPort(BUTTON_PIN);
  Serial.print("Button pin port:");
  Serial.println(printPort); // 5==PORTD

  uint8_t pin_mask = digitalPinToBitMask(BUTTON_PIN);
  Serial.print("Button pin mask:");
  Serial.println(pin_mask);

  Serial.println("Setting DDRD:");
  Serial.println(DDRD, BIN);
  DDRD |= ~pin_mask; // same: DDRD = 0b11111110 // pinMode(BUTTON_PIN, OUTPUT);
  Serial.println(DDRD, BIN);

  btnPressedFlag = false;
  Serial.println("setupBtnPort end");
}

void setup()
{
  setupSerial();
  setupLedsPort();
  setupBtnPort();
  Serial.println("Setup ended");
}

void pressHandler()
{
  if (digitalRead(BUTTON_PIN))
  {
    //timer could be implemented here with millis
    Serial.println("PRESS");
    btnPressedFlag = true;
  }
  else
  {
    delay(48); // 16ms * 3ticks 
  }
}

void ledsHandler()
{
  if(btnPressedFlag)
  {
    for (uint8_t i = 0; i < 8; i ++)
    {
      bitSet(MY_PORT, i);
      Serial.println("Port state:");
      Serial.println(MY_PORT, BIN);
      delay(TIMER_LEDS);
      MY_PORT ^= 1<<i; //bit flip

      //!!!!!! IN OTHER LAB VARIANTS NO INFINITE LOOP !!!!!!!!!!!!!!!
      // btnPressedFlag = false;
    }
  }
  
}

void loop()
{
  pressHandler();
  ledsHandler();
}
