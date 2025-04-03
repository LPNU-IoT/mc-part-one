#include <WiFi.h>             
#include <WebServer.h>         

#define TX_PIN 17
#define RX_PIN 16
#define BUTTON 19
#define LEDRED 18
#define LEDYELLOW 5
#define LEDGREEN 4

#define LEDS_NUM 6
char Lcommand ='L';
String algos = "algo";
bool ledgo = true;
uint16_t countclick = 0;
uint16_t count_tab = 2;
int length = 3;
int threshold = -1;
unsigned long buttonPressMillis = 0;
const long buttonDelay = 1000;
bool is_algo_1 = false;
bool is_algo_2 = false;
bool isButtonPressedOnce = false; 
bool currentButtonState = HIGH;  
bool lastButtonState = HIGH;
byte byte_mask_led1 =0b001;
byte byte_mask_led2=0b010;
byte byte_mask_led3 =0b100;


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


const char* ssid = "ESP32_AP"; 
const char* password = "12345678"; 
String ledState = "000"; 

WebServer server(80);

void setup() {
 
  Serial.begin(115200); 
  Serial2.begin(28800, SERIAL_8O1, RX_PIN, TX_PIN); 
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LEDRED, OUTPUT);
  pinMode(LEDYELLOW, OUTPUT);
  pinMode(LEDGREEN, OUTPUT);
  Serial.println("ESP32 is ready!");

  WiFi.softAP(ssid, password);
  Serial.println("Access Point started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

 
  server.on("/", HTTP_GET, []() {
    String html = R"rawliteral(<!DOCTYPE html>
<html lang="uk">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 LED Control</title>
    <style>
        .square {
            width: 50px;
            height: 50px;
            display: inline-block;
            margin: 10px;
            border: 2px solid black;
            background-color: white; /* Додаємо білий фон */
        }
        .on {
            background-color: yellow;
        }
    </style>
</head>
<body>
    <h1>ESP32 LED Control</h1>
    <p>LED State: <span id="ledState">---</span></p>

    <div id="square1" class="square"></div>
    <div id="square2" class="square"></div>
    <div id="square3" class="square"></div>

    <button onclick="Click()">is_algo_1!</button>
    <button onclick="Click1()">is_algo_2!</button>

    <script>
        function Click() {
            fetch('/click')
                .then(response => response.text())
                .then(console.log)
                .catch(console.error);
        }

        function Click1() {
            fetch('/click1')
                .then(response => response.text())
                .then(console.log)
                .catch(console.error);
        }

        function updateLedState() {
            fetch('/state')
                .then(response => response.text())
                .then(data => {
                    document.getElementById('ledState').innerText = data;
                    
                   
                    let ledState = data.padEnd(3, '0').split('').map(Number);

                    
                    document.getElementById('square1').classList.toggle('on', ledState[0] === 1);
                    document.getElementById('square2').classList.toggle('on', ledState[1] === 1);
                    document.getElementById('square3').classList.toggle('on', ledState[2] === 1);
                })
                .catch(console.error);
        }

        setInterval(updateLedState, 1000);
    </script>
</body>
</html>)rawliteral";
    server.send(200, "text/html", html);
  });

  server.on("/state", HTTP_GET, []() {
    server.send(200, "text/plain", ledState);
  });
  server.on("/click", HTTP_GET, handle_web_click_algo1);
  server.on("/click1", HTTP_GET, handle_web_click_algo2);

  server.begin();
}

void handle_web_click_algo1() {
  is_algo_1 = true;
  is_algo_2 = false;
}

void handle_web_click_algo2() {
  is_algo_1 = false;
  is_algo_2 = true;
  Serial2.println(algos);
  Serial.println(algos);
}

void serv() {
  currentButtonState = digitalRead(BUTTON);


  if ((currentButtonState == LOW && lastButtonState == HIGH) || is_algo_1) {
    byte current_led_state = leds_states[(countclick / count_tab) % LEDS_NUM];
    ledState = String((current_led_state & byte_mask_led1) ? "1" : "0") + 
               String((current_led_state & byte_mask_led2) ? "1" : "0") + 
               String((current_led_state & byte_mask_led3) ? "1" : "0");
    countclick += count_tab;           


    isButtonPressedOnce = true;
  }

  if (currentButtonState == HIGH && lastButtonState == LOW) {
    isButtonPressedOnce = false; 
  }

 
  if (Serial2.available() > 0 || is_algo_2) {
    ledState = String(digitalRead(LEDRED)) + String(digitalRead(LEDYELLOW)) + String(digitalRead(LEDGREEN));
  }

  lastButtonState = currentButtonState;
}



void handle_buttone() {
  unsigned long currentMillis = millis();  

 if ((digitalRead(BUTTON) == LOW || is_algo_1) && (currentMillis - buttonPressMillis) >= buttonDelay){ 
    byte asciiValue = Lcommand;
    Serial2.println(asciiValue, OCT); 
    Serial.println("Sent command: L");
    ledgo = false;


    buttonPressMillis = currentMillis;

    algorithm();
    is_algo_1 = true;
    is_algo_2=false;
  }
}

void handle_comand() {
  if (Serial2.available() > 0) {
    is_algo_2= true;
    is_algo_1 = false;
    String command = Serial2.readStringUntil('\n');
    command.trim();

    
    long octalValue = strtol(command.c_str(), NULL, 8);
    char outputChar = (char)octalValue;
   
    


    if (outputChar == Lcommand) {
      ledgo = true;
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

  Serial2.print((current_led_state & byte_mask_led1) ? "1" : "0");
  Serial2.print((current_led_state & byte_mask_led2) ? "1" : "0");
  Serial2.println((current_led_state & byte_mask_led3) ? "1" : "0");


  Serial.print("Sent LED state: ");
  Serial.print((current_led_state & byte_mask_led1) ? "1" : "0");
  Serial.print((current_led_state & byte_mask_led2) ? "1" : "0");
  Serial.println((current_led_state & byte_mask_led3) ? "1" : "0");
  
}

void update_led_state() {
  ledgo = true;
  if (Serial2.available() > 0) {

    String command = Serial2.readStringUntil('\n');
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

void loop() {
  server.handleClient();
  handle_comand();
  handle_buttone();

  if (ledgo) {

    update_led_state();
    
  } else {
    digitalWrite(LEDRED, LOW);
    digitalWrite(LEDYELLOW, LOW);
    digitalWrite(LEDGREEN, LOW);
  }
  serv();
  is_algo_1=false;
}