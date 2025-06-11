const char* ssid = "iPhone de Vitaliy";
const char* password = "iphone14";
const char* mqtt_server = "9a5d1102c4ff49e28175850c39141c2c.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "ESP8266V";
const char* mqtt_pass = "Esp8266V";
const char* mqtt_topic = "emqx/esp8266";

WiFiClientSecure secureClient;
PubSubClient client(secureClient);

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");
}

void setupTime() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for time sync");
  while (time(nullptr) < 100000) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nTime synchronized");
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Received: ");
  Serial.println(message);
  showMessage(message);
}

void connectToMQTT() {
  secureClient.setInsecure();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
      Serial.println("MQTT connected");
      client.subscribe(mqtt_topic);
      client.publish(mqtt_topic, "ESP connected!");
      showConnectingAnimation();
      showWaiting();
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 sec");
      delay(5000);
    }
  }
}
