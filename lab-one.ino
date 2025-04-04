#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#define LEDRED 16
#define LEDYELLOW 4
#define LEDGREEN 5
#define BUTTONE 14
#define LEDS_NUM 6

WebServer server(80);

bool web_click_block = false; 
uint16_t countclick = 0;
bool last_buttone_state = HIGH;
uint32_t last_buttone_press = 0;
const uint32_t debounce_dalay = 50;
// qwiq action
bool buttone_state = HIGH;
uint32_t last_debounce_time = 0;
const uint32_t double_click_dalay = 300;
bool waiting_second_click = false;

const uint32_t block_timeout = 5000;
uint32_t last_action_time = 0;

enum lesStates
{
  TWO_FIRST_LEDS = 0b011,
  FIRST_LED = 0b001,
  SECOND_LED = 0b010,
  THIRD_LED = 0b100,
  TWO_SECOND_LEDS = 0b110,
  TWO_THIRD_LEDS = 0b101
};

// зроби форк і закинь в репрозиторій на мс парт 1 
byte leds_states[LEDS_NUM] =
    {
      TWO_FIRST_LEDS,
      FIRST_LED,
      TWO_SECOND_LEDS,
      SECOND_LED,
      TWO_THIRD_LEDS,
      THIRD_LED
};


void update_led_state()
{
  byte current_led_state = leds_states[(countclick / 2) % sizeof(leds_states)];
  digitalWrite(LEDRED, (current_led_state & 0b001) ? HIGH : LOW);
  digitalWrite(LEDYELLOW, (current_led_state & 0b010) ? HIGH : LOW);
  digitalWrite(LEDGREEN, (current_led_state & 0b100) ? HIGH : LOW);
}



void handle_web_click()
{
  if (!web_click_block)
  {
    countclick += 2;
    update_led_state();
  }
}

// http://192.168.4.1
void handleroot()
{
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP32 LED Control</title>
        <script>
        function Click() {
            fetch('/click')  
                .then(response => response.text())
                .then(data => {
                    console.log(data);
                })
                .catch(error => console.error('Error:', error));
        }
        </script>
    </head>
    <body>
        <h1>Click</h1>
        <button onclick='Click()'>Click Me!</button>
    </body>
    </html>
    )rawliteral";

  server.send(200, "text/html", html);
}

void setup()
{
  Serial.begin(115200);

  pinMode(BUTTONE, INPUT);
  pinMode(LEDRED, OUTPUT);
  pinMode(LEDYELLOW, OUTPUT);
  pinMode(LEDGREEN, OUTPUT);
 

  WiFi.softAP("XDlab1LoL", "shubak1982");
   server.begin();
  server.on("/", HTTP_GET, handleroot);
  server.on("/click", HTTP_GET, handle_web_click);
  
}

void handleButton()
{
  bool reading = digitalRead(BUTTONE);
  if (reading != last_buttone_state)
  {
    last_debounce_time = millis();
  }
  if ((millis() - last_debounce_time) > debounce_dalay)
  {
    if (reading != buttone_state)
    {
      buttone_state = reading;
      if (buttone_state == LOW)
      {
        uint32_t curraent_milis = millis();
        if (waiting_second_click && (curraent_milis - last_buttone_press < double_click_dalay))
        {
          countclick += 2;
          web_click_block = true;
          waiting_second_click = false;
          update_led_state();
        }
        else
        {
          waiting_second_click = true;
          last_buttone_press = curraent_milis;
        }
        last_action_time = millis();
      }
    }
  }
  last_buttone_state = reading;
   if (web_click_block && (millis() - last_action_time > block_timeout))
  {
    web_click_block = false;
  }
}

void loop()
{
  server.handleClient();
  handleButton();
}
