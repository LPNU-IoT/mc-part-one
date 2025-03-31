#ifndef PINS_H
#define PINS_H

#define LED1_PIN 2
#define LED2_PIN 14
#define LED3_PIN 12
#define BTN_PIN 16
#define TX 13
#define RX 15

void setupPins(){
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(BTN_PIN, INPUT);
}

#endif