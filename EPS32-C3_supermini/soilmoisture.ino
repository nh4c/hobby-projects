#include <WiFi.h>

// ======== CONFIG ========
const char* ssid = "YOUR_WIFI_SSID";       // <-- Replace with your WiFi SSID
const char* password = "YOUR_WIFI_PASSWORD"; // <-- Replace with your WiFi password

const int soilPin = 4;  // GPIO4 (ADC1_CH4) for sensor
int soilValue = 0;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("🌱 ESP32-C3 Soil Moisture Web Server Starting...");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi connected!");
  Serial.print("📡 IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  soilValue = analogRead(soilPin); // 0 - 4095
  int percentage = map(soilValue, 4095, 1200, 0, 100); 
  percentage = constrain(percentage, 0, 100);

  // Debug to Serial
  Serial.print("Soil Moisture ADC: ");
  Serial.print(soilValue);
  Serial.print(" | ");
  Serial.print(percentage);
  Serial.println("%");

  // Handle web requests
  WiFiClient client = server.available();
  if (client) {
    String response = "<!DOCTYPE html><html><head>";
    response += "<meta http-equiv='refresh' content='5'>";
    response += "<title>Soil Moisture</title></head><body>";
    response += "<h1>🌱 ESP32-C3 Soil Moisture Monitor</h1>";
    response += "<p>ADC Value: " + String(soilValue) + "</p>";
    response += "<p>Moisture: " + String(percentage) + "%</p>";

    if (percentage < 30) response += "<p>Status: Dry</p>";
    else if (percentage < 70) response += "<p>Status: Moist</p>";
    else response += "<p>Status: Wet</p>";

    response += "<p>(Page auto-refreshes every 5s)</p>";
    response += "</body></html>";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();
    client.println(response);
    client.stop();
  }

  delay(2000);
}
