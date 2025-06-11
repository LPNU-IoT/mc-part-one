#include <ESP8266WebServer.h>

extern ESP8266WebServer server;
extern const char* ssid;
extern const char* password;

extern bool stopRequested;
extern unsigned long stopTime;
extern const int CMD_STOP_L;
extern const int CMD_STOP_M;

void handleRoot() {
  String html = "<html>\
  <head>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <style>\
      body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\
      .led-box { display: inline-block; width: 50px; height: 50px; margin: 10px; border-radius: 10px; border: 2px solid black; }\
      .off { background: #333; }\
      .on { background: red; }\
      button { font-size: 20px; padding: 10px; margin: 5px; background: navy; color: white; border: none; cursor: pointer; }\
      button:hover { background: darkblue; }\
    </style>\
    <script>\
      function sendCommand(cmd) {\
        var xhr = new XMLHttpRequest();\
        xhr.open('GET', '/send?cmd=' + encodeURIComponent(cmd), true);\
        xhr.send();\
      }\
      function updateLEDs() {\
        var xhr = new XMLHttpRequest();\
        xhr.open('GET', '/status', true);\
        xhr.onreadystatechange = function() {\
          if (xhr.readyState == 4 && xhr.status == 200) {\
            var activeLed = parseInt(xhr.responseText);\
            for (var i = 0; i < 3; i++) {\
              document.getElementById('led' + i).className = 'led-box ' + (i == activeLed ? 'on' : 'off');\
            }\
          }\
        };\
        xhr.send();\
      }\
      setInterval(updateLEDs, 500);\
    </script>\
  </head>\
  <body>\
    <h2>ESP_L Controller</h2>\
    <div>\
      <div id='led0' class='led-box off'></div>\
      <div id='led1' class='led-box off'></div>\
      <div id='led2' class='led-box off'></div>\
    </div>\
    <button onclick='sendCommand(65)'>Stop ESP_L</button>\
    <button onclick='sendCommand(66)'>Stop ESP_M</button>\
  </body>\
  </html>";
  server.send(200, "text/html", html);
}

void handleSendCommand() {
  if (server.hasArg("code")) {
    int code = server.arg("code").toInt();
    Serial.println("Received command via web: " + String(code));

    if (code == CMD_STOP_L) {
      stopRequested = true;
      stopTime = millis();
      server.send(200, "text/plain", "ESP_L stopped");
    } else if (code == CMD_STOP_M) {
      Serial.write(CMD_STOP_M);
      server.send(200, "text/plain", "Sent stop command to ESP_M");
    }
  } else {
    server.send(400, "text/plain", "Invalid command");
  }
}

void checkRX() {
  if (Serial.available() > 0) {
    int received = Serial.read();
    Serial.println("Received via RX: " + String(received));

    if (received == CMD_STOP_L || received == CMD_STOP_M) {
      stopRequested = true;
      stopTime = millis();
    }
  }
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void setupServer() {
  server.on("/", handleRoot);
  server.on("/send", handleSendCommand);
  server.begin();
}

void handleWebRequests() {
  server.handleClient();
}
