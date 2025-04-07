#ifndef WEB_SWERVER_H
#define WEB_SERVER_H

#include <WiFi.h>
#include <WebServer.h>

extern void increaseInterval();
extern uint8_t currentLed;

WebServer server(80);

const char* ssid = "ESP32";
const char* password = "12345678";

void handleRoot() 
{
    server.send(200, "text/html", R"rawliteral(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>ESP32 LED Control</title>
            <body>
                <h1>ESP32 LED Control</h1>
                <button onclick="sendCommand('algo1')">Algorythm 1</button>
                <button onclick="sendCommand('algo2')">Algorythm 2</button>
    
                <div class="led-container">
                    <div id="led1" class="led"></div>
                    <div id="led2" class="led"></div>
                    <div id="led3" class="led"></div>
                </div>
            </body>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    text-align: center;
                    background-color: #f4f4f9;
                }
                h1 {
                    color: #333;
                }
                button {
                    background-color: #007BFF;
                    color: white;
                    border: none;
                    padding: 15px 32px;
                    margin: 10px;
                    font-size: 16px;
                    cursor: pointer;
                    border-radius: 8px;
                    transition: background-color 0.3s;
                }
                button:hover {
                    background-color: #0056b3;
                }
                .led-container {
                    display: flex;
                    justify-content: center;
                    gap: 10px;
                    margin-top: 20px;
                }
                .led {
                    width: 50px;
                    height: 50px;
                    background-color: gray;
                    border-radius: 5px;
                    display: inline-block;
                    transition: background-color 0.3s;
                }
            </style>
            <script>
                const led1 = document.getElementById('led1');
                const led2 = document.getElementById('led2');
                const led3 = document.getElementById('led3');

                let currentLED;

                function ledLight() {
                    led1.style.background = 'gray';
                    led2.style.background = 'gray';
                    led3.style.background = 'gray';
                    
                    switch (currentLED) {
                        case "0":
                            led1.style.background = 'yellow';
                            break;
                        case "1":
                            led2.style.background = 'green';
                            break;
                        case "2":
                            led3.style.background = 'red';
                            break;
                    }
                }

                function sendCommand(command) {
                    fetch('/' + command)
                        .then(response => response.text())
                        .then(data => console.log(data))
                        .catch(error => console.error('Error:', error));
                }

                function updateLEDs() {
                    fetch('/led_state')
                        .then(response => response.text())
                        .then(data => {
                            currentLED = data;
                            console.log(data);
                            ledLight();
                        })
                        .catch(error => console.error('Error:', error));
                }

                setInterval(updateLEDs, 100);
            </script>
        </head>

        </html>
    )rawliteral");
}


void handleClick1() 
{
    increaseInterval();
    server.send(200, "text/plain", "Button clicked!");
    Serial.println("Button clicked!");
}

void handleClick2() {
  Serial2.println('C');
  Serial.println("Send command");
}

void sendLedState() {
    server.send(200, "text/plain", String(currentLed));
}

void initWebServer() {
    WiFi.softAP(ssid, password);
    Serial.println("WiFi AP launched!");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, handleRoot);
    server.on("/algo1", HTTP_GET, handleClick1);
    server.on("/algo2", HTTP_GET, handleClick2);
    server.on("/led_state", HTTP_GET, sendLedState);
    server.begin();
}


#endif // WEB_SERVER_H