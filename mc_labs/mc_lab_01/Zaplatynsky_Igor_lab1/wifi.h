#ifndef WIFI_H
#define WIFI_H

const char* ssid = "Kria";
const char* password = "Igorko)))";

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi.");
  } else {
    Serial.println("WiFi connected!");
    Serial.print("Access the site at: http://");
    Serial.println(WiFi.localIP());
  }
}

#endif
