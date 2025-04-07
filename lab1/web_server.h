#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <WiFi.h>
#include <WebServer.h>

extern void startAlgorithm1();
extern void startAlgorithm2();
extern String getLedState();

WebServer server(80);

const char* ssid = "ESP32";
const char* password = "12345678";

void handleRoot() {
    server.send(200, "text/html", R"rawliteral(
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>ESP32 LED Control</title>
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
                function sendCommand(command) {
                    fetch('/' + command)
                        .then(response => response.text())
                        .then(data => console.log(data))
                        .catch(error => console.error('Error:', error));
                }

                function updateLEDs() {
                    fetch('/led_state')
                        .then(response => response.json())
                        .then(state => {
                            document.getElementById('led1').style.backgroundColor = state.led1 ? 'red' : 'gray';
                            document.getElementById('led2').style.backgroundColor = state.led2 ? 'green' : 'gray';
                            document.getElementById('led3').style.backgroundColor = state.led3 ? 'blue' : 'gray';
                        })
                        .catch(error => console.error('Error:', error));
                }

                setInterval(updateLEDs, 1000);
            </script>
        </head>
        <body>
            <h1>ESP32 LED Control</h1>
            <button onclick="sendCommand('algo1')">Алгоритм 1</button>
            <button onclick="sendCommand('algo2')">Алгоритм 2</button>

            <div class="led-container">
                <div id="led1" class="led"></div>
                <div id="led2" class="led"></div>
                <div id="led3" class="led"></div>
            </div>
        </body>
        </html>
    )rawliteral");
}

void handleAlgo1() {
    startAlgorithm1();
    server.send(200, "text/plain", "Algorithm 1 started");
}

void handleAlgo2() {
    startAlgorithm2();
    server.send(200, "text/plain", "Algorithm 2 started");
}

void handleLedState() {
    server.send(200, "application/json", getLedState());
}

void initWebServer() {
    WiFi.softAP(ssid, password);
    Serial.println("WiFi AP launched!");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, handleRoot);
    server.on("/algo1", HTTP_GET, handleAlgo1);
    server.on("/algo2", HTTP_GET, handleAlgo2);
    server.on("/led_state", HTTP_GET, handleLedState);
    server.begin();
}

#endif // WEB_SERVER_H
