#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

extern bool isBlinking;
extern unsigned long timerStart;
extern bool timerActive;
extern int pauseState;
extern int ledState;
extern int ledCount;
extern int timer;

void web() {
  String html = "<html>\
  <head>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <style>\
      body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\
      button { font-size: 20px; padding: 10px 20px; background: navy; color: white; border: none; cursor: pointer; }\
      button:hover { background: darkblue; }\
    </style>\
    <script>\
      function toggleLED() {\
        var xhr = new XMLHttpRequest();\
        xhr.open('GET', 'toggle', true);\
        xhr.send();\
      }\
    </script>\
  </head>\
  <body>\
    <h2>ESP Pushbutton Web Server</h2>\
    <button onclick='toggleLED()'>LED PUSHBUTTON</button>\
  </body>\
  </html>";

  server.send(200, "text/html", html);
}

void webToggle() {
  isBlinking = !isBlinking;

  if (!isBlinking) {
    timerActive = true;
    timerStart = millis();
    pauseState = ledState;

    Serial.println("Web button pressed, pausing LEDs for 15 sec");
  }

  server.send(200, "text/plain", "Toggled");
}
