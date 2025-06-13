#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>
#include <DHT.h>

const char* DEVICE_NAME = "Politechnick_tracker_N1";
const char* DEVICE_LOCATION = "LVIV";

// Wi-Fi
const char* ssid = "Ivanna";
const char* password = "ivanna0707";

// MQTT
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "Politechnick_trecker_t_h_Taras/sensors";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Flask Server
const char* serverUrl = "http://192.168.0.105:8000/api/v1/conditions?without_local=true";

// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// DHT11
#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Дані
String localTemperature = "N/A";
String localHumidity = "N/A";
String temperature = "N/A";
String humidity = "N/A";

time_t savedEpoch = 0;
uint32_t lastSyncMillis = 0;
bool wasWiFiConnected = false;

uint32_t lastScreenUpdate = 0;
uint32_t lastWeatherUpdate = 0;
uint32_t lastMQTTPublish = 0;
const uint32_t screenInterval = 3000;
const uint32_t weatherInterval = 60000;
const uint32_t mqttInterval = 10000;

// === Display ===
void initDisplay() {
    Wire.begin();
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("[OLED] Failed to initialize display. Check I2C connection!");
        while (true);
    }
    display.clearDisplay();
    display.display();
    Serial.println("[OLED] Display initialized");
}

void showSensorData() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    display.println("Local Sensor:");
    display.print("Temp: ");
    display.print(localTemperature);
    display.println(" C");
    display.print("Hum:  ");
    display.print(localHumidity);
    display.println(" %");

    display.println("");

    display.println("External:");
    display.print("Temp: ");
    display.print(temperature);
    display.println(" C");
    display.print("Hum:  ");
    display.print(humidity);
    display.println(" %");

    display.display();
}


// === Wi-Fi & Time ===
bool connectToWiFi(uint32_t timeout = 15000) {
    WiFi.begin(ssid, password);
    Serial.print("[WiFi] Connecting to WiFi");
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - start < timeout)) {
        delay(500); Serial.print(".");
    }
    Serial.println(WiFi.status() == WL_CONNECTED ? "\n[WiFi] Connected!" : "\n[WiFi] Failed to connect.");
    return WiFi.status() == WL_CONNECTED;
}

void syncTimeIfConnected() {
    bool now = WiFi.status() == WL_CONNECTED;
    if (now && !wasWiFiConnected) {
        configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
        struct tm timeinfo;
        if (getLocalTime(&timeinfo)) {
            savedEpoch = mktime(&timeinfo);
            lastSyncMillis = millis();
            Serial.println("[Time] Time synchronized");
        }
    }
    wasWiFiConnected = now;
}

// === Sensors & HTTP ===
void readLocalSensor() {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    Serial.printf("[Sensor] Temp: %.2f, Humidity: %.2f\n", t, h);
    if (isnan(t) || isnan(h)) {
        localTemperature = "Err";
        localHumidity = "Err";
        Serial.println("[Sensor] Failed to read from DHT");
    } else {
        localTemperature = String((uint8_t)t);
        localHumidity = String((uint8_t)h);
    }
}

void fetchWeather() {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[HTTP] Fetching weather...");
        HTTPClient http;
        http.begin(espClient, serverUrl);
        int code = http.GET();
        Serial.printf("[HTTP] Response code: %d\n", code);
        if (code == 200) {
            String payload = http.getString();
            StaticJsonDocument<512> doc;
            DeserializationError error = deserializeJson(doc, payload);
            if (error) {
                Serial.print("[HTTP] JSON deserialization failed: ");
                Serial.println(error.c_str());
            } else if (doc.size() > 0 && doc[0]["temperature"] && doc[0]["humidity"]) {
                temperature = String(doc[0]["temperature"].as<float>(), 1);
                humidity = String(doc[0]["humidity"].as<float>(), 1);
                Serial.printf("[HTTP] Remote temp: %s, humidity: %s\n", temperature.c_str(), humidity.c_str());
            } else {
                Serial.println("[HTTP] No valid data in JSON response");
                temperature = "N/A";
                humidity = "N/A";
            }
        } else {
            Serial.println("[HTTP] Failed to get weather");
        }
        http.end();
    } else {
        Serial.println("[HTTP] No WiFi connection");
        temperature = "N/A";
        humidity = "N/A";
    }
}

// === MQTT ===
void reconnectMQTT() {
    while (!mqttClient.connected()) {
        Serial.print("[MQTT] Connecting... ");
        if (mqttClient.connect("ESP8266Client")) {
            Serial.println("connected");
        } else {
            Serial.printf("failed, rc=%d. Retry in 2s\n", mqttClient.state());
            delay(2000);
        }
    }
}

void publishToMQTT() {
    if (!mqttClient.connected()) reconnectMQTT();
    StaticJsonDocument<128> doc;
    doc["device_name"] = DEVICE_NAME;
    doc["location"] = DEVICE_LOCATION;
    doc["temperature"] = localTemperature;
    doc["humidity"] = localHumidity;
    char payload[128];
    serializeJson(doc, payload);
    mqttClient.publish(mqtt_topic, payload);
    Serial.printf("[MQTT] Published: %s\n", payload);
}

// === Main ===
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("[System] Setup started");
    initDisplay();
    dht.begin();
    connectToWiFi();
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    mqttClient.setServer(mqtt_server, mqtt_port);
    fetchWeather();
    Serial.println("[System] Setup complete");
}

void loop() {
    uint32_t now = millis();

    if (now - lastScreenUpdate > screenInterval) {
        readLocalSensor();
        showSensorData();
        lastScreenUpdate = now;
    }

    if (now - lastWeatherUpdate > weatherInterval) {
        fetchWeather();
        lastWeatherUpdate = now;
    }

    if (now - lastMQTTPublish > mqttInterval) {
        publishToMQTT();
        lastMQTTPublish = now;
    }

    syncTimeIfConnected();
    mqttClient.loop();
}
