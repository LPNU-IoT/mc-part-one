#include <SoftwareSerial.h>


#define RX_PIN 5  // RX (D1)
#define TX_PIN 4  // TX (D2)

#define LEDRED 14    // D5
#define LEDYELLOW 12 // D6
#define LEDGREEN 13  // D7
#define LEDS_NUM 6
#define BUTTON 2
int threshold = -1;
int length = 3;
bool ledgo = true;
uint16_t countclick = 0;
uint16_t count_tab =2;
char Lcommand ='L';
unsigned long buttonPressMillis = 0;
const long buttonDelay = 1000;
SoftwareSerial espSerial(RX_PIN, TX_PIN);
String algos ="algo";
byte byte_mask_led1 =0b001;
byte byte_mask_led2=0b010;
byte byte_mask_led3 =0b100;
void setup() {
  Serial.begin(115200);      
  espSerial.begin(28800,SWSERIAL_8O1);      

  pinMode(LEDRED, OUTPUT);
  pinMode(LEDYELLOW, OUTPUT);
  pinMode(LEDGREEN, OUTPUT);

  digitalWrite(LEDRED, LOW);
  digitalWrite(LEDYELLOW, LOW);
  digitalWrite(LEDGREEN, LOW);

  Serial.println("ESP8266 готовий!");
}

enum ledStates {
  TWO_FIRST_LEDS = 0b011,
  FIRST_LED = 0b001,
  SECOND_LED = 0b010,
  THIRD_LED = 0b100,
  TWO_SECOND_LEDS = 0b110,
  TWO_THIRD_LEDS = 0b101
};

byte leds_states[LEDS_NUM] = {
    TWO_FIRST_LEDS,
    FIRST_LED,
    TWO_SECOND_LEDS,
    SECOND_LED,
    TWO_THIRD_LEDS,
    THIRD_LED
};

void update_led_state() {
  ledgo = true;
  if (espSerial.available() > 0) {

    String command = espSerial.readStringUntil('\n');
    command.trim(); 


    if (command.length() != length) {
      Serial.println("Invalid command length! Expected 3 characters.");

    }

    for (int i = 0; i < length; i++) {
      if (command[i] != '0' && command[i] != '1') {
        Serial.println("Invalid command format! Expected only '0' or '1'.");

      }
    }

    digitalWrite(LEDRED, (command[0] == '1') ? HIGH : LOW);
    digitalWrite(LEDYELLOW, (command[1] == '1') ? HIGH : LOW);
    digitalWrite(LEDGREEN, (command[2] == '1') ? HIGH : LOW);


    Serial.print("LEDs updated: ");
    Serial.print(command[0]);
    Serial.print(command[1]);
    Serial.println(command[2]);
  }
}

void handle_buttone() {
  unsigned long currentMillis = millis();  

  if (digitalRead(BUTTON) == LOW && (currentMillis - buttonPressMillis) >= buttonDelay) {

    byte asciiValue = Lcommand;
    espSerial.println(asciiValue, OCT); 
    Serial.println("Sent command: L");
    ledgo = false;

    buttonPressMillis = currentMillis;


    algorithm();
  }
}


void handle_comand() {
  if (espSerial.available()) {
    String command = espSerial.readStringUntil('\n');
    command.trim();

    Serial.print("Отримано: ");
    
    long octalValue = strtol(command.c_str(), NULL, 8);
    char outputChar = (char)octalValue;
    Serial.println(outputChar);
    
    if (outputChar == Lcommand) {
      ledgo = true;
    }
    if (command == algos) {
      byte asciiValue = Lcommand;
      espSerial.println(asciiValue, OCT); 
      Serial.println("Sent command: L");
      ledgo = false;
      
      algorithm();
    }

    if (command.length() == length && command.indexOf('1') != threshold || command.indexOf('0') != threshold) {

      
    
      if (command[0] == '0' || command[0] == '1') {
        digitalWrite(LEDRED, (command[0] == '1') ? HIGH : LOW);
      }
      if (command[1] == '0' || command[1] == '1') {
        digitalWrite(LEDYELLOW, (command[1] == '1') ? HIGH : LOW);
      }
      if (command[2] == '0' || command[2] == '1') {
        digitalWrite(LEDGREEN, (command[2] == '1') ? HIGH : LOW);
      }

      Serial.print("LEDs updated: ");
      Serial.print(command[0]);
      Serial.print(command[1]);
      Serial.println(command[2]);
    }
  }
}

void algorithm() {

  byte current_led_state = leds_states[(countclick / count_tab) % LEDS_NUM];
  espSerial.print((current_led_state & byte_mask_led1) ? "1" : "0");
  espSerial.print((current_led_state & byte_mask_led2) ? "1" : "0");
  espSerial.println((current_led_state & byte_mask_led3) ? "1" : "0");

  Serial.print("Sent LED state: ");
  Serial.print((current_led_state & byte_mask_led1) ? "1" : "0");
  Serial.print((current_led_state & byte_mask_led2) ? "1" : "0");
  Serial.println((current_led_state & byte_mask_led3) ? "1" : "0");
  countclick += count_tab;
}

void loop() {
  handle_comand(); 
  handle_buttone(); 
  if (ledgo) {

    update_led_state();
    
  } else {
    digitalWrite(LEDRED, LOW);
    digitalWrite(LEDYELLOW, LOW);
    digitalWrite(LEDGREEN, LOW);
  }
}
