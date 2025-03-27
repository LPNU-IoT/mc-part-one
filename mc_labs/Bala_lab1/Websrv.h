#ifndef WEBSRV_H
#define WEBSRV_H

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern uint32_t ledDelay;

//const char* ssid = "Mi Phone2";
//const char* password = "12345678";

const char* ssid = "__MY__VAULT__";
const char* password = "79715218";

const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML>
    <html>
    
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
            body {
                height: 100vh;  
                display: flex;
                flex-direction: column;
                align-items: center;
                justify-content: center;
                font-family: Arial;
                text-align: center;
                margin: 0px auto;
                padding-top: 30px;
            }
    
            .button {
                padding: 10px 20px;
                font-size: 24px;
                text-align: center;
                outline: none;
                color: #fff;
                background-color: #2f4468;
                border: none;
                border-radius: 5px;
                box-shadow: 0 6px #999;
                cursor: pointer;
                -webkit-tap-highlight-color: rgba(0, 0, 0, 0);
            }
    
            .button:hover {
                background-color: #1f2e45
            }
    
            .button:active {
                background-color: #1f2e45;
                box-shadow: 0 4px #666;
                transform: translateY(2px);
            }
    
            @media screen and (max-width: 480px) {
                .button {
                    padding: 15px 100px 15px 10px;
                    font-size: 10px;
                }
    
                h1 {
                    font-size: 24px;
                    padding-top: 20px;
                }
            }
        </style>
    </head>
    
    <body>
        <h1>ESP Pushbutton Web Server</h1>
        <button class="button" onclick="changeDelay();">Change Delay</button>
        <p id="delayValue">Current Delay: 1000 ms</p>
    
        <script>
            function changeDelay() {
                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/change_delay", true);
                xhr.send();
            }
    
            function getDelayValue() {
                var xhr = new XMLHttpRequest();
                xhr.open("GET", "/get_delay", true);
                xhr.onreadystatechange = function () {
                    if (xhr.readyState === 4 && xhr.status === 200) {
                        document.getElementById('delayValue').innerText = "Current Delay: " + xhr.responseText + " ms";
                    }
                };
                xhr.send();
            }
    
            setInterval(getDelayValue, 1000);
        </script>
    </body>
    
    </html>
    )rawliteral";

AsyncWebServer server(80);

void changeDelay();

void setupWebServer() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html);
    });

    server.on("/change_delay", HTTP_GET, [](AsyncWebServerRequest *request) {
        changeDelay();
        request->send(200, "text/plain", String(ledDelay).c_str());
    });

    server.on("/get_delay", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", String(ledDelay).c_str());
    });

    server.begin();
}

#endif